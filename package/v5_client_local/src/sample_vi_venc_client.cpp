/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <linux/tcp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <setjmp.h>
#include <signal.h>

#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "tcp_states.h"
#include "common.h"
#include "mpp_comm.h"

struct v5_socket_data{
    int frame_type;
    uint64_t pts;
    int endOfFrame;
    int endOfStream;
    uint64_t len;
    int offset;
    int start;
    uint64_t cksum;
    unsigned char data[1000];
};

static unsigned char * tmpFrameBox;
static unsigned char * tmpBuf;
static MPPCallbackInfo  g_cb_info;
static DEMUX_INFO_S     g_demux_info;
static DEMUX_CB_PARAM_S g_demux_param;

static MUX_CB_PARAM_S  g_mux_param;
static MPPCallbackInfo g_mux_cb;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H265;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

//static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_1080P_30FPS;
static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;

//static VENC_CFG_TYPE_E g_venc_type_0 = VENC_1080P_TO_1080P_8M_30FPS;
static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;

static int serverPid = 0;
static int needRestart = 1;
#define HLAY(chn, lyl) (chn*4+lyl)

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

int connectServer(int port){
    int sockfd = 0 , flags = 0, ret = 0, error = 0;
    fd_set rset, wset;
    char recvBuff[1024];
    struct sockaddr_in serv_addr,client_addr;
    struct timeval ts;
    socklen_t len = sizeof(error);

    ts.tv_sec = 2;

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error : Could not create socket");
        return -1;
    }

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;
 
    if( (flags = fcntl(sockfd, F_GETFL, 0 )) < 0 )return -1;

    if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)return -1;

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, "192.168.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("inet_pton error occured \n");
        goto error;
    }
    if( (ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        if(errno != EINPROGRESS) {
           printf("Error : Connect Failed");
            return -1;
        }
    }

    if(ret == 0){
        errno = ETIMEDOUT;
        goto error;
    }

    if(ret == 0){
        errno = ETIMEDOUT;
        goto error;
    }

    if(FD_ISSET(sockfd,&rset) || FD_ISSET(sockfd, &wset)){
        if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error,&len) < 0)goto error;
    }else goto error;

    if(error){
        errno = error;
        goto error;
    }

    if(fcntl(sockfd,F_SETFL,flags) < 0) goto error;

    return sockfd;
error:
    return -1;
}

unsigned checksum(void *buffer, uint64_t len, unsigned int seed)
{
      unsigned char *buf = (unsigned char *)buffer;
      size_t i;

      for (i = 0; i < len; ++i)
            seed += (unsigned int)(*buf++);

      return seed;
}

static void getDataToVDEC(){
    static int tryCount = 0;
    static uint64_t totalLen = 0, cksum = 0;
    struct v5_socket_data info;
    unsigned char *box = tmpFrameBox;
    static VDEC_STREAM_S vdec_stream = {0};
    int ret = 0;
    int cc[] = {1};

    memset(tmpBuf, 0, sizeof(struct v5_socket_data));
    if(recv( serverPid, tmpBuf, sizeof(struct v5_socket_data), 0) <= 0){
    	    printf("%s(%d)\n", __func__, __LINE__);
	    if(serverPid > 0)	
	    close(serverPid);
	    serverPid = -1;
	 return;
    }
    setsockopt(serverPid, IPPROTO_TCP, TCP_QUICKACK, cc, sizeof(int));

    memcpy(&info, tmpBuf, sizeof(struct v5_socket_data));
    usleep(10);
    if(info.len <= 0 || info.pts <= 0 || info.endOfFrame < 0 || info.endOfFrame > 1 || info.offset > 1500){
	printf("%s(%d) ---------------write Frame----------------------\n", __func__, __LINE__);
	printf("%s(%d) -------------- vdec_stream.mPTS : %lld--------------\n", __func__, __LINE__, info.pts);
	printf("%s(%d) ---------------vdec_stream.mLen : %lld----------------------\n", __func__, __LINE__, info.len);
	printf("%s(%d) ---------------totalLen : %lld----------------------\n", __func__, __LINE__, totalLen);
	printf("%s(%d) ---------------needRestart : %d offset : %d----------------------\n\n", __func__, __LINE__, info.endOfFrame, info.offset);
        printf("%s(%d) drop %d\n", __func__, __LINE__, tryCount);
	
	if(++tryCount > 5) {
    	    printf("%s(%d)\n", __func__, __LINE__);
	    close(serverPid);
	    serverPid = -1;
	    tryCount = 0;
	}
	memset(&info, 0, sizeof(v5_socket_data));
	memset(&vdec_stream, 0, sizeof(VDEC_STREAM_S));
	needRestart = 1;
	totalLen = 0;
	return;
    }

    if(needRestart == 1 && info.start != 1){
        printf("%s(%d) skip package\n", __func__, __LINE__);
	return;
    }
	
    if(info.start == 1){
	if( vdec_stream.mLen > 0 && needRestart != 1){
	    if(totalLen == vdec_stream.mLen){
	        vdec_stream.pAddr = tmpFrameBox;
/*
*/
		if(cksum != checksum(tmpFrameBox, vdec_stream.mLen, 0))return;

	        ret = AW_MPI_VDEC_SendStream(VDEC_CHN_0, &vdec_stream, -1);
                if (ret){
		    printf("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
		    ERR_PRT("Do AW_MPI_VDEC_SendStream fail! ret:%d\n", ret);
	        }
	    } else {
		printf("%s(%d) bad package\n", __func__, __LINE__);
	    }
	}
        needRestart = 0;
	tryCount = 0;
	vdec_stream.mPTS = info.pts;
	vdec_stream.mLen = info.len;
	vdec_stream.mbEndOfStream = FALSE;
	cksum = info.cksum;

	if(info.endOfFrame == 1){
            vdec_stream.mbEndOfFrame = TRUE;
        } else {
            vdec_stream.mbEndOfFrame = FALSE;
        }
        totalLen = 0;
    } 
	if(info.start != 0 && info.start != 1){
    	    printf("%s(%d)\n", __func__, __LINE__);
		 return;
	}
        box += totalLen;
        totalLen += info.offset;
        memcpy(box, (unsigned char *)&info.data, info.offset);
}

static void *SendStream_1(void *param)
{
    fd_set fds;
    struct timeval timeout;
    int trycount = 0;

    if(tmpBuf == NULL){
        tmpBuf = (unsigned char *)malloc(sizeof(v5_socket_data));
	if(tmpBuf == NULL) {
	    printf("craete memory fail\n");
	    return NULL;
	}
        memset(tmpBuf, 0, sizeof(v5_socket_data));
    }

    if(tmpFrameBox == NULL){
	tmpFrameBox = (unsigned char *)malloc(4096 * 2160 * 3);
	if(tmpFrameBox == NULL) {
	    printf("craete memory fail\n");
	    return NULL;
	}
    }

    while (1)
    {
	timeout.tv_sec = 0;
        timeout.tv_usec = 10;

	if(trycount >= 10){
	     trycount = 0;
	     if(serverPid > 0 ) {
		//close(serverPid);
		//serverPid = -1;
	    }
	}

	if(serverPid  <= 0){
    	    printf("%s(%d)\n", __func__, __LINE__);
	    serverPid = connectServer(12345);
	}
	if(serverPid <= 0){
    	    printf("%s(%d)\n", __func__, __LINE__);
	    sleep(5);
	    continue;
	}

        FD_ZERO(&fds);
        FD_SET(serverPid, &fds);

        //printf("%s(%d) serverPid : %d\n", __func__, __LINE__,serverPid);
        if(select(serverPid + 1, &fds, NULL, NULL, &timeout) <= 0){
    	    //printf("%s(%d)\n", __func__, __LINE__);
	    trycount++;
	    continue;
	}

	if(FD_ISSET(serverPid, &fds)){	
	    trycount = 0;
	    getDataToVDEC();
	}
    }
    printf("Out this function ... ... \n");
    return NULL;
}

static int thread_start()
{
    int ret = 0;
    pthread_t id; 

    ret = pthread_create(&id, NULL, SendStream_1, &ret);
    if (ret)
    {
       ERR_PRT("Do pthread_create fail! ret:%d\n", ret);
    }

    return 0;
}

static int g_yuv_run_flag  = 1;
static int g_yuv_save_flag = 1;

static int vo_create(void)
{
    int ret = 0;
    VO_DEV_TYPE_E vo_type;
    VO_DEV_CFG_S  vo_cfg;
    vo_type           = VO_DEV_HDMI;
    vo_cfg.res_width  = 1920;
    vo_cfg.res_height = 1080;
    ret = mpp_comm_vo_dev_create(vo_type, &vo_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_dev_create fail! ret:%d  vo_type:%d\n", ret, vo_type);
        return -1;
    }

    VO_CHN_CFG_S vo_chn_cfg;
    vo_chn_cfg.top        = 0;
    vo_chn_cfg.left       = 0;
    vo_chn_cfg.width      = 1920;
    vo_chn_cfg.height     = 1080;
    vo_chn_cfg.vo_buf_num = 0;
    ret = mpp_comm_vo_chn_create(VO_CHN_0, &vo_chn_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_chn_create fail! ret:%d  vo_chn:%d\n", ret, VO_CHN_0);
        return -1;
    }
    /* Create Clock for vo mode */
    ret = mpp_comm_vo_clock_create(CLOCK_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_clock_create fail! ret:%d  clock_chn:%d\n", ret, CLOCK_CHN_0);
        return -1;
    }
    return 0;
}

static int components_bind(void)
{
    int ret = 0;

    ret = mpp_comm_vdec_bind_vo(VDEC_CHN_0, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_bind_vo fail! ret:%d vdec_chn:%d vo_chn:%d\n", ret, VDEC_CHN_0, VO_CHN_0);
        return -1;
    }

    ret = mpp_comm_vo_bind_clock(CLOCK_CHN_0, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_bind_clock fail! ret:%d clock_chn:%d vo_chn:%d\n", ret, CLOCK_CHN_0, VO_CHN_0);
        return -1;
    }

    return 0;
}

static int components_start(void)
{
    int ret = 0;

    ret = AW_MPI_CLOCK_Start(CLOCK_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_CLOCK_Start fail! clock_chn:%d ret:%d\n", CLOCK_CHN_0, ret);
        return -1;
    }

    ret = AW_MPI_VDEC_StartRecvStream(VDEC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VDEC_StartRecvStream fail! vdec_chn:%d ret:%d\n", VDEC_CHN_0, ret);
        return -1;
    }

    ret = mpp_comm_vo_chn_start(VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_chn_start vo_chn:%d fail! ret:%d\n", VO_CHN_0, ret);
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    printf("\033[2J");
    printf("\n\n\nDo sample vi+venc+rtsp. default:VI_4K@25fps+VENC(4K@25fps+720P@25fps) H264/CBR/MainProfile\n");

    while((serverPid = connectServer(12345)) < 0){
	printf("socket fail, wait try....");
	sleep(1);
    }

    /* Init mpp system */
    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

    memset(&g_cb_info,    0, sizeof(MPPCallbackInfo));
    memset(&g_demux_info, 0, sizeof(DEMUX_INFO_S));
    g_cb_info.cookie   = NULL;
    g_cb_info.callback = mpp_comm_demux_callbcak;
    VDEC_CFG_S vdec_cfg;
    DB_PRT("Demux info: w:%d h:%d codec_type:%d\n", g_demux_info.width, g_demux_info.height, g_demux_info.codec_type);
    vdec_cfg.width     = 3840;
    vdec_cfg.height     = 2160;
    vdec_cfg.codec_type = g_venc_type;
    vdec_cfg.rotation   = g_rotate;
    ret = mpp_comm_vdec_create(VDEC_CHN_0, &vdec_cfg, &g_cb_info);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_create fail!  vdec_chn:%d  ret:%d\n", VDEC_CHN_0, ret);
    }

    ret = vo_create();
    if (ret)
    {
        ERR_PRT("Do vo_create fail! ret:%d\n", ret);
    }

    /* Bind */

    ret = components_bind();
    if (ret)
    {
        ERR_PRT("Do components_bind fail! ret:%d\n", ret);
    }

    /* Start */
    ret = components_start();
    if (ret)
    {
        ERR_PRT("Do components_start fail! ret:%d\n", ret);
    }

    thread_start();
    
    while(1){
	sleep(1);
    }

    return ret;
}

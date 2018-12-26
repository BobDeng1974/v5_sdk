/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <mpi_videoformat_conversion.h>

#include <linux/tcp.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "mpp_comm.h"
#include "tcp_states.h"

#define _DEBUG_MSG(e,...) fprintf(stderr, "[%s:%d] " e "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define HLAY(chn, lyl) (chn*4+lyl)

//#define VO_ENABLE

struct v5_socket_data{
    int frame_type;
    uint64_t pts;
    int endOfFrame;
    int endOfStream;
    unsigned int len;
    int offset;
    int start;
    uint64_t cksum;
    unsigned char data[1000];
};

#define MAXNO(__A__, __B__) ((__A__ > __B__) ? __A__ : __B__)

static MPPCallbackInfo  g_cb_info;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H265;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;

static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;
int clientPid = -1;

const char *endFlag = "$$";
const char *startFlag = "##";

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
}

int user_connect (int server)
{
    struct sockaddr_in cc;
    int addr_len = sizeof(cc);

    int result = -1;
    struct sockaddr_in caddr;
    struct timeval tv;

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    int clientIndex = 0;

    int optval = 1;
    result = accept( server, (struct sockaddr*)&caddr, (socklen_t *)&addr_len);
    if (!strcmp(inet_ntoa(caddr.sin_addr), "0.0.0.0")) return -1;

    if(setsockopt(result, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
        printf("set SO_RCVTIMEO ERROR\n");
    if(setsockopt(result, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
        printf("set SO_SNDTIMEO ERROR\n");

    return result;
}

int socket_server(unsigned short int server_port){
    struct sockaddr_in saddr;
    int server_pid = -1;
    int optval = 1;

    server_pid = socket( AF_INET, SOCK_STREAM, 0 ); 
    if(server_pid < 0)
    {
        printf("TCP Server start Error!!\n");
        return -1;
    }

    memset( &saddr , 0 , sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons( server_port );
    saddr.sin_addr.s_addr = INADDR_ANY;

    //if(setsockopt( server_pid, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 ) return -1;
    if(setsockopt( server_pid, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval)) < 0 ){
	printf("%s(%d) set socket no delay fail\n", __func__, __LINE__);
    }

    while (bind( server_pid , (struct sockaddr *)&saddr , 16 ) < 0)
    {
        printf("TCP Server set fail\n");
        printf("Tcp Server Port[ %d ] locking.. Waitting... \n", server_port);
        sleep(1);
    }

    if (listen(server_pid , 5) < 0)
    {
        printf("TCP Server listen fail !! \n");
        return -1;
    }
    return server_pid; 
}

unsigned checksum(void *buffer, uint64_t len, unsigned int seed)
{
      unsigned char *buf = (unsigned char *)buffer;
      size_t i;

      for (i = 0; i < len; ++i)
            seed += (unsigned int)(*buf++);

      return seed;
}

static void sendToSocket(unsigned char *framData, unsigned int framLen, int frame_type, uint64_t pts){

    struct v5_socket_data info;
    unsigned int len = framLen;
    int offset = 0, ret = 0;
    unsigned char *data = framData;
    unsigned char buf[1500] = {0};
    unsigned char *ptr = NULL;

    info.frame_type = frame_type;

    if(frame_type == 1)
	info.endOfFrame = 0;	
    else 
	info.endOfFrame = 1;	

    info.len = framLen;
    info.pts = pts;
    info.endOfStream = 0;
    info.start = 1;
    //info.cksum = checksum(framData, framLen, 0);
    info.cksum = 0;
    printf("New Package(%d)\n", sizeof(info));

    do{
        data += offset;
        if(len > sizeof(info.data))
            offset = sizeof(info.data);
	else 
	    offset = len;

        memcpy(&info.data, data, offset);
        info.offset = offset;

        ptr = (unsigned char *)&buf;
        memcpy(ptr, startFlag, sizeof(char) * strlen(startFlag));
        ptr += sizeof(char) * strlen(startFlag);

        memcpy(ptr, &info.frame_type, sizeof(int));
        ptr += sizeof(int);

        memcpy(ptr, &info.pts, sizeof(uint64_t));
        ptr += sizeof(uint64_t);

        memcpy(ptr, &info.endOfFrame, sizeof(int));
        ptr += sizeof(int);

        memcpy(ptr, &info.endOfStream, sizeof(int));
        ptr += sizeof(int);

        memcpy(ptr, &info.len, sizeof(unsigned int));
        ptr += sizeof(unsigned int);

        memcpy(ptr, &info.offset, sizeof(int));
        ptr += sizeof(int);

        memcpy(ptr, &info.start, sizeof(int));
        ptr += sizeof(int);
/*
        memcpy(ptr, &info.cksum, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
*/
        memcpy(ptr, &info.data, sizeof(info.data));
        ptr += sizeof(info.data);

        memcpy(ptr, endFlag, sizeof(char) * strlen(endFlag));

        if((ret = send(clientPid, buf, sizeof(buf), 0)) <= 0){
	    printf("Client disconnect!!!! [%s][%d]\n\n", strerror(errno), errno);
	    if(ret == EINTR)continue;
	    else if(ret == EAGAIN){
		continue;
	    } else {
	    	close(clientPid);
	    	clientPid = -1;
	    }
            break;
        }

        info.start = 0;
        len -= offset;
    }while(len > 0);
}

static void *SendStream_1(void *param)
{
    int            ret = 0;
    unsigned char *buf = NULL;
    unsigned int   len = 0;
    uint64_t       pts = 0;
    int            frame_type = 0;
    uint64_t curPts = 0;
    VencHeaderData head_info;
    VIDEO_FRAME_INFO_S       video_frame = {0};
    VDEC_STREAM_S vdec_stream = {0};
    struct timeval tv;
    uint64_t timeStamp = 0, sleepTime = 0;
    int isFirst = 1;

    while (1)
    {
        ret = mpp_comm_venc_get_stream(VENC_CHN_0, g_venc_type, -1, &buf, &len, &pts, &frame_type, &head_info);
	if(clientPid > 0){
	    if(isFirst == 0 || frame_type == 1){
	        if(frame_type == 1){
	            sendToSocket(head_info.pBuffer, head_info.nLength, frame_type, pts);
	    	}
	    	sendToSocket(buf, len, frame_type, pts);
		isFirst = 0;
	    } else {
		isFirst = 1;
	    }
	}
	usleep(1000);
    }
    printf("Out this function ... ... \n");
    return NULL;
}

static int thread_start()
{
    int ret = 0;
    pthread_t id = 0;

    ret = pthread_create(&id, NULL, SendStream_1, &ret);
    if (ret)
    {
       ERR_PRT("Do pthread_create fail! ret:%d\n", ret);
    }

    return 0;
}

static int vi_create_new(void){
    int ret = 0;
    VI_ATTR_S stAttr;

    ret = AW_MPI_VI_CreateVipp(VI_DEV_0);
    if (ret != SUCCESS){
        printf("fatal error! AW_MPI_VI CreateVipp failed");
    }

    ret = AW_MPI_VI_GetVippAttr(VI_DEV_0, &stAttr);
    if (ret != SUCCESS){
        printf("fatal error! AW_MPI_VI GetVippAttr failed");
    }

    memset(&stAttr, 0, sizeof(VI_ATTR_S));
    stAttr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    stAttr.memtype = V4L2_MEMORY_MMAP;
    stAttr.format.pixelformat = map_PIXEL_FORMAT_E_to_V4L2_PIX_FMT(MM_PIXEL_FORMAT_YVU_SEMIPLANAR_420);
    stAttr.format.field = V4L2_FIELD_NONE;
    stAttr.format.width = 3840;
    stAttr.format.height = 2160;
    stAttr.nbufs = 10;
    stAttr.nplanes = 2;
    stAttr.use_current_win = 0;
    stAttr.fps = 30;

    ret = AW_MPI_VI_SetVippAttr(VI_DEV_0, &stAttr);
    if (ret != SUCCESS){
        printf("fatal error! AW_MPI_VI SetVippAttr failed");
    }

    ret = AW_MPI_VI_EnableVipp(VI_DEV_0);
    if(ret != SUCCESS){
        printf("fatal error! enableVipp fail!");
    }

    ret = AW_MPI_VI_CreateVirChn(VI_DEV_0, VI_CHN_0, NULL);
    if(ret != SUCCESS){
        printf("fatal error! createVirChn[%d] fail!", VI_CHN_0);
    }

    ret = AW_MPI_VI_CreateVirChn(VI_DEV_0, VI_CHN_1, NULL);
    if(ret != SUCCESS){
        printf("fatal error! createVirChn[%d] fail!", VI_CHN_0);
    }
    return ret;
}


static int vi_create(void)
{
    int ret = 0;
    VI_ATTR_S vi_attr;

    ret = mpp_comm_vi_get_attr(g_vi_type_0, &vi_attr);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_get_attr fail! ret:%d \n", ret);
        return -1;
    }
    vi_attr.use_current_win = 0;
    vi_attr.wdr_mode = 1;
    vi_attr.capturemode = V4L2_MODE_VIDEO; /* V4L2_MODE_VIDEO; V4L2_MODE_IMAGE; V4L2_MODE_PREVIEW */

    ret = mpp_comm_vi_dev_create(VI_DEV_0, &vi_attr);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_dev_create fail! ret:%d  vi_dev:%d\n", ret, VI_DEV_0);
        return -1;
    }

    ret = mpp_comm_vi_chn_create(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_create fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_0, VI_CHN_0);
        return -1;
    }

#ifdef VO_ENABLE
    ret = mpp_comm_vi_chn_create(VI_DEV_0, VI_CHN_1);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_create fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_0, VI_CHN_1);
        return -1;
    }
#endif
    return 0;
}

static int vo_create_new(void){
    int ret = 0;
    int mVoLayer = 0;
    int hlay0 = 0;
    AW_MPI_VO_Enable(0);
    AW_MPI_VO_AddOutsideVideoLayer(HLAY(2, 0));
    AW_MPI_VO_CloseVideoLayer(HLAY(2, 0)); /* close ui layer. */

    while(hlay0 < VO_MAX_LAYER_NUM)
    {
        if(SUCCESS == AW_MPI_VO_EnableVideoLayer(hlay0))
        {
            break;
        }
        hlay0++;
    }
    if(hlay0 >= VO_MAX_LAYER_NUM)
    {
        printf("fatal error! enable video layer fail!");
    }

    VO_PUB_ATTR_S spPubAttr;
    AW_MPI_VO_GetPubAttr(0, &spPubAttr);
    spPubAttr.enIntfType = VO_INTF_HDMI;
    spPubAttr.enIntfSync = VO_OUTPUT_3840x2160_30;
    AW_MPI_VO_SetPubAttr(0, &spPubAttr);

    VO_VIDEO_LAYER_ATTR_S mLayerAttr;
    mVoLayer = hlay0;
    AW_MPI_VO_GetVideoLayerAttr(mVoLayer, &mLayerAttr);
    mLayerAttr.stDispRect.X = 0;
    mLayerAttr.stDispRect.Y = 0;
    mLayerAttr.stDispRect.Width = 3840;
    mLayerAttr.stDispRect.Height = 2160;
    AW_MPI_VO_SetVideoLayerAttr(mVoLayer, &mLayerAttr);

    ret = AW_MPI_VO_EnableChn(mVoLayer, 0);
    if(SUCCESS == ret){
        printf("create vo channel[0] success!");
    }
    else if(ERR_VO_CHN_NOT_DISABLE == ret){
        printf("vo channel[0] is exist, find next!");
    } else {
        printf("fatal error! create vo channel[0] ret[0x%x]!", ret);
    }
 
    MPPCallbackInfo cbInfo;
    cbInfo.cookie = NULL;
    cbInfo.callback = (MPPCallbackFuncType)&MPPCallbackFunc;
    AW_MPI_VO_RegisterCallback(mVoLayer, 0, &cbInfo);
    AW_MPI_VO_SetChnDispBufNum(mVoLayer, 0, 2);
}

static int vo_create(void)
{
    int ret = 0;
    VO_DEV_TYPE_E vo_type;
    VO_DEV_CFG_S  vo_cfg;
    vo_type           = VO_DEV_HDMI;
    vo_cfg.res_width  = 3840;
    vo_cfg.res_height = 2160;
    ret = mpp_comm_vo_dev_create(vo_type, &vo_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_dev_create fail! ret:%d  vo_type:%d\n", ret, vo_type);
        return -1;
    }

    VO_CHN_CFG_S vo_chn_cfg;
    vo_chn_cfg.top        = 0;
    vo_chn_cfg.left       = 0;
    vo_chn_cfg.width      = 3840;
    vo_chn_cfg.height     = 2160;
    vo_chn_cfg.vo_buf_num = 0;
    ret = mpp_comm_vo_chn_create(VO_CHN_0, &vo_chn_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_chn_create fail! ret:%d  vo_chn:%d\n", ret, VO_CHN_0);
        return -1;
    }

    return 0;
}

static int venc_create(void)
{
    int ret = 0;

    VENC_CFG_S venc_cfg  = {0};

    ret = mpp_comm_venc_get_cfg(g_venc_type_0, &venc_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_get_cfg fail! ret:%d \n", ret);
        return -1;
    }
    ret = mpp_comm_venc_create(VENC_CHN_0, g_venc_type, g_rc_mode, g_profile, g_rotate, &venc_cfg);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_create fail! ret:%d \n", ret);
        return -1;
    }


    /* Register Venc call back function */
    MPPCallbackInfo venc_cb;
    venc_cb.cookie   = NULL;
    venc_cb.callback = (MPPCallbackFuncType)&MPPCallbackFunc;

    AW_MPI_VENC_RegisterCallback(VENC_CHN_0, &venc_cb);

    return 0;
}

static int components_bind(void)
{
    int ret = 0;

    ret = mpp_comm_vi_bind_venc(VI_DEV_0, VI_CHN_0, VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_bind_venc fail! ret:%d \n", ret);
        return -1;
    }

#ifdef VO_ENABLE
#if 0
    MPP_CHN_S VOChn = {MOD_ID_VOU, 0, 0};
    MPP_CHN_S VIChn = {MOD_ID_VIU, 0, 0};
    AW_MPI_SYS_Bind(&VIChn, &VOChn);
#else
    ret = mpp_comm_vi_bind_vo(VI_DEV_0, VI_CHN_1, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_create fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_0, VI_CHN_1);
        return -1;
    }
#endif
#endif
    return 0;
}

static int components_start(void)
{
    int ret = 0;
    ret = AW_MPI_VI_EnableVirChn(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_EnableVirChn fail! ret:%d\n", ret);
        return -1;
    }

    ret = AW_MPI_VI_EnableVirChn(VI_DEV_0, VI_CHN_1);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_EnableVirChn fail! ret:%d\n", ret);
        return -1;
    }

    ret = AW_MPI_VENC_StartRecvPic(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StartRecvPic fail! ret:%d\n", ret);
        return -1;
    }
#ifdef VO_ENABLE
    ret = mpp_comm_vo_chn_start(VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vo_chn_start vo_chn:%d fail! ret:%d\n", VO_CHN_0, ret);
        return -1;
    }
#endif
    return 0;
}

int main(int argc, char **argv)
{
    int ret = 0;
    int sockFd = 0;
    int d_val;
    int maxfd = 0;
    fd_set fds;

    sockFd = socket_server(12345);
    if(sockFd < 0){
	printf("socket fail");
	return -1;
    }

    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

    ret = vi_create_new();
    if (ret)
    {
        ERR_PRT("Do vi_create fail! ret:%d\n", ret);
    }

    ret = venc_create();
    if (ret)
    {
        ERR_PRT("Do venc_create fail! ret:%d\n", ret);
    }
#ifdef VO_ENABLE
    ret = vo_create_new();
    if (ret)
    {
        ERR_PRT("Do vo_create fail! ret:%d\n", ret);
    }
#endif
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

    while (1) {
	FD_ZERO(&fds);
        maxfd = 0;
        FD_SET(sockFd, &fds);
        maxfd = MAXNO(sockFd, maxfd);
 	select(maxfd + 1, &fds, NULL, NULL, NULL);
	if(FD_ISSET(sockFd, &fds)){
	    if(clientPid > 0)close(clientPid);
            clientPid = user_connect(sockFd);
	    printf("User Connect : %d\n", clientPid);
        }
    }
    return ret;
}


/************************************************************************************************/
/*                                      Include Files                                           */
/************************************************************************************************/

#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "mpp_comm.h"

struct vi_venc_param {
    pthread_t       thd_id;
    int             venc_chn;
    int             run_flag;
    PAYLOAD_TYPE_E  venc_type;
};


/************************************************************************************************/
/*                                      Global Variables                                        */
/************************************************************************************************/

static MPPCallbackInfo  g_cb_info;
static DEMUX_INFO_S     g_demux_info;
static DEMUX_CB_PARAM_S g_demux_param;

static MUX_CB_PARAM_S  g_mux_param;
static MPPCallbackInfo g_mux_cb;

static struct vi_venc_param g_param_0;
static struct vi_venc_param g_param_1;

static unsigned int   g_rc_mode   = 0;          /* 0:CBR 1:VBR 2:FIXQp 3:ABR 4:QPMAP */
static unsigned int   g_profile   = 1;          /* 0: baseline  1:MP  2:HP  3: SVC-T [0,3] */
static PAYLOAD_TYPE_E g_venc_type = PT_H265;    /* PT_H264/PT_H265/PT_MJPEG */
static ROTATE_E       g_rotate    = ROTATE_NONE;

static MPP_COM_VI_TYPE_E g_vi_type_0 = VI_4K_30FPS;
static MPP_COM_VI_TYPE_E g_vi_type_1 = VI_720P_30FPS;

static VENC_CFG_TYPE_E g_venc_type_0 = VENC_4K_TO_4K_20M_30FPS;
static VENC_CFG_TYPE_E g_venc_type_1 = VENC_720P_TO_720P_4M_25FPS;

/************************************************************************************************/
/*                                    Function Declarations                                     */
/************************************************************************************************/
/* None */
#define HLAY(chn, lyl) (chn*4+lyl)

/************************************************************************************************/
/*                                     Function Definitions                                     */
/************************************************************************************************/

static ERRORTYPE MPPCallbackFunc(void *cookie, MPP_CHN_S *pChn, MPP_EVENT_TYPE event, void *pEventData)
{
    DB_PRT(" pChn:%d  event:%d \n", pChn->mModId, (int)event);

    return SUCCESS;
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

static int vi_create(void)
{
    int ret = 0;
    VI_ATTR_S vi_attr;

    /**  create vi dev 2  src 1080P **/
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

    return 0;
}


static int vi_destroy(void)
{
    int ret = 0;

    /**  destroy vi dev 2  **/
    ret = mpp_comm_vi_chn_destroy(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_chn_destroy fail! ret:%d  vi_dev:%d  vi_chn:%d \n", ret, VI_DEV_0, VI_CHN_0);
    }
    ret = mpp_comm_vi_dev_destroy(VI_DEV_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_dev_destroy fail! ret:%d  vi_dev:%d \n", ret, VI_DEV_0);
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

static int venc_destroy(void)
{
    int ret = 0;

    ret = mpp_comm_venc_destroy(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_venc_destroy fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }

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

    AW_MPI_SYS_Bind(VENC_CHN_0, VDEC_CHN_0);

    ret = mpp_comm_vdec_bind_vo(VDEC_CHN_0, VO_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_bind_vo fail! ret:%d vdec_chn:%d vo_chn:%d\n", ret, VDEC_CHN_0, VO_CHN_0);
        return -1;
    }

    return 0;
}


static int components_unbind(void)
{
    int ret = 0;

    ret = mpp_comm_vi_unbind_venc(VI_DEV_0, VI_CHN_0, VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vi_unbind_venc fail! ret:%d\n", ret);
    }

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

    ret = AW_MPI_VENC_StartRecvPic(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StartRecvPic fail! ret:%d\n", ret);
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

static int components_stop(void)
{
    int ret = 0;

    ret = AW_MPI_VENC_StopRecvPic(VENC_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VENC_StopRecvPic fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }

    ret = AW_MPI_VI_DisableVirChn(VI_DEV_0, VI_CHN_0);
    if (ret)
    {
        ERR_PRT("Do AW_MPI_VI_DisableVirChn fail! ret:%d  venc_chn:%d \n", ret, VENC_CHN_0);
    }

    return 0;
}


int main(int argc, char **argv)
{
    int ret = 0;
    int sockFd = 0;

    /* Init mpp system */
    ret = mpp_comm_sys_init();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_init fail! ret:%d \n", ret);
        return -1;
    }

    /* Get vi config to create 1080P/720P vi channel */
    ret = vi_create();
    if (ret)
    {
        ERR_PRT("Do vi_create fail! ret:%d\n", ret);
        goto _exit_1;
    }

    /* Create vo device and layer */
    ret = vo_create();
    if (ret)
    {
        ERR_PRT("Do vo_create fail! ret:%d\n", ret);
        goto _exit_1;
    }

    /* Get venc config to create 1080P/720P venc channel */

    ret = venc_create();
    if (ret)
    {
        ERR_PRT("Do venc_create fail! ret:%d\n", ret);
        goto _exit_2;
    }

    /* Create vdec channel */

    memset(&g_cb_info,    0, sizeof(MPPCallbackInfo));
    memset(&g_demux_info, 0, sizeof(DEMUX_INFO_S));
    g_cb_info.cookie   = NULL;
    g_cb_info.callback = mpp_comm_demux_callbcak;
    VDEC_CFG_S vdec_cfg;
    DB_PRT("Demux info: w:%d h:%d codec_type:%d\n", g_demux_info.width, g_demux_info.height, g_demux_info.codec_type);
    vdec_cfg.height     = 3840;
    vdec_cfg.height     = 2160;
    vdec_cfg.codec_type = g_venc_type;
    vdec_cfg.rotation   = g_rotate;
    ret = mpp_comm_vdec_create(VDEC_CHN_0, &vdec_cfg, &g_cb_info);
    if (ret)
    {
        ERR_PRT("Do mpp_comm_vdec_create fail!  vdec_chn:%d  ret:%d\n", VDEC_CHN_0, ret);
        goto _exit_2;
    }


    /* Bind */

    ret = components_bind();
    if (ret)
    {
        ERR_PRT("Do components_bind fail! ret:%d\n", ret);
        goto _exit_3;
    }

    /* Start */
    ret = components_start();
    if (ret)
    {
        ERR_PRT("Do components_start fail! ret:%d\n", ret);
        goto _exit_4;
    }

    while (1) {
	sleep(1);
    }

_exit_4:
    components_stop();

_exit_3:
    components_unbind();

_exit_2:
    venc_destroy();

_exit_1:
    vi_destroy();

    ret = mpp_comm_sys_exit();
    if (ret)
    {
        ERR_PRT("Do mpp_comm_sys_exit fail! ret:%d \n", ret);
    }

    return ret;
}


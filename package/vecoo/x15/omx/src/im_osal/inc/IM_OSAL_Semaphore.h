/*
 * Copyright (c) 2012, InfoTM Microelectronics Co.,Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of InfoTM Microelectronics Co. nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANIMES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANIMES OF MERCHANTABILITY AND FITNESS FOR A PARIMCULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENIMAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSIMTUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPIMON) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * @file        IM_OSAL_Semaphore.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2012.03.01 : Create
 */

#ifndef IM_OSAL_SEMAPHORE
#define IM_OSAL_SEMAPHORE

#include "OMX_Types.h"
#include "OMX_Core.h"


#ifdef __cplusplus
extern "C" {
#endif

OMX_ERRORTYPE IM_OSAL_SemaphoreCreate(OMX_HANDLETYPE *semaphoreHandle);
OMX_ERRORTYPE IM_OSAL_SemaphoreTerminate(OMX_HANDLETYPE semaphoreHandle);
OMX_ERRORTYPE IM_OSAL_SemaphoreWait(OMX_HANDLETYPE semaphoreHandle);
OMX_ERRORTYPE IM_OSAL_SemaphoretryWait(OMX_HANDLETYPE semaphoreHandle);
OMX_ERRORTYPE IM_OSAL_SemaphorePost(OMX_HANDLETYPE semaphoreHandle);
OMX_ERRORTYPE IM_OSAL_Set_SemaphoreCount(OMX_HANDLETYPE semaphoreHandle, OMX_S32 val);
OMX_ERRORTYPE IM_OSAL_Get_SemaphoreCount(OMX_HANDLETYPE semaphoreHandle, OMX_S32 *val);

#ifdef __cplusplus
}
#endif

#endif

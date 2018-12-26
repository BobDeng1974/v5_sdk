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
 * *  Neither the name of  InfoTM Microelectronics Co. nor the names of
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
 * @file        IM_OSAL_ETC.h
 * @brief
 * @author      ayakashi (yanyuan_xu@infotm.com)
 * @version     1.0
 * @history
 *   2010.03.01 : Create
 */

#ifndef IM_OSAL_ETC
#define IM_OSAL_ETC

#include "OMX_Types.h"


#ifdef __cplusplus
extern "C" {
#endif

OMX_PTR IM_OSAL_Strcpy(OMX_PTR dest, OMX_PTR src);
OMX_PTR IM_OSAL_Strncpy(OMX_PTR dest, OMX_PTR src, size_t num);
OMX_S32 IM_OSAL_Strcmp(OMX_PTR str1, OMX_PTR str2);
OMX_PTR IM_OSAL_Strcat(OMX_PTR dest, OMX_PTR src);
size_t IM_OSAL_Strlen(const char *str);
ssize_t getline(char **ppLine, size_t *len, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif
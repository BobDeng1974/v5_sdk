/*
 * Copyright (c) 2014, InfoTM Microelectronics Co.,Ltd
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
 * @file      library_register.h
 * @brief
 * @author    ayakashi (eric.xu@infotmic.com.cn)
 * @version   1.1.0
 * @history
 *   2012.04.15 : Create
 */

#ifndef IM_OMX_INFO_DEC_REG
#define IM_OMX_INFO_DEC_REG

#include "IM_OMX_Def.h"
#include "OMX_Component.h"
#include "IM_OMX_Component_Register.h"


#define OSCL_EXPORT_REF __attribute__((visibility("default")))
#define MAX_COMPONENT_NUM         1
#define MAX_COMPONENT_ROLE_NUM    3

/* INFO */
#define IM_OMX_COMPONENT_AUDIO_DEC         "OMX.Infotm.Audio.Decoder"
#define IM_OMX_COMPONENT_MP2_DEC_ROLE    "audio_decoder.mp2"
#define IM_OMX_COMPONENT_AAC_DEC_ROLE    "audio_decoder.aac"
#define IM_OMX_COMPONENT_AC3_DEC_ROLE    "audio_decoder.aac"

#ifdef __cplusplus
extern "C" {
#endif

OSCL_EXPORT_REF int IM_OMX_COMPONENT_Library_Register(IMRegisterComponentType **ppIMComponent);

#ifdef __cplusplus
};
#endif

#endif /* IM_OMX_INFO_DEC_REG */

/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:                 ut_vad_msg.h
* PACKAGE:      
*   Implement of:       utils vad message
*   Date:               2018.08.31
*   Author:             lixx
*   Version:            2.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UT_VAD_MSG_H__
#define  __UT_VAD_MSG_H__

#include <vad_tlvdef.h>

#ifdef __cplusplus
extern "C" {
#endif

void VAD_TLV_HEADER_Init(ST_VAD_TLV_HEADER *hdr, char type, int sid, short seq, short len);

void VAD_TLV_HEADER_Host2Net(ST_VAD_TLV_HEADER* mhdr);
void VAD_TLV_HEADER_Net2Host(ST_VAD_TLV_HEADER* mhdr);

int UT_VAD_MsgToStr(ST_VAD_TLV_HEADER* msg, char* buf, int size);

#ifdef __cplusplus
}
#endif

#endif


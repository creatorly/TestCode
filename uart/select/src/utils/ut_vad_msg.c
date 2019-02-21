/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:                 ut_vad_msg.c
* PACKAGE:      
*   Implement of:       utils vad message
*   Date:               2018.08.31
*   Author:             lixx
*   Version:            2.0
*
* Change
* 
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <eos.h>
#include <utils/ut_vad_msg.h>

#ifdef __cplusplus
extern "C" {
#endif

void VAD_TLV_HEADER_Init(ST_VAD_TLV_HEADER *hdr, char type, int sid, short seq, short len)
{
    hdr->v = VAD_TLV_PROTO_VER;
    hdr->f = 0;
    hdr->magic = VAD_TLV_PROTO_HEADER_MAGIC;
    hdr->type = type;
    hdr->code = 0;
    
    hdr->sid = sid;
    hdr->seq = seq;
    hdr->len = len;
}

void VAD_TLV_HEADER_Host2Net(ST_VAD_TLV_HEADER* mhdr)
{
    mhdr->sid = htonl(mhdr->sid);
    mhdr->seq = htons(mhdr->seq);
    mhdr->len = htons(mhdr->len);
}

void VAD_TLV_HEADER_Net2Host(ST_VAD_TLV_HEADER* mhdr)
{
    mhdr->sid = ntohl(mhdr->sid);
    mhdr->seq = ntohs(mhdr->seq);
    mhdr->len = ntohs(mhdr->len);
}

#ifdef __cplusplus
}
#endif


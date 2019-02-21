/********************************************************
* 
*
* xp-workroom utils platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_quebuf.h
* PACKAGE:      libeut
*   Header of:          utl_quebuf.c
*   Date:               2016.12.03
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_QUEBUF_H__
#define  __UTL_QUEBUF_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*FP_UTL_QBUF_CMP)(const void* q_data, int len, const void* data);
typedef bool (*FP_UTL_QBUF_DO)(const void* q_data, int len);

typedef struct tagstUtlQueBuf utl_qbuf_t;

utl_qbuf_t* utl_qbuf_create(int maxMsgSize);
void utl_qbuf_delete(utl_qbuf_t* qbuf);

int utl_qbuf_push(utl_qbuf_t* qbuf, const void* msg, int len);
int utl_qbuf_pop(utl_qbuf_t* qbuf, void* buf, int size);
int utl_qbuf_pop_end(utl_qbuf_t* qbuf, void* buf, int size);

int utl_qbuf_pop_if(utl_qbuf_t* qbuf, void* buf, int size, FP_UTL_QBUF_CMP fpCond, const void* data);

int utl_qbuf_size(utl_qbuf_t* qbuf);

#if defined(__cplusplus)
}
#endif

#endif


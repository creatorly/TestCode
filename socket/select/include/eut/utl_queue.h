/********************************************************
* 
*
* xp-workroom utils platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_queue.h
* PACKAGE:      libeut
*   Header of:          utl_queue.c
*   Date:               2015.09.07
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_QUEQUE_H__
#define  __UTL_QUEQUE_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UTL_QUE_MODE_FIFO = 0,
    UTL_QUE_MODE_LIFO,
}EN_UTL_QUE_MODE;

typedef struct tagstUtlQueue utl_que_t;

typedef bool (*FP_UTL_QUE_CMP)(const void* ele_in_que, const void* data);
typedef bool (*FP_UTL_QUE_DO)(const void* ele_in_que);

utl_que_t* utl_que_create(EN_UTL_QUE_MODE mode);
void utl_que_destroy(utl_que_t* que);

int utl_que_push(utl_que_t* que, const void* data);
const void* utl_que_pop(utl_que_t* que);
int utl_que_find(utl_que_t* que, const void* data);

int utl_que_size(utl_que_t* que);
const void* utl_que_get(utl_que_t* que, int pos);

void utl_que_concat(utl_que_t* dst, utl_que_t* src);

int utl_que_push_if(utl_que_t* que, const void* data, FP_UTL_QUE_CMP fpCmp);
const void* utl_que_pop_if(utl_que_t* que, const void* data, FP_UTL_QUE_CMP fpCmp);
const void* utl_que_find_if(utl_que_t* que, const void* data, FP_UTL_QUE_CMP fpCmp);

int utl_que_foreach(utl_que_t* que, FP_UTL_QUE_DO fpDo);
int utl_que_foreach_if(utl_que_t* que, const void* data, FP_UTL_QUE_CMP fpCmp, FP_UTL_QUE_DO fpDo);

#if defined(__cplusplus)
}
#endif

#endif


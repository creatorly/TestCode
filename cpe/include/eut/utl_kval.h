/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_kval.h
* PACKAGE:      libeut
*   Header of:          utl_kval.c
*   Date:               16/04/26
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef  __UTL_KVAL_H__
#define  __UTL_KVAL_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlKeyVal
{
    const char* str;    
    long equ_sign;
    
    const char* dlmt;
    size_t dlmt_len;
    
    const char* end;
    size_t end_len;
}utl_kval_t;

void utl_kval_init(utl_kval_t* kv, const char* str, int equ_sign, const char* dlmt, const char* end);
void utl_kval_free(utl_kval_t* kv);

const char* utl_kval_get(utl_kval_t* kv, const char* name, int* pLen);
int utl_kval_get_r(utl_kval_t* kv, const char* name, char* val, int size);

#ifdef __cplusplus
}
#endif

#endif


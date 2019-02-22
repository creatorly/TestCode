/********************************************************
* 
*
* Ejoin SIP Proxy Lib
* Copyright (C) 2013-2013, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_json.h
* PACKAGE:      libeut
*   Header of:          utl_json.c
*   Date:               2013.03.31
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_JSON_H__
#define __UTL_JSON_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlJsonArray
{
    const char* pArr;
    const char* pEle;
}utl_json_arr_t;

const char* utl_json_get_par(const char* json_str, const char* name);

int utl_json_get_int(const char* msg, const char* name);
int utl_json_get_int_dflt(const char* msg, const char* name, int dflt);
int utl_json_get_hint(const char* msg, const char* name);
int utl_json_get_str(const char* msg, const char* name, char* buf, int size);

int utl_json_get_int_if(const char* msg, const char* name, int* piVal);
int utl_json_get_str_if(const char* msg, const char* name, char* buf, int size);

int utl_json_arr_init(utl_json_arr_t* pArray, const char* strArray);
const char* utl_json_arr_get(utl_json_arr_t* pArray, int* pLen);
int utl_json_arr_get_r(utl_json_arr_t* pArray, char* buf, int size);
int utl_json_arr_get_int(utl_json_arr_t* pArray);
bool utl_json_arr_eoa(utl_json_arr_t* pArray);

#define utl_json_get_buf(js, name, buf) \
            utl_json_get_str(js, name, buf, sizeof(buf))

#define utl_json_get_buf_if(js, name, buf) \
            utl_json_get_str_if(js, name, buf, sizeof(buf))
            
#ifdef __cplusplus
}
#endif

#endif


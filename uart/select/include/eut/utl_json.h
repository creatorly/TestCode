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
#include <utl_typdef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_JSON_MIN_SIZE      512

typedef struct tagstUtlJson utl_json_t;

typedef struct tagstUtlJsonArray
{
    const char* pArr;
    const char* pEle;
}utl_json_arr_t;

typedef struct tagstUtlJsonIterator
{
    const char* i_pos;
}utl_json_it_t;

typedef struct tagstUtlJsonAttribute
{
    char a_name[64];
    char a_value[128];      /* the copied and escaped value */

    const char* a_vbegin;   /* original attr value include '\'', '\"', '{' and '[' */
    const char* a_vend;     /* original attr value end, may be '\'', '\"', '}',  ']' and ',' */
}utl_json_attr_t;

const char* utl_json_get_par(const char* msg, const char* name);
const char* utl_json_get_par2(const char* msg, const char* name, int* plen);

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

/* Initialized a json iterator */
void utl_json_it_init(utl_json_it_t* jit, const char* jstr);

/* Get a attribute for a json iterator
 * Return 0 if success, -1 otherwise
 */
int utl_json_it_get(utl_json_it_t* jit, utl_json_attr_t* jattr);

#define utl_json_get_buf(js, name, buf) \
            utl_json_get_str(js, name, buf, sizeof(buf))

#define utl_json_get_buf_if(js, name, buf) \
            utl_json_get_str_if(js, name, buf, sizeof(buf))

/****************************************************************************************
 * Define the APIs for the manipulation of json data
 */
int utl_json_init(utl_json_t** pJson, int init_size, bool is_array);
void utl_json_free(utl_json_t* json);

/****************************************************************************************
 * ADD a attribute to a given json object by name
 *
 * NOTE: argument name is invalid for the json object with array type
 */
int utl_json_add_int(utl_json_t* json, const char* name, int value);
int utl_json_add_str(utl_json_t* json, const char* name, const char* value);
int utl_json_add_nstr(utl_json_t* json, const char* name, const char* value, int len);
int utl_json_add_txt(utl_json_t* json, const char* name, const char* value);
int utl_json_add_json(utl_json_t* json, const char* name, utl_json_t* jvalue);

int UTL_CHK_FMTARGS(3) 
    utl_json_add_fmt(utl_json_t* json, const char* name, const char* fmt, ...);

/**
 * GET the string format of the json data
 */
const char* utl_json_to_str(utl_json_t* json);
int utl_json_strlen(utl_json_t* json);

/**
 * POP the string format of the json data with it's length
 *
 * NOTE: you SHOULD free the returned string using utl_free()
 */
char* utl_json_pop_str(utl_json_t* json, int* plen);

#ifdef __cplusplus
}
#endif

#endif


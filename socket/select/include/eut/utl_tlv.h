/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_tlv.h
* PACKAGE:      libeut
*   Header of:          utl_tlv.c
*   Date:               09/07/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_TLV_H__
#define  __UTL_TLV_H__

#include <utl_typdef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_TLV_INVAL_INT   UTL_INT_MIN

typedef struct tagstUtlTypeLenVal
{
    int len;
    int size;
    unsigned char* data;
}utl_tlv_t;

typedef struct tagstUtlTlvIterator
{
    int tag;
    int len;
    void* val;
    
    unsigned char* ptr;
    unsigned char* end;
}utl_tlv_it_t;

void utl_tlv_init(utl_tlv_t* tlv, void* data, int len, int size);
void* utl_tlv_get(utl_tlv_t* tlv, int tag, int* pLen);
int utl_tlv_set(utl_tlv_t* tlv, int tag, int len, const void* val);

void utl_tlv_add_int(utl_tlv_t* tlv, int tag, int val);
void utl_tlv_add_str(utl_tlv_t* tlv, int tag, const char* val);
void utl_tlv_add_ip(utl_tlv_t* tlv, int tag, const char ip[32]);
int utl_tlv_add_mac(utl_tlv_t* tlv, int tag, const char mac[18]);
int utl_tlv_add_hex(utl_tlv_t* tlv, int tag, const char* val);

/**
 * GET a integer value by specified tlv type
 * @param tlv:      The tlv to be worked on
 * @param tag:      The type of tlv param
 *
 * @return:         the integer value if successful, otherwise UTL_INT_MIN
 */
int utl_tlv_get_int(utl_tlv_t* tlv, int tag);

const char* utl_tlv_get_str(utl_tlv_t* tlv, int tag);
int utl_tlv_get_str_r(utl_tlv_t* tlv, int tag, char* buf, int size);
int utl_tlv_get_ip(utl_tlv_t* tlv, int tag, char ip[32]);
int utl_tlv_get_mac(utl_tlv_t* tlv, int tag, char mac[18]);

/* try to get a value if the tag exists, otherwise do nothing */
int utl_tlv_get_int_if(utl_tlv_t* tlv, int tag, int* piVal);
int utl_tlv_get_str_if(utl_tlv_t* tlv, int tag, char* buf, int size);

#define utl_tlv_get_buf(t, i, b)    utl_tlv_get_str_r(t, i, b, sizeof(b))
#define utl_tlv_get_buf_if(t, i, b) utl_tlv_get_str_if(t, i, b, sizeof(b))

/* return the first value and store the tag and length in iterator */
void* utl_tlv_get_first(const void* data, int len, utl_tlv_it_t* it);
/* return next value and store the tag and length in iterator */
void* utl_tlv_get_next(utl_tlv_it_t* it);

#ifdef __cplusplus
}
#endif

#endif


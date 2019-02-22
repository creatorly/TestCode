/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2011-2011, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_base64.h
* PACKAGE:      libeut
*   Header of:          utl_base64.c
*   Date:               12/10/11
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_BASE64_H__
#define  __UTL_BASE64_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlBase64Contex
{
    const char* charset;
    int bNoCrLf;
    int bNoEnd;
}utl_b64ctx_t;

int utl_base64_enc(const unsigned char* utf8, unsigned int len, char* base64, int size);
int utl_base64_dec(const char* base64, unsigned int len, unsigned char* utf8, int size);

/* the argument "chs" should be a string with the length as 64 */
int utl_base64_encode(const char* chs, const void* utf8, unsigned int len, char* base64, int size);
int utl_base64_decode(const char* chs, const char* base64, unsigned int len, unsigned char* utf8, int size);

int utl_base64_ctx_enc(utl_b64ctx_t* b64ctx, const void* utf8, unsigned int len, char* base64, int size);

#ifdef __cplusplus
}
#endif

#endif


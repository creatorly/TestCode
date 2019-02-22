/********************************************************
* 
*
* Ejoin Utils library
* Copyright (C) 2011-2011, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         utl_crypt.h
* PACKAGE:      
*   Header of:          utl_crypt.c
*   Date:               2014.10.23
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef  __UTL_CRYPT_H__
#define  __UTL_CRYPT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UTL_CPT_METHOD_NONE = 0,    /* don't encrypt/decrypt */
    UTL_CPT_METHOD_RC4,
    UTL_CPT_METHOD_XOR,         /* xor with key */
    UTL_CPT_METHOD_B64,
}EN_UTL_CPT_METHOD;

typedef struct tagstUtlCrypt utl_crypt_t;

/* return the length of encrypted data */
typedef int (*FP_UTL_CRYPT)(const char* indat, int len, 
                            const char* key, int keyLen,
                            char* outdat, int size);

typedef FP_UTL_CRYPT FP_UTL_ENCRYPT;
typedef FP_UTL_CRYPT FP_UTL_DECRYPT;

utl_crypt_t* utl_crypt_create(int id, const char* pubKey, const char* priKey);
void utl_crypt_destroy(utl_crypt_t* pCrypt);

int utl_crypt_get_id(utl_crypt_t* pCrypt);

FP_UTL_ENCRYPT utl_crypt_get_enc(utl_crypt_t* pCrypt);
FP_UTL_DECRYPT utl_crypt_get_dec(utl_crypt_t* pCrypt);
const char* utl_crypt_get_pubkey(utl_crypt_t* pCrypt);

int utl_crypt_get_key(utl_crypt_t* pCrypt, 
                      const char* pubKey, 
                      const char* priKey,
                      char* key, int size);

int utl_crypt_enc(utl_crypt_t* pCrypt, 
                  const char* indat, int len, 
                  char* outdat, int size);

int utl_crypt_dec(utl_crypt_t* pCrypt, 
                  const char* indat, int len, 
                  char* outdat, int size);

#ifdef __cplusplus
}
#endif

#endif


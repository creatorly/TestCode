/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2011-2011, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_rc4.h
* PACKAGE:      libeut
*   Header of:          utl_rc4.c
*   Date:               12/10/11
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_RC4_H__
#define  __UTL_RC4_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * RC4 Crypt
 * @param inDat:    The input data should be crypted
 * @param len:      The length of input data
 * @param key:      The string key used to crypt
 * @param keyLen:   The length of key
 * @param outDat:   The output data crypted
 * @param size:     The size of output buffer
 *
 * @return:         The length of output data crypted
 */
int utl_rc4_code(const char* inDat, int len, 
                 const char* key, int keyLen,
                 char* outDat, int size);

#ifdef __cplusplus
}
#endif

#endif


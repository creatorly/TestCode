/********************************************************
* 
*
* Ejoin Utils Library
* Copyright (C) 2017-2017, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         utl_xor.h
* PACKAGE:      
*   Header of:          utl_cpt_xor.c
*   Date:               2017.11.14
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef  __UTL_XOR_H__
#define  __UTL_XOR_H__

#ifdef __cplusplus
extern "C" {
#endif

int utl_xor_code(const char* indat, int len, 
                 const char* key, int keyLen,
                 char* outdat, int size);
    
#ifdef __cplusplus
}
#endif

#endif


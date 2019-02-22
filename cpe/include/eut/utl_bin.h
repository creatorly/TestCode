/********************************************************
* 
*
* xp-workroom IAD platform
* Copyright (C) 2012-2012, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_bin.h
* PACKAGE:      
*   Header of:          utl_bin.c
*   Date:               2012.07.08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_BIN_H__
#define  __UTL_BIN_H__

#include <stdbool.h>
#include <ctype.h>

#ifndef __VXWORKS_OS__
#include <wchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * for the verfication of the firmware of the device
 */
 
typedef enum
{
    UTL_BIN_TYPE_VOIP = 0,
    UTL_BIN_TYPE_GOIP,
}EN_UTL_BIN_TYPE;

typedef enum
{
    UTL_BIN_ERRNO_OK = 0,
    UTL_BIN_ERRNO_FILE_BROKEN,
    UTL_BIN_ERRNO_INVALID_HW,
    UTL_BIN_ERRNO_INVALID_BD,
    UTL_BIN_ERRNO_INVALID_VER,
    UTL_BIN_ERRNO_INVALID_OEM,
    UTL_BIN_ERRNO_INVALID_FTYPE,
}EN_UTL_BIN_ERRNO;

typedef struct tagstUtlBin utl_bin_t;

/* version check function, return the reason when failed */
typedef const char* (*utl_bin_func_verchk)(const char* curVer, const char* newVer);

utl_bin_t* utl_bin_create(EN_UTL_BIN_TYPE type);
void utl_bin_destroy(utl_bin_t* pBin);

void utl_bin_set_ver(utl_bin_t* pBin, const char* version);
void utl_bin_set_func_verchk(utl_bin_t* pBin, utl_bin_func_verchk fpVerChk);
void utl_bin_set_type(utl_bin_t* pBin, const char* type);

const char* utl_bin_verify_data(utl_bin_t* pBin, const char* data, int len);

int utl_bin_get_int(utl_bin_t* pBin, const char* name);
const char* utl_bin_get_str(utl_bin_t* pBin, const char* name);

int utl_bin_get_str_r(utl_bin_t* pBin, const char* name, char* buf, int size);
#define utl_bin_get_buf(pBin, name, buf) \
    utl_bin_get_str_r(pBin, name, buf, sizeof(buf))

/* get the type(File-Type) of data */
const char* utl_bin_get_type(utl_bin_t* pBin);

/* get the original length(Length) of data */
int utl_bin_get_len(utl_bin_t* pBin);

/*************************************************************************************************
 * the general binary data operations
 */

unsigned int utl_bin_hash(void* ptr, size_t len);

/* to Hex(abcdef) */
const char* utl_bin2hex(const void* bin, int len, char* hex);
/* to Hex(ABCDEF) */
const char* utl_bin2Hex(const void* bin, int len, char* hex);

int utl_hex2bin_l(const char* hex, int hlen, unsigned char* bin, int size);
int utl_wch2bin_l(const wchar_t* hex, int hlen, unsigned char* bin, int size);
int utl_h1toi(char hex);

#define utl_hex2bin(h, b, s) utl_hex2bin_l(h, eos_strlen(h), b, s)

#ifdef __cplusplus
}
#endif

#endif


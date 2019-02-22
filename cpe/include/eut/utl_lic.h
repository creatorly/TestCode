/********************************************************
* 
*
* xp-workroom IAD platform
* Copyright (C) 2012-2012, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_lic.h
* PACKAGE:      
*   Header of:          utl_lic.c
*   Date:               2012.06.25
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_LIC_H__
#define  __UTL_LIC_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_LIC_KEY_MAC         "MAC"
#define UTL_LIC_KEY_EXPIRED     "Expired"
#define UTL_LIC_KEY_MAXLINES    "Max-Lines"
#define UTL_LIC_KEY_NTPSVR      "NTP-Server"

#define UTL_LIC_KEY_ID          "License-ID"
#define UTL_LIC_KEY_REMTIME     "Remain-Time"

#define UTL_LIC_KEY_SUSR        "S-Username"
#define UTL_LIC_KEY_SPWD        "S-Password"
#define UTL_LIC_KEY_HIDUSR      "Hidden-Usr"
#define UTL_LIC_KEY_HIDPWD      "Hidden-Pwd"

#define UTL_LIC_KEY_DEV_ENABLED "Dev-Enabled"
#define UTL_LIC_KEY_MAX_OFFLINE_LIFE    "Max-Offline-Life"
#define UTL_LIC_KEY_OFFLINE_LIFE        "Offline-Life"

#define UTL_LIC_ERRNO_SUCCESS   0
#define UTL_LIC_ERRNO_MALLOC    -1
#define UTL_LIC_ERRNO_FOPEN     -2
#define UTL_LIC_ERRNO_B64_DEC   -3
#define UTL_LIC_ERRNO_MD5_HASH  -4

#define UTL_LIC_ERRNO_INVALID_MAC   -5
#define UTL_LIC_ERRNO_INVALID_EXP   -6

typedef struct tagstUtlLicence utl_lic_t;

typedef char utl_lic_csmd5_t[16];
typedef char utl_lic_csb64_t[64];

typedef int (*fp_utl_lic_savefile)(const char* data, int len, const char* filename);

utl_lic_t* utl_lic_create(const char* pwd, const char* mac);
void utl_lic_destroy(utl_lic_t* plic);

void utl_lic_set_mac(utl_lic_t* plic, const char* mac);
void utl_lic_set_private_key(utl_lic_t* plic, const char* pkey);

/* the default MD5 charset is "0123456789ABCDEF" */
void utl_lic_set_md5_charset(utl_lic_t* plic, const utl_lic_csmd5_t cs_md5);

/* the default Base64 charset is 
 * "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" 
 */
void utl_lic_set_b64_charset(utl_lic_t* plic, const utl_lic_csb64_t cs_b64);

/* the default action is fopen, fwrite and fclose */
void utl_lic_set_hook_savefile(utl_lic_t* plic, fp_utl_lic_savefile fpSaveFile);

int utl_lic_get_int(utl_lic_t* plic, const char* name);
const char* utl_lic_get_str(utl_lic_t* plic, const char* name);

int utl_lic_get_str_r(utl_lic_t* plic, const char* name, char* buf, int size);
#define utl_lic_get_buf_r(plic, name, buf) \
    utl_lic_get_str_r(plic, name, buf, sizeof(buf))

int utl_lic_set_str(utl_lic_t* plic, const char* name, const char* value);
int utl_lic_set_int(utl_lic_t* plic, const char* name, int value);

int utl_lic_parse_fd(utl_lic_t* plic, int fd);
int utl_lic_parse_file(utl_lic_t* plic, const char* lic_file);
int utl_lic_parse_data(utl_lic_t* plic, const char* lic_data, int len);

int utl_lic_verify(utl_lic_t* plic);

int utl_lic_save_file(utl_lic_t* plic, const char* lic_file);
int utl_lic_save_data(utl_lic_t* plic, char* buf, int size);

void utl_lic_random_string(const char charset[128], unsigned rand_num, char* buf, int len);
bool utl_lic_date_expired(const char* datetime, const char* ntpsvr, int timeout, int retrys);

#ifdef __cplusplus
}
#endif

#endif


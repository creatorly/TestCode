/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2011-2011, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_md5.h
* PACKAGE:      libeut
*   Header of:          utl_md5.c
*   Date:               12/10/11
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_MD5_H__
#define  __UTL_MD5_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ubyte
#define ubyte unsigned char
#endif

#ifndef ubyte4
#define ubyte4 unsigned int
#endif

#ifndef sbyte4
#define sbyte4 int
#endif

/* Data structure for MD5 (Message Digest) computation */
typedef struct 
{
  ubyte4    i[2];                           /* number of _bits_ handled mod 2^64 */
  ubyte4    buf[4];                         /* scratch buffer */
  ubyte     in[64];                         /* input buffer */
  
  ubyte     digest[16];     /* actual digest after utl_md5_final call */

} utl_md5_ctx_t;

/* forward references */
void utl_md5_init  (utl_md5_ctx_t* md5ctx);
void utl_md5_update(utl_md5_ctx_t* md5ctx, const void* inBuf, ubyte4 inLen);
void utl_md5_final (utl_md5_ctx_t* md5ctx);

void utl_md5_get_digest(utl_md5_ctx_t* md5ctx, const char* charset, char digest[33]);
void utl_md5_calc_digest(const void* data, int len, const char* charset, char digest[33]);

#ifdef __cplusplus
}
#endif

#endif


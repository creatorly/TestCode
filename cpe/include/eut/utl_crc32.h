/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2014-2014, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_crc32.h
* PACKAGE:      libeut
*   Header of:          utl_crc32.c
*   Date:               2014.08.05
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef __UTL_CRC32_H__
#define __UTL_CRC32_H__

#ifdef __cplusplus
extern "C" {
#endif
    
int utl_crc32_init();
unsigned long utl_crc32(unsigned long crc, const unsigned char *buf, int len);   
    
#ifdef __cplusplus
}
#endif

#endif /* !__CRC32_HEADER__ */


/********************************************************
* 
*
* Ejoin utils
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_udp.h
* PACKAGE:      libeut
*   Header of:          utl_udp.c
*   Date:               2016.05.04
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_UDP_H__
#define __UTL_UDP_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned short utl_udp_chksum(void* data, int len);

#ifdef __cplusplus
}
#endif

#endif


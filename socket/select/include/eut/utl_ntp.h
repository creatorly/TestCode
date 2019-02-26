/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2012-2012, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_ntp.h
* PACKAGE:      libeut
*   Implement of:       utl_ntp.c
*   Date:               2012.03.25
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_NTP_H__
#define __UTL_NTP_H__

#include <time.h>
#ifdef __VXWORKS_OS__
#include <times.h>
#else
#include <sys/time.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

int utl_ntpdate(const char* ntp_server, int timeout, int retrys, struct timeval* cur_time);

#ifdef __cplusplus
}
#endif

#endif  


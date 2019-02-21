/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2012-2012, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_dns.h
* PACKAGE:      libeut
*   Implement of:       utl_dns.c
*   Date:               2012.03.25
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_DNS_H__
#define __UTL_DNS_H__

#if defined(__cplusplus)
extern "C" {
#endif

int utl_dns_init(int resrv_intvl);

const char* utl_dns_get(const char* hostname);
const char* utl_dns_get_r(const char* hostname, char* buf, int size);

/* get the host, wait the result, return the ipv4 address in network order */
unsigned int utl_gethostbyname(const char* name, char* buf, int size);

/* get all the hosts like utl_gethostbyname, but return the count of got host  */
int utl_gethostsbyname(const char* name, char buf[][32], int size);

#ifdef __cplusplus
}
#endif

#endif  


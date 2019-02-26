/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         ut_popen.h
* PACKAGE:      
*   Date:               2018.07.19
*   Author:             PengJian
*   Version:            1.0
*
* Change     
* 
*********************************************************/

#ifndef  __UT_POPEN_H__
#define  __UT_POPEN_H__

#ifdef __cplusplus
extern "C" {
#endif

int UT_Popen(const char* cmd, char* outbuf, int osize);
int UT_PopenNolog(const char* cmd, char* outbuf, int osize);
int UT_PopenInDir(const char* dir, const char* cmd, char* outbuf, int osize, char* errbuf, int esize);

#ifdef __cplusplus
}
#endif

#endif


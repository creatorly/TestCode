/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vfd_log.h
* PACKAGE:      
*   Header of:          vfd_log.c
*   Date:               2017.06.01
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VFD_LOG_H__
#define  __VFD_LOG_H__

#include <utl_log.h>

#ifdef __cplusplus
extern "C" {
#endif

#define vfd_log VFD_LogGet()

int VFD_LogInit(int level, const char* file, int size, int count);
void VFD_LogFree();

void VFD_LogSetUtlLevel(int level);
void VFD_LogSetStdout(bool enabled);

utl_log_t* VFD_LogGet();

#ifdef __cplusplus
}
#endif

#endif


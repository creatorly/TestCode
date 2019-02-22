/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2017-2017, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vad.h
* PACKAGE:      
*   Date:               2017.06.30
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef  __VAD_H__
#define  __VAD_H__

#ifdef __cplusplus
extern "C" {
#endif

int vad_init(const char* home_dir);
int vad_free();

int vad_start();
int vad_stop();

int vad_proc_simrst(int value);

int vad_logout(const char* file, int line, int level, const char* info);

#ifdef __cplusplus
}
#endif

#endif


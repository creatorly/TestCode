/********************************************************
* 
*
* Ejoin SIP Media Relay
* Copyright (C) 2008-2008, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_portmgr.h
* PACKAGE:      libeut
*   Header of:          utl_portmgr.c
*   Date:               2012.03.16
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_PORTMGR_H__
#define __UTL_PORTMGR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlPortManager utl_pm_t;

utl_pm_t* utl_pm_create(int iStart, int iSize);
void utl_pm_destroy(utl_pm_t* hPm);

int utl_pm_reset(utl_pm_t* pstMgr, int iStart, int iSize);

int utl_pm_get_start(utl_pm_t* hPm);
int utl_pm_get_size(utl_pm_t* hPm);

int utl_port_alloc(utl_pm_t* hPm);
void utl_port_free(utl_pm_t* hPm, int port);

bool utl_port_isfree(utl_pm_t* hPm, int port);

#ifdef __cplusplus
}
#endif

#endif



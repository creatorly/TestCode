/********************************************************
* 
*
* Ejoin Utils Library
* Copyright (C) 2014-2014, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         utl_bridge.h
* PACKAGE:      
*   Header of:          utl_bridge.c
*   Date:               2014.10.23
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_BRIDGE_H__
#define  __UTL_BRIDGE_H__

#include <utl_timer.h>
#include <utl_conn.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlBridge utl_brg_t;

typedef int (*FP_UTL_BRG_OPENFAIL)(utl_brg_t* pBrg, int cause, const void* data);
typedef int (*FP_UTL_BRG_OPENED)(utl_brg_t* pBrg, const void* data);
typedef int (*FP_UTL_BRG_CLOSED)(utl_brg_t* pBrg, const void* data);

/* the session consists of two connections */
utl_brg_t* utl_brg_create(utl_conn_t* con1, utl_conn_t* con2);
void utl_brg_destroy(utl_brg_t* pBrg);

void utl_brg_set_timers(utl_brg_t* pBrg, utl_timers_t* pTmrGrp);
void utl_brg_set_nodatdur(utl_brg_t* pBrg, int iNoDatDur);

/* set encrypt/decrypt for the connection of the session */
void utl_brg_conn_set_crypt(utl_conn_t* pCon, 
                            utl_crypt_t* pCrypt, 
                            const char* pubKey);

utl_conn_t* utl_brg_get_conn1(utl_brg_t* pBrg);
utl_conn_t* utl_brg_get_conn2(utl_brg_t* pBrg);

int utl_brg_open(utl_brg_t* pBrg,
                 const void* data,
                 FP_UTL_BRG_OPENED fpOpened,
                 FP_UTL_BRG_OPENFAIL fpOpenFail,
                 FP_UTL_BRG_CLOSED fpClosed);

#ifdef __cplusplus
}
#endif

#endif


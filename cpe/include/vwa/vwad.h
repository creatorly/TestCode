/********************************************************
* 
*
* ViFi Web Authorization Daemon
* Copyright (C) 2017-2017, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vwad.h
* PACKAGE:      
*   Header of:          vwad.c
*   Date:               2017.03.29
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VWAD_H__
#define  __VWAD_H__

#include <stdbool.h>
#include <vwa_config.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstVwaDaemon* PST_VWAD;

/**
 * CREATE a ViFi Web Authorization Daemon handle
 * @param pCfg:     the configuration handle initialized before
 *
 * @return:     the vwad handle if successful, NULL otherwise
 */
PST_VWAD VWAD_Create(PST_VWA_CFG pCfg);

/**
 * DESTROY a vwad handle
 * @param vwad: the vwad handle to be released
 */
void VWAD_Destroy(PST_VWAD vwad);

/**
 * Start a vwad handle
 * @param create_task:  create a main task or not
 *
 * @return:     0 if create a task successfully
 *              DO NOT return if no need to create a task
 */
int VWAD_Start(PST_VWAD vwad, bool create_task);

/**
 * Stop a vwad handle
 */
void VWAD_Stop(PST_VWAD vwad);

/**
 * RELOAD the configuratin of a vwad handle
 * @param vwad:     the vwad handle to be reloaded
 */
void VWAD_Reload(PST_VWAD vwad);

/**
 * TELL if the vwad handle is started
 * @param vwad:     the vwad handle to be worked
 */
bool VWAD_IsStarted(PST_VWAD vwad);

#ifdef __cplusplus
}
#endif

#endif


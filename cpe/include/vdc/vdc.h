/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vdc.h
* PACKAGE:      
*   Header of:          vdc.c
*   Date:               2015.10.15
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VDC_H__
#define  __VDC_H__

#include <stdbool.h>
#include <utl_log.h>
#include <utl_cfg.h>

#include "vdc_ver.h"
#include "vdc_config.h"

#include "utl_httpc.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstVifiDeviceCtrl* PST_VDC;

/**
 * GET version of VDC
 *
 * @return:     the version string
 */
const char* VDC_Version();

/**
 * CREATE a Vifi Device Ctrl handle
 * @param pLog: the log instanse to be used, if NULL created by VDC_Create() itself
 * @param pCfg: the configuration handle initialized before
 *
 * @return:     the server handle if successful, NULL otherwise
 */
PST_VDC VDC_Create(utl_log_t* pLog, utl_cfg_h pAppCfg, PST_VDC_CFG pCfg);

/**
 * DESTROY a Vifi Device Ctrl
 * @param pCli: the server handle to be released
 */
void VDC_Delete(PST_VDC pSvr);


/**
 * START a Vifi Device Ctrl
 * @param pCli: the server handle to be start
 * @param bThreadCreate: if create a new thread
 *
 * @return:     bThreadCreate = 1: 0 if successfuly, non zero otherwise 
 *              bThreadCreate = 0: DO NOT return
 */
int VDC_Start(PST_VDC pSvr, bool bThreadCreate);

/**
 * STOP a Vifi Device Ctrl
 * @param pCli: the server handle to be stopped
 *
 * @return:     0 if successful, non zero otherwise
 */
int VDC_Stop(PST_VDC pSvr);

/**
 * RELOAD the configuratin of a Vifi Device Ctrl
 * @param pSvr: the server handle to be reloaded
 *
 * @return:     0 if successful, non zero otherwise
 */
void VDC_Reload(PST_VDC pSvr);

/**
 * TELL if a Vifi Device Ctrl is started
 * @param pCli: the server handle to be tell
 *
 * @return:     true if started, false otherwise
 */
bool VDC_IsStarted(PST_VDC pSvr);

utl_httpc_t* VDC_GetVDC_IPLHttpHlr(void);

#ifdef __cplusplus
}
#endif

#endif


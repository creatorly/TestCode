/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vdc_config.h
* PACKAGE:      
*   Header of:          vdc_config.c
*   Date:               2015.10.10
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VDC_CONFIG_H__
#define  __VDC_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    VDC_CFG_NONE = 0,

    VDC_CFG_UTL_LOGLV,           /* string£¬ the utl log level in string */
    
    VDC_CFG_LOG_LEVEL,          /* string£¬ the log level in string */
    VDC_CFG_LOG_STDOUT,         /* integer£¬trace the log to stdout or not */
    VDC_CFG_LOG_FILE,           /* string£¬ the log full path and name */
    VDC_CFG_LOG_SIZE,           /* string£¬ the size in MB of each log file */
    VDC_CFG_LOG_COUNT,          /* integer, the total count of log file */
    
    VDC_CFG_VRS_ADDR,           /* record, VRS(ViFi Root Server) addr include fields "ip, port" */
    
    VDC_CFG_APP_CFGFILE,        /* string, the app config filename */
    VDC_CFG_MMAP_FILE,          /* string, the mem map filename */
    VDC_CFG_MMAP_FFILE,         /* string, the flash map filename */

    VDC_CFG_MAX
}EN_VDC_CFG;

typedef void* PST_VDC_CFG;

/**
 * Initialize a Cosmos SecureTalk configuration handle
 * @param cfgfile:  the full path name of the config file
 *
 * @return:         the config handle if successful, NULL otherwise
 *
 * @note:   if argument cfgfile is NULL, use default config file
 *          or use external configuration value by invoking 
 *          VDC_CfgSetInt(), VDC_CfgSetStr() and VDC_CfgAddRec()
 */
PST_VDC_CFG VDC_CfgInit(const char* cfgfile);

/**
 * RELEASE a Cosmos SecureTalk configuration
 * @param cfg:  the configuration to be release
 */
int VDC_CfgFree(PST_VDC_CFG cfg);

/**
 * GET a integer configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VDC_CFG
 *
 * @return:     the integer value got
 */
int VDC_CfgGetInt(PST_VDC_CFG cfg, EN_VDC_CFG id);
/**
 * GET a string configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VDC_CFG
 * @param buf:  the buffer to store the string value
 * @param size: the buffer size
 *
 * @return:     zero if successful, otherwise non-zero
 */
int VDC_CfgGetStr(PST_VDC_CFG cfg, EN_VDC_CFG id, char* buf, int size);

/**
 * SET a integer configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VDC_CFG
 * @param iVal: the specified integer value
 */
void VDC_CfgSetInt(PST_VDC_CFG cfg, EN_VDC_CFG id, int iVal);

/**
 * SET a string configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VDC_CFG
 * @param sVal: the specified string value
 */
void VDC_CfgSetStr(PST_VDC_CFG cfg, EN_VDC_CFG id, const char* sVal);

/**
 * ADD a record configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VDC_CFG
 * @param sRec: the specified recrod value
 */
void VDC_CfgAddRec(PST_VDC_CFG cfg, EN_VDC_CFG id, const char* sRec);

#ifdef __cplusplus
}
#endif

#endif


/********************************************************
* 
*
* Ejoin SIM Cloud platform
* Copyright (C) 2014-2014, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vwa_config.h
* PACKAGE:      
*   Header of:          VWA_config.c
*   Date:               2014.11.25
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VWA_CONFIG_H__
#define  __VWA_CONFIG_H__

#include <utl_cfg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VWA_SESS_NODAT_TIMEOUT  60

typedef enum
{
    VWA_CFG_NONE = 0,

    VWA_CFG_LOG_LEVEL,          /* string£¬ the log level in string */
    VWA_CFG_LOG_LVUTL,          /* string£¬ the UTL log level in string */
    VWA_CFG_LOG_STDOUT,         /* bool, print the log to stdout or not */
    
    VWA_CFG_LOG_FILE,           /* string£¬ the log full path and name*/
    VWA_CFG_LOG_SIZE,           /* string£¬ the size in MB of each log file */
    VWA_CFG_LOG_COUNT,          /* integer, the total count of log file */

    VWA_CFG_MMAP_FILE,          /* string, the mem map filename */
    VWA_CFG_MMAP_FFILE,         /* string, the flash map filename */
    
    VWA_CFG_BIND_IP,            /* string£¬ erm client ip */
    VWA_CFG_BIND_PORT,          /* integer, erm client port */
    VWA_CFG_SVR_ADDR,           /* record, server addr include fields "ip, port and expires" */

    VWA_CFG_HOME_DIR,           /* string£¬ html file home directory */
    VWA_CFG_HTML_URL,           /* string£¬ html file url in server */
    VWA_CFG_HTML_EXPIRES,       /* string,  html file expires */
    VWA_CFG_HTML_FILE,          /* record, html file desc "code, name_in_homedir" */

    VWA_CFG_MAX
}EN_VWA_CFG;

typedef void* PST_VWA_CFG;

/**
 * Initialize a Cosmos SecureTalk configuration handle
 * @param cfgfile:  the full path name of the config file
 *
 * @return:         the config handle if successful, NULL otherwise
 *
 * @note:   if argument cfgfile is NULL, use default config file
 *          or use external configuration value by invoking 
 *          VWA_CfgSetInt(), VWA_CfgSetStr() and VWA_CfgAddRec()
 */
PST_VWA_CFG VWA_CfgInit(const char* cfgfile);

/**
 * RELEASE a Cosmos SecureTalk configuration
 * @param cfg:  the configuration to be release
 */
int VWA_CfgFree(PST_VWA_CFG cfg);

/**
 * GET a integer configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VWA_CFG
 *
 * @return:     the integer value got
 */
int VWA_CfgGetInt(PST_VWA_CFG cfg, EN_VWA_CFG id);
/**
 * GET a string configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VWA_CFG
 * @param buf:  the buffer to store the string value
 * @param size: the buffer size
 *
 * @return:     zero if successful, otherwise non-zero
 */
int VWA_CfgGetStr(PST_VWA_CFG cfg, EN_VWA_CFG id, char* buf, int size);

/**
 * SET a integer configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VWA_CFG
 * @param iVal: the specified integer value
 */
void VWA_CfgSetInt(PST_VWA_CFG cfg, EN_VWA_CFG id, int iVal);

/**
 * SET a string configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VWA_CFG
 * @param sVal: the specified string value
 */
void VWA_CfgSetStr(PST_VWA_CFG cfg, EN_VWA_CFG id, const char* sVal);

/**
 * GET the count of a specified configuration
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_VWA_CFG
 *
 * @return:     the count of configuration
 */
int VWA_CfgSize(PST_VWA_CFG cfg, EN_VWA_CFG id);

#ifdef __cplusplus
}
#endif

#endif


#ifndef __INVOKE_CONFIG_H__
#define __INVOKE_CONFIG_H__

#include <utl_cfg.h>
#include <utl_typdef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    	CPE_CFG_ACCESS_SWITCH,	/* integer, 0: don't use acl rule, 1:use acl rule */
	CPE_CFG_ACCESS_MODE,        /* integer, 0: Normal(using black list), 1: Secure(using white list) */
    	CPE_CFG_BLACK_LIST,         /* record, access black list "hwaddr hostname(optional)" */
    	CPE_CFG_WHITE_LIST,         /* record, access white list "hwaddr hostname(optional)" */
    	CPE_CFG_PORT_FORWARD_LIST,  /* record, Port Forwarding Rules list "Rule—identification Source-Port Protocols Destination-IP Destination-Port " */

    	CPE_CFG_MAX
}EN_CPE_CFG;

typedef void* PST_CPE_CFG;

/**
 * Initialize a Cosmos SecureTalk configuration handle
 * @param cfgfile:  the full path name of the config file
 *
 * @return:         the config handle if successful, NULL otherwise
 *
 * @note:   if argument cfgfile is NULL, use default config file
 *          or use external configuration value by invoking 
 *          CPE_CfgSetInt(), CPE_CfgSetStr() and CPE_CfgAddRec()
 */
PST_CPE_CFG CPE_CfgInit(const char* cfgfile);

/**
 * RELEASE a Cosmos SecureTalk configuration
 * @param cfg:  the configuration to be release
 */
int CPE_CfgFree(PST_CPE_CFG cfg);

int CPE_CfgSave(PST_CPE_CFG cfg);
int CPE_CfgReload(PST_CPE_CFG cfg);

/**
 * GET a integer configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_CPE_CFG
 *
 * @return:     the integer value got
 */
int CPE_CfgGetInt(PST_CPE_CFG cfg, EN_CPE_CFG id);
/**
 * GET a string configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_CPE_CFG
 * @param buf:  the buffer to store the string value
 * @param size: the buffer size
 *
 * @return:     zero if successful, otherwise non-zero
 */
int CPE_CfgGetStr(PST_CPE_CFG cfg, EN_CPE_CFG id, char* buf, int size);

#define CPE_CfgGetBuf(cfg, id, b)   CPE_CfgGetStr(cfg, id, b, sizeof(b));

/**
 * SET a integer configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_CPE_CFG
 * @param iVal: the specified integer value
 */
void CPE_CfgSetInt(PST_CPE_CFG cfg, EN_CPE_CFG id, int iVal);

/**
 * SET a string configuration value
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_CPE_CFG
 * @param sVal: the specified string value
 */
void CPE_CfgSetStr(PST_CPE_CFG cfg, EN_CPE_CFG id, const char* sVal);

/**
 * GET the count of a specified configuration
 * @param cfg:  the configuration handle
 * @param id:   the param id, defined by enum EN_CPE_CFG
 *
 * @return:     the count of configuration
 */
int CPE_CfgSize(PST_CPE_CFG cfg, EN_CPE_CFG id);

#ifdef __cplusplus
}
#endif

#endif



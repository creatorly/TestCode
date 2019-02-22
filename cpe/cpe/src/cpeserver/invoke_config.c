
#ifdef __cplusplus
extern "C" {
#endif

#include "invoke_config.h"

#define CPE_CFG_CFGFILE         "/etc/cpe/invoke.conf"

static utl_cfg_desc_t g_astCpeCfg[] =
{
    {CPE_CFG_ACCESS_SWITCH, UTL_CFGTYPE_INT, "CPE_CFG_ACCESS_SWITCH", "0"},
    {CPE_CFG_ACCESS_MODE, UTL_CFGTYPE_INT, "CPE_ACCESS_MODE", "0"},
    {CPE_CFG_BLACK_LIST, UTL_CFGTYPE_REC, "CPE_BLACK_LIST", NULL},
    {CPE_CFG_WHITE_LIST, UTL_CFGTYPE_REC, "CPE_WHITE_LIST", NULL},
    {CPE_CFG_PORT_FORWARD_LIST, UTL_CFGTYPE_REC, "CPE_CFG_PORT_FORWARD_LIST", NULL},
    
    /* TODO: add other config item */
};

PST_CPE_CFG CPE_CfgInit(const char* cfgfile)
{
    utl_cfg_h cfg = NULL;

    if (NULL == cfgfile || '\0' == *cfgfile)
    {
        cfgfile = CPE_CFG_CFGFILE;
    }
        
    if (utl_cfg_init(&cfg, cfgfile, g_astCpeCfg, utl_sizeofa(g_astCpeCfg)))
    {
        printf("failed to init cpe config handle!\r\n");
        return NULL;
    }
    
    return cfg;
}

int CPE_CfgFree(PST_CPE_CFG cfg)
{
    return 0;
}

int CPE_CfgSave(PST_CPE_CFG cfg)
{
    return utl_cfg_save(cfg);
}

int CPE_CfgReload(PST_CPE_CFG cfg)
{
    return utl_cfg_load(cfg);
}

int CPE_CfgGetInt(PST_CPE_CFG cfg, EN_CPE_CFG id)
{
    int iVal;

    utl_cfg_get_int(cfg, id, &iVal);

    return iVal;
}

int CPE_CfgGetStr(PST_CPE_CFG cfg, EN_CPE_CFG id, char* buf, int size)
{
    return utl_cfg_get_str_r(cfg, id, 0, buf, size);
}

void CPE_CfgSetInt(PST_CPE_CFG cfg, EN_CPE_CFG id, int iVal)
{
    utl_cfg_set(cfg, id, 0, iVal, NULL);
}

void CPE_CfgSetStr(PST_CPE_CFG cfg, EN_CPE_CFG id, const char* sVal)
{
    utl_cfg_set(cfg, id, 0, 0, sVal);
}

int CPE_CfgSize(PST_CPE_CFG cfg, EN_CPE_CFG id)
{
    return utl_cfg_size(cfg, id);
}

#ifdef __cplusplus
}
#endif

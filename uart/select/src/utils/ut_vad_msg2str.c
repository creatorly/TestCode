/********************************************************
* 
*
* Ejoin SIM Cloud platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         escc_msg2str.c
* PACKAGE:      
*   Implement of:       escc message to string
*   Date:               2016.10.24
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#include <stdio.h>
#include <string.h>
#include <utl_json.h>

#include <utl_tlv.h>
#include <vad_tlvdef.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef int (*FP_UT_VAD_PAR2STR)(char* buf, int size, const char* name, int len, unsigned char* val);

typedef struct tagstUtilsVadMsgParDesc
{
    int mtype;
    int pid;
    const char* pname;
    FP_UT_VAD_PAR2STR fpToStr;
}ST_VAD_MSG_PAR_DESC;

static const char* g_utVadMsgTyps[256] = {NULL};

/* 2:   for request or response */
/* 16:  for message type(0-15) */
/* 256: for message param(0-255) */
static ST_VAD_MSG_PAR_DESC g_utVadMsgPars[2][16][256] = {{{{0}}}};

int utVadMsgParIntToStr(char* buf, int size, const char* name, int len, unsigned char* val)
{
    int iVal = 0;
    int i;

    for (i = 0; i < len; i++)
    {
        iVal = (iVal << 8) | val[i];
    }

    return snprintf(buf, size, "%s:%d", name, iVal);
}

int utVadMsgParStrToStr(char* buf, int size, const char* name, int len, unsigned char* val)
{
    int ret;
    unsigned char cEnd = val[len];
    
    val[len] = '\0';
    ret = snprintf(buf, size, "%s:\"%s\"", name, (char*) val);
    val[len] = cEnd;

    return ret;
}

int utVadMsgParBinToStr(char* buf, int size, const char* name, int len, unsigned char* val)
{
    int blen;
    int i;

    if (len > 8)
    {
        blen = snprintf(buf, size, "%s(%d):", name, len);
    }
    else
    {
        blen = snprintf(buf, size, "%s:", name);
    }
    
    for (i = 0; i < len && blen + 3 < size; i++)
    {
        buf[blen++] = ' ';
        buf[blen++] = "0123456789ABCDEF"[(val[i] >> 4) & 0x0f];
        buf[blen++] = "0123456789ABCDEF"[val[i] & 0x0f];
    }

    if (blen < size)
    {
        buf[blen] = '\0';
    }

    return blen;
}

int utVadMsgParLenToStr(char* buf, int size, const char* name, int len, unsigned char* val)
{
    return snprintf(buf, size, "%s:%d", name, len);
}

int utVadMsgParIpToStr(char* buf, int size, const char* name, int len, unsigned char* val)
{
    if (len < 4)
    {
        return snprintf(buf, size, "%s:0.0.0.0", name);
    }
    else
    {
        return snprintf(buf, size, "%s:%d.%d.%d.%d", name, val[0], val[1], val[2], val[3]);
    }
}

int utVadMsgParMacToStr(char* buf, int size, const char* name, int len, unsigned char* val)
{
    if (len < 6)
    {
        return snprintf(buf, size, "%s:00-00-00-00-00-00", name);
    }
    else
    {
        return snprintf(buf, size, "%s:%02X-%02X-%02X-%02X-%02X-%02X", 
            name, val[0], val[1], val[2], val[3], val[4], val[5]);
    }
}

int utVadMsgParPwdToStr(char* buf, int size, const char* name, int len, unsigned char* val)
{
    return snprintf(buf, size, "%s:\"******\"", name);
}

void utVadMsgSetTypeDesc(int type, const char* name)
{
    g_utVadMsgTyps[type & 0xff] = name;
}

void utVadMsgSetMsgParDesc(int mtype, int pid, const char* pname, FP_UT_VAD_PAR2STR fpToStr)
{
    ST_VAD_MSG_PAR_DESC* pParDesc = &g_utVadMsgPars[(mtype >> 7) & 1][mtype & 0x0f][pid & 0xff];

    pParDesc->mtype = mtype & 0x0f;
    pParDesc->pid = pid & 0xff;
    pParDesc->pname = pname;
    pParDesc->fpToStr = fpToStr;
}

void utVadMsgSetParDesc(int pid, const char* pname, FP_UT_VAD_PAR2STR fpToStr)
{
    utVadMsgSetMsgParDesc(0, pid, pname, fpToStr);
}

ST_VAD_MSG_PAR_DESC* utVadMsgGetMsgParDesc(int mtype, int pid)
{
    ST_VAD_MSG_PAR_DESC* pParDesc = &g_utVadMsgPars[(mtype >> 7) & 1][mtype & 0x0f][pid & 0xff];

    return (pParDesc->fpToStr != NULL) ? pParDesc : &g_utVadMsgPars[0][0][pid & 0xff];
}

void utVadMsgDescInit()
{
    memset(&g_utVadMsgTyps, 0, sizeof(g_utVadMsgTyps));
    memset(&g_utVadMsgPars, 0, sizeof(g_utVadMsgPars));

    utVadMsgSetTypeDesc(ESCC_TLV_REQ_REG,        "REG");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_LOGIN,      "LOGIN");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_GET,        "GET");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_SETUP,      "SETUP");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_UPDATE,     "UPDATE");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_NOTIFY,     "NOTIFY");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_RELEASE,    "RELEASE");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_INFO,       "INFO");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_KEEP,       "KEEP");

    utVadMsgSetTypeDesc(ESCC_TLV_RSP_REG,        "REG-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_LOGIN,      "LOGIN-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_GET,        "GET-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_SETUP,      "SETUP-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_UPDATE,     "UPDATE-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_NOTIFY,     "NOTIFY-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_RELEASE,    "RELEASE-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_INFO,       "INFO-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_KEEP,       "KEEP-ACK");

    /* add the message name for VNS message */
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_VNS_HB,     "VHB");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_VNS_REG,    "VREG");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_VNS_NOTIFY, "VNOTIFY");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_VNS_SETUP,  "VSETUP");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_VNS_UPDATE, "VUPDATE");
    utVadMsgSetTypeDesc(ESCC_TLV_REQ_VNS_RELEASE,"VRELEASE");

    utVadMsgSetTypeDesc(ESCC_TLV_RSP_VNS_HB,     "VHB-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_VNS_REG,    "VREG-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_VNS_NOTIFY, "VNOTIFY-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_VNS_SETUP,  "VSETUP-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_VNS_UPDATE, "VUPDATE-ACK");
    utVadMsgSetTypeDesc(ESCC_TLV_RSP_VNS_RELEASE,"VRELEASE-ACK");

    /* add the param descriptions of common messages */
    utVadMsgSetParDesc(ESCC_TLV_PAR_SUBTYPE,     "subtype",  utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DESC,        "desc",     utVadMsgParStrToStr);

    utVadMsgSetParDesc(VAD_TLV_VDC_VERSION_BUILD_TIME,      "dev_ver_build_time",  utVadMsgParStrToStr);
    utVadMsgSetParDesc(VAD_TLV_VDC_VERSION,      "dev_ver",  utVadMsgParStrToStr);
    utVadMsgSetParDesc(VAD_TLV_VDC_UPDATE_FLAG,  "upd_flag",  utVadMsgParIntToStr);
    utVadMsgSetParDesc(VAD_TLV_VDC_UPDATE_FILE_URL,  "upd_furl",  utVadMsgParStrToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_EXPIRES,     "expires",  utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_USERNAME,    "username", utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_CNONCE,      "cnonce",   utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_NONCE,       "nonce",    utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_NC,          "nc",       utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_RESP,        "rsp",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_GETDATA,     "getdata",  utVadMsgParIntToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_CONN_SID,    "sid",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_CONN_PROTO,  "cpro",     utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_CONN_IP,     "cip",      utVadMsgParIpToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_CONN_PORT,   "cport",    utVadMsgParIntToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_UP0,    "rsim_up",     utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_DOWN0,  "rsim_down",   utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_UP1,    "lsim_up",     utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_DOWN1,  "lsim_down",   utVadMsgParIntToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_LOCIFID, "if_id0",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_REMIFID, "if_id1",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_MACLIST, "mac_list",    utVadMsgParStrToStr);

    /*add by chenjie 2018.05.10*/
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_SIMLBS,    "sim_lbs",   utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_GPS,       "gps",       utVadMsgParStrToStr); 
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_LIMITRATE, "max_speed", utVadMsgParIntToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_GSIM_ICCID,  "gsim_iccid",   utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_GSIM_DATADOWN, "gsim_down",  utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_GSIM_DATAUP, "gsim_up",      utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_GSIM_SIG,    "gsim_sig",     utVadMsgParIntToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_NAME,    "dev_name",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_PORT,    "max_port",     utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_MAC,     "dev_mac",      utVadMsgParMacToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_IP,      "dev_ip",       utVadMsgParIpToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_VER,     "dev_ver",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_TYPE,    "dev_type",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_STAT,    "dev_stat",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_CPU,     "cpu",          utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_MEM,     "free",         utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DEV_TEMP,    "temp",         utVadMsgParIntToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_SLOT,    "mod_slot",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_SIG,     "mod_sig",      utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_STAT,    "mod_stat",     utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_DESC,    "mod_desc",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_OPID,    "opid",         utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_OPNAME,  "opname",       utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_IMEI,    "imei",         utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_TYPE,    "mod_type",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_MOD_VER,     "mod_ver",      utVadMsgParStrToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_WIFI_CLIENTS,    "wifinum",  utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DOWN_SPEED,  "down_spd",     utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_UP_SPEED,    "up_spd",       utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_BAT_SOC,     "soc",          utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_BAT_STATUS,  "charging",     utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_BAT_VOLTAGE, "voltage",      utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_BAT_CURRENT, "current",      utVadMsgParIntToStr);

    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_SLOT,    "sim_slot",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_TYPE,    "sim_type",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_STAT,    "sim_stat",     utVadMsgParBinToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_LEDACT,  "led_act",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_LEDMOD,  "led_mod",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_ICCID,   "iccid",        utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_IMSI,    "imsi",         utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_NUM,     "sim_num",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_BAL,     "sim_bal",      utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_HASH,    "sim_hash",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_LEN,     "sim_len",      utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SIM_DATA,    "sim_data",     utVadMsgParLenToStr);

    /*add by chenjie. 2018.05.18*/
    utVadMsgSetParDesc(ESCC_TLV_PAR_ERROR_CODE,      "err_code",         utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_ERROR_DESC,      "err_desc",         utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_SURPLUS_FLOW,    "flow_remain",      utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_FLOW_STATISTIC,  "flow_stat",        utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_DATA_APPINFO,    "data_appinfo",     utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_RUN_CMD,         "run_cmd",          utVadMsgParStrToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_IDLE_CONTROL,    "idle_control",     utVadMsgParStrToStr);

    /* add by xkh 2018.12.04*/
    utVadMsgSetParDesc(ESCC_TLV_PAR_GSIM_MAXSPEED,          "gsim_maxspeed",            utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_GSIM_REMAIN,            "gsim_reamin",              utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_LSIM_MAXSPEED,          "lsim_maxspeed",            utVadMsgParIntToStr);
    utVadMsgSetParDesc(ESCC_TLV_PAR_LSIM_REMAIN,            "lsim_reamin",              utVadMsgParIntToStr);
   
    /* add the param descriptions of VRS LOGIN request */
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_VID, "did",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_SEQ, "seq",    utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_NON, "nonce",  utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_RSP, "resp",   utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_NET, "net",    utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_VER, "ver",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_ICC, "iccid",  utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_LOGIN, VAD_TLV_VRS_REQ_OPI, "opid",   utVadMsgParStrToStr);

    /* add the param descriptions of VRS LOGIN response */
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_DESC,      "vrs_desc", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_VNSIP,     "vns_ip",   utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_VNSPORT,   "vns_port", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_EXP,       "vns_exp",  utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_VNSPRO,    "vns_pro",  utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_ACTION,    "action",   utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_OEMNAME,   "oem_name", utVadMsgParStrToStr);
    
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_WIFIMAXASSOC,  "wifi_max", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_SVRPINTYPE,    "gsim_pin", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_LOGIN, VAD_TLV_VRS_RSP_DEBUGMODE,     "debug",    utVadMsgParIntToStr);

    /* add the param descriptions of VNS GET request */
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_SIMLBS,     "bsinf",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_LOCATION,   "locat",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_TCONSUME,   "vns_rtt",  utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_FIRSTMNC,   "fst_mnc",  utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_CELLID,     "cellid",   utVadMsgParIntToStr);
    
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_ICCID,  "iccid",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_LFC,    "lfc",      utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_LAC,    "lac",      utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_MCC,    "mcc",      utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_OPI,    "opid",     utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_RSP,    "resp",     utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_SEQ,    "seq",      utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_TGT,    "tgt",      utVadMsgParStrToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_IMEI,   "imei",     utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_RIMEI,  "imei2",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_VID,    "did",      utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_SESID,  "sid",      utVadMsgParStrToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_NETTYPE, "net",     utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_LICCID,  "iccid3",  utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_LSIMFLAG,"lsim_cause",  utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_LSIMDESC,"lsim_reason", utVadMsgParStrToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET,  VAD_TLV_VNS_REQ_LASTGPIN_ERROR,"last_gsim_pin_result", utVadMsgParIntToStr);

    /* xkh add 20190107*/
    utVadMsgSetMsgParDesc(ESCC_TLV_REQ_GET, VAD_TLV_VNS_REQ_DEVROM, "devRomDate",    utVadMsgParIntToStr);

    /* add the param descriptions of VNS GET response */
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_SWITCHLSIM, "lsim_flag", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_PORT_AUTH,  "pauth",    utVadMsgParStrToStr);
    
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_IMEI,       "bimei",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_SIMROAM,    "sim_roam", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_ICCID,      "iccid",    utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_IMSI,       "imsi",     utVadMsgParStrToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_TYPE,   "vpn_type", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_PROTO,  "vpn_proto", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_ONLY,   "vpn_only", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_HOST,   "vpn_host", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_PORT,   "vpn_port", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_AUTH,   "vpn_auth", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_USER,   "vpn_user", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_PASS,   "vpn_pass", utVadMsgParPwdToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_CERT,   "vpn_cert", utVadMsgParBinToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VPN_DNS,    "vpn_dns",  utVadMsgParStrToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_NETTYPE,    "net_type", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_DESC,   "vns_desc", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_SEQ,    "seq",      utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VID,    "did",      utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_SESID,  "sid",      utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VEN,    "voc_enb",  utVadMsgParIntToStr);
    
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_DPAPN,  "apn_name", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_DPNUM,  "apn_num",  utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_DPUSR,  "apn_user", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_DPPWD,  "apn_pass", utVadMsgParStrToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_ACTION, "action",   utVadMsgParStrToStr);
    
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VSWPRO, "vsw_pro",  utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VSWIP,  "vsw_ip",   utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_VSWPORT,"vsw_port", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_EXP,    "vsw_exp",  utVadMsgParIntToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_APPLOG, "log_mode", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_URLLOG, "log_url",  utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LOG,    "log_lvl",  utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LOGID,  "log_id",   utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LOGIP,  "log_ip",   utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LOGPORT,"log_port", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LOGPRO, "log_pro",  utVadMsgParStrToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LSIM_REMAIN, "lsim_remain", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LSIM_ENDDATE, "lsim_enddate", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LSIM_MAXSPEED, "lsim_maxspeed", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LSIM_MODE, "lsim_mode", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_LSIM_PIN, "lsim_pin", utVadMsgParPwdToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_GSIM_PIN, "gsim_pin", utVadMsgParStrToStr);

    /* xkh add 20180910 */ 
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_GSIM_MAXSPEED,   "gsim_maxspeed",   utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_GSIM_REMAIN,     "gsim_remain",     utVadMsgParIntToStr);


    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_DEVROM_UPDATE_FLAG,     "devRom_update_flag",     utVadMsgParIntToStr);

    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_VNS_UPDATE,  VAD_TLV_VNS_RSP_LSIM_REMAIN, "lsim_remain", utVadMsgParIntToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_VNS_UPDATE,  VAD_TLV_VNS_RSP_LSIM_ENDDATE, "lsim_enddate", utVadMsgParStrToStr);
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_VNS_UPDATE,  VAD_TLV_VNS_RSP_LSIM_MAXSPEED, "lsim_maxspeed", utVadMsgParIntToStr);

    /* xkh add 20190114*/
    utVadMsgSetMsgParDesc(ESCC_TLV_RSP_GET,  VAD_TLV_VNS_RSP_SSIM_INFO,     "soft_sim_info",     utVadMsgParStrToStr);
}

int UT_VAD_MsgToStr(ST_VAD_TLV_HEADER* msg, char* buf, int size)
{
    static int bParDescInited = false;
    int len;

    if (!bParDescInited)
    {
        utVadMsgDescInit();
        bParDescInited = true;
    }

    if (VAD_TLV_MSG_ISREQ(msg->type))
    {
        len = snprintf(buf, size, "{sid:%X, type:%s, seq:%d, len:%d}", 
            msg->sid, g_utVadMsgTyps[msg->type], msg->seq, msg->len);
    }
    else
    {
        len = snprintf(buf, size, "{sid:%X, type:%s, code:%d, seq:%d, len:%d}", 
            msg->sid, g_utVadMsgTyps[msg->type], msg->code, msg->seq, msg->len);
    }
    
    if (msg->len > 0)
    {
        utl_tlv_it_t it;

        --len;  /* ignore the last '}' */

        utl_tlv_get_first(msg->data, msg->len, &it);

        while (it.val != NULL && len+2 < size)  /* 2 for ", " */
        {
            ST_VAD_MSG_PAR_DESC* pParDesc = utVadMsgGetMsgParDesc(msg->type, it.tag);

            if (pParDesc->fpToStr != NULL)
            {
                len += sprintf(buf+len, ", ");
                len += pParDesc->fpToStr(buf+len, size-len, pParDesc->pname, it.len, it.val);
            }
            
            utl_tlv_get_next(&it);
        }

        if (len >= size)
        {
            len = size - 1;
        }

        if (len + 1 < size)
        {
            buf[len++] = '}';
        }

        buf[len] = '\0';
    }

    return len;
}

#ifdef __cplusplus
}
#endif


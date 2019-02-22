
/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2017-2017, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vad_tlvdef.h
* PACKAGE:      
*   Date:               2017.10.11
*   Author:             pj
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VAD_TLVDEF_H__
#define  __VAD_TLVDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define VAD_TLV_PROTO_VER               1
#define VAD_TLV_PROTO_HEADER_MAGIC      0x8a

typedef struct tagstVad_Tlv_Header
{
#if (EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN)
    unsigned char v:4;      /* protocol version, now is 1 */
    unsigned char f:4;      /* flags, reserved */
#endif
#if (EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN)
    unsigned char f:4;
    unsigned char v:4;
#endif
    unsigned char magic;    /* protocol magic id: 138 */
    unsigned char type;     /* message type */
    unsigned char code;     /* response code */
    
    unsigned int sid;        /* session id */
    
    unsigned short seq;     /* request sequence */
    unsigned short len;     /* the length of TLV parameters */

    char data[0];
}__attribute__((__packed__)) ST_VAD_TLV_HEADER;


/******************************************************************************
 * define the type of TLV message
 */
typedef enum
{
    VAD_TLV_CMD_NONE = 0,
    VAD_TLV_CMD_REG,
    
    VAD_TLV_CMD_VRS_LOGIN,
    VAD_TLV_CMD_VNS_GET,
    
    VAD_TLV_CMD_OPEN,
    VAD_TLV_CMD_UPDATE,
    VAD_TLV_CMD_NOTIFY,
    VAD_TLV_CMD_CLOSE = 0x0f,

    VAD_TLV_CMD_DATA  = 0x10,
    VAD_TLV_CMD_SIMIO,
        
    VAD_TLV_CMD_KEEP = 0x7f,
}EN_VAD_TLV_CMD_TYPE;

#define VAD_TLV_MSG_ISRSP(msg)     ((msg) & 0x80)
#define VAD_TLV_MSG_ISREQ(msg)     (!VAD_TLV_MSG_ISRSP(msg))
#define VAD_TLV_REQ2RSP(req)       (0x80 | ((req) & 0x7f))

/* define the type of TLV request for ESCC */
#define ESCC_TLV_REQ_REG            VAD_TLV_CMD_REG
#define ESCC_TLV_REQ_GET            VAD_TLV_CMD_VNS_GET
#define ESCC_TLV_REQ_SETUP          VAD_TLV_CMD_OPEN
#define ESCC_TLV_REQ_UPDATE         VAD_TLV_CMD_UPDATE
#define ESCC_TLV_REQ_NOTIFY         VAD_TLV_CMD_NOTIFY
#define ESCC_TLV_REQ_RELEASE        VAD_TLV_CMD_CLOSE

#define ESCC_TLV_REQ_INFO           VAD_TLV_CMD_DATA
#define ESCC_TLV_REQ_KEEP           VAD_TLV_CMD_KEEP
    
/* define the type of TLV response for ESCC */
#define ESCC_TLV_RSP_REG            VAD_TLV_REQ2RSP(ESCC_TLV_REQ_REG)
#define ESCC_TLV_RSP_GET            VAD_TLV_REQ2RSP(ESCC_TLV_REQ_GET)
#define ESCC_TLV_RSP_SETUP          VAD_TLV_REQ2RSP(ESCC_TLV_REQ_SETUP)
#define ESCC_TLV_RSP_UPDATE         VAD_TLV_REQ2RSP(ESCC_TLV_REQ_UPDATE)
#define ESCC_TLV_RSP_NOTIFY         VAD_TLV_REQ2RSP(ESCC_TLV_REQ_NOTIFY)
#define ESCC_TLV_RSP_RELEASE        VAD_TLV_REQ2RSP(ESCC_TLV_REQ_RELEASE)
#define ESCC_TLV_RSP_INFO           VAD_TLV_REQ2RSP(ESCC_TLV_REQ_INFO)
#define ESCC_TLV_RSP_KEEP           VAD_TLV_REQ2RSP(ESCC_TLV_REQ_KEEP)

/**************************************************************************
 * define the TLV message subtype
 */

/* define the subtype for message NOTIFY */
#define ESCC_TLV_UPDATE_KEEP        0
#define ESCC_TLV_UPDATE_USER        1

/* define the subtype for message NOTIFY */
#define ESCC_TLV_NOTIFY_SIM         1
#define ESCC_TLV_NOTIFY_CDR         2
#define ESCC_TLV_NOTIFY_SMS         3
#define ESCC_TLV_NOTIFY_USSD        4

/* define the subtype for message INFO */
#define ESCC_TLV_INFO_DATA          0
#define ESCC_TLV_INFO_CMD           1

/**************************************************************************
 * define the code of response 
 */
#define ESCC_TLV_CODE_OK                0

#define ESCC_TLV_CODE_SYSTEM_SHUTDOWN   11
#define ESCC_TLV_CODE_SYSTEM_BUSY       12
#define ESCC_TLV_CODE_DEVICE_NOTFOUND   13
#define ESCC_TLV_CODE_DEVICE_STATEERROR 14
#define ESCC_TLV_CODE_AREA_NOTSUPPORT   15
#define ESCC_TLV_CODE_RATE_NOTFOUND     16
#define ESCC_TLV_CODE_USER_NOBIND       17
#define ESCC_TLV_CODE_USER_NOFOUND      21
#define ESCC_TLV_CODE_USER_STATEERROR   22
#define ESCC_TLV_CODE_USER_NOMONEY      23
#define ESCC_TLV_CODE_AGENT_NOMONEY     24
#define ESCC_TLV_CODE_USER_OUTLIMIT     25

#define ESCC_TLV_CODE_VSWSVR_OFFLINE    26
#define ESCC_TLV_CODE_SIMP_OFFLINE      28
#define ESCC_TLV_CODE_SIMP_PORTERROR    29
#define ESCC_TLV_CODE_SIM_SPLCHANGE     31
#define ESCC_TLV_CODE_SIM_NOFOUND       32
#define ESCC_TLV_CODE_SIM_DISABLE       33
#define ESCC_TLV_CODE_SIM_STATEERROR    34
#define ESCC_TLV_CODE_SIM_RESDATALIMIT  35
#define ESCC_TLV_CODE_SIM_BUSY          36
#define ESCC_TLV_CODE_SIM_AREA_NOTSUPPORT 37
#define ESCC_TLV_CODE_SIM_NOPERMISSION  38
#define ESCC_TLV_CODE_SIM_NOENABLE      39

#define ESCC_TLV_CODE_BAD_REQUEST       40
#define ESCC_TLV_CODE_UNAUTHORIZED      41
#define ESCC_TLV_CODE_FORBIDDEN         43
#define ESCC_TLV_CODE_NOT_FOUND         44
#define ESCC_TLV_CODE_NOT_ALLOWED       45
#define ESCC_TLV_CODE_NOT_ACCEPTABLE    46
#define ESCC_TLV_CODE_BAD_PARAM         47
#define ESCC_TLV_CODE_UNAVAILABLE       48

#define ESCC_TLV_CODE_SVR_ERROR         50
#define ESCC_TLV_CODE_SVR_NOTIMPL       51
#define ESCC_TLV_CODE_SVR_UNAVAIL       53

/**************************************************************************
 * define the TLV common parameters  
 */
#define ESCC_TLV_PAR_SUBTYPE        1
#define ESCC_TLV_PAR_DESC           2


/**************************************************************************
 * define the parameters of TLV message for VRS/VNS 
 */
typedef enum
{
    VAD_TLV_VRS_REQ_NONE = 0,

    VAD_TLV_VRS_REQ_VID = 51,
    VAD_TLV_VRS_REQ_SEQ,
    VAD_TLV_VRS_REQ_NON,
    VAD_TLV_VRS_REQ_RSP,
    VAD_TLV_VRS_REQ_NET,
    VAD_TLV_VRS_REQ_VER,
    VAD_TLV_VRS_REQ_ICC,
    VAD_TLV_VRS_REQ_OPI,
    
    VAD_TLV_VRS_REQ_MAX,
}EN_VAD_TLV_VRS_REQ_PARAMTYPE;

typedef enum
{
    VAD_TLV_VRS_RSP_NONE = 0,

    VAD_TLV_VRS_RSP_DESC = 61,
    VAD_TLV_VRS_RSP_VNSIP,
    VAD_TLV_VRS_RSP_VNSPORT,
    VAD_TLV_VRS_RSP_EXP,
    VAD_TLV_VRS_RSP_VNSPRO,
    VAD_TLV_VRS_RSP_ACTION,
    VAD_TLV_VRS_RSP_OEMNAME,

    VAD_TLV_VRS_RSP_WIFIMAXASSOC = 69,
    VAD_TLV_VRS_RSP_SVRPINTYPE,
    VAD_TLV_VRS_RSP_DEBUGMODE,
    
    VAD_TLV_VRS_RSP_MAX,
}EN_VAD_TLV_VRS_RSP_PARAMTYPE;

typedef enum
{
    VAD_TLV_VNS_REQ_NONE = 0,

    VAD_TLV_VNS_REQ_SIMLBS = 60,
    VAD_TLV_VNS_REQ_LOCATION = 64,
    VAD_TLV_VNS_REQ_TCONSUME,
    VAD_TLV_VNS_REQ_FIRSTMNC,
    
    VAD_TLV_VNS_REQ_CELLID = 71,
    VAD_TLV_VNS_REQ_ICCID,
    VAD_TLV_VNS_REQ_LFC,
    VAD_TLV_VNS_REQ_LAC,
    VAD_TLV_VNS_REQ_MCC,
    VAD_TLV_VNS_REQ_OPI,
    VAD_TLV_VNS_REQ_RSP,
    VAD_TLV_VNS_REQ_SEQ,
    VAD_TLV_VNS_REQ_TGT,
    
    VAD_TLV_VNS_REQ_IMEI = 80,  /* the IMEI of boot module */
    VAD_TLV_VNS_REQ_RIMEI,      /* the IMEI of remote module */
    
    VAD_TLV_VNS_REQ_VID = 83,
    VAD_TLV_VNS_REQ_SESID,
    
    VAD_TLV_VNS_REQ_MAX,
}EN_VAD_TLV_VNS_REQ_PARAMTYPE;

typedef enum
{
    VAD_TLV_VNS_RSP_NONE = 0,

    VAD_TLV_VNS_RSP_URLLOG = 71,
    VAD_TLV_VNS_RSP_IMEI,
    VAD_TLV_VNS_RSP_SIMROAM,

    VAD_TLV_VNS_RSP_DESC = 81,
    VAD_TLV_VNS_RSP_SEQ,
    VAD_TLV_VNS_RSP_VID,
    VAD_TLV_VNS_RSP_SESID,
    VAD_TLV_VNS_RSP_EXP,
    VAD_TLV_VNS_RSP_VEN,
    VAD_TLV_VNS_RSP_DPAPN,
    VAD_TLV_VNS_RSP_DPNUM,
    VAD_TLV_VNS_RSP_DPUSR,
    VAD_TLV_VNS_RSP_DPPWD,
    VAD_TLV_VNS_RSP_ACTION,
    VAD_TLV_VNS_RSP_VSWPRO,
    VAD_TLV_VNS_RSP_VSWIP,
    VAD_TLV_VNS_RSP_VSWPORT,
    VAD_TLV_VNS_RSP_LOG,
    VAD_TLV_VNS_RSP_LOGID,
    VAD_TLV_VNS_RSP_LOGIP,
    VAD_TLV_VNS_RSP_LOGPORT,
    VAD_TLV_VNS_RSP_PRO,
    VAD_TLV_VNS_RSP_APPLOG,    /*for app autolog    */
    
    VAD_TLV_VNS_RSP_MAX,
}EN_VAD_TLV_VNS_RSP_PARAMTYPE;

/**************************************************************************
 * define the parameters of TLV message for VSW(ESCC) 
 */
 
#define VAD_TLV_VDC_PORTAL_AUTHENTICATE 11
#define VAD_TLV_VDC_VERSION             59
#define VAD_TLV_VDC_UPDATE_FLAG         67
#define VAD_TLV_VDC_UPDATE_FILE_URL     68

/**************************************************************************
 * define the parameters of TLV message for SWITCH TO LOCAL_SIM 
 */

#define VAD_TLV_VNS_REQ_SUBTYPE        1
#define VAD_TLV_VNS_REQ_NETTYPE        160
#define VAD_TLV_VNS_REQ_LICCID         161
#define VAD_TLV_VNS_REQ_LSIMFLAG       162
#define VAD_TLV_VNS_REQ_LSIMDESC       163

#define VAD_TLV_VNS_RSP_SWITCHLSIM     10

#define ESCC_TLV_PAR_EXPIRES    101
#define ESCC_TLV_PAR_USERNAME   102
#define ESCC_TLV_PAR_CNONCE     103
#define ESCC_TLV_PAR_NONCE      104
#define ESCC_TLV_PAR_NC         105
#define ESCC_TLV_PAR_RESP       106
#define ESCC_TLV_PAR_GETDATA    107

#define ESCC_TLV_PAR_CONN_SID   120
#define ESCC_TLV_PAR_CONN_PROTO 121
#define ESCC_TLV_PAR_CONN_IP    122
#define ESCC_TLV_PAR_CONN_PORT  123

#define ESCC_TLV_PAR_ERROR_CODE 127
#define ESCC_TLV_PAR_ERROR_DESC 128

#define ESCC_TLV_PAR_DATA_UP0   130         /* the up traffic in KBs of remote SIM */
#define ESCC_TLV_PAR_DATA_DOWN0 131         /* the down traffic in KBs of remote SIM */
#define ESCC_TLV_PAR_DATA_UP1   132         /* the up traffic in KBs of local SIM */
#define ESCC_TLV_PAR_DATA_DOWN1 133         /* the down traffic in KBs of local SIM */
#define ESCC_TLV_PAR_DATA_LOCIFID   134
#define ESCC_TLV_PAR_DATA_REMIFID   135
#define ESCC_TLV_PAR_DATA_MACLIST   136
#define ESCC_TLV_PAR_DATA_SIMLBS    137
#define ESCC_TLV_PAR_DATA_GPS       138

#define ESCC_TLV_PAR_DATA_LIMITRATE 139

#define ESCC_TLV_PAR_GSIM_ICCID     140
#define ESCC_TLV_PAR_GSIM_DATAUP    141     /* the up traffic in KBs of boot SIM */
#define ESCC_TLV_PAR_GSIM_DATADOWN  142     /* the up traffic in KBs of boot SIM */
#define ESCC_TLV_PAR_SURPLUS_FLOW   143     /* the surplus flow in KBs */
#define ESCC_TLV_PAR_FLOW_STATISTIC 144     /* remote sim flow statistic*/
#define ESCC_TLV_PAR_DATA_APPINFO   145     /* APP information for UPDATE*/
#define ESCC_TLV_PAR_RUN_CMD        146     /* run some commands */
#define ESCC_TLV_PAR_IDLE_CONTROL   148     /* IDLE CONTROL: idleTimes, idleAction:0(NONE),1(shutdown),2(sleep)*/

#define ESCC_TLV_PAR_DEV_NAME   150
#define ESCC_TLV_PAR_DEV_PORT   151
#define ESCC_TLV_PAR_DEV_MAC    152
#define ESCC_TLV_PAR_DEV_IP     153
#define ESCC_TLV_PAR_DEV_VER    154
#define ESCC_TLV_PAR_DEV_TYPE   155

#define ESCC_TLV_PAR_MOD_SLOT   160
#define ESCC_TLV_PAR_MOD_SIG    161
#define ESCC_TLV_PAR_MOD_STAT   162
#define ESCC_TLV_PAR_MOD_DESC   163
#define ESCC_TLV_PAR_MOD_OPID   164
#define ESCC_TLV_PAR_MOD_OPNAME 165

#define ESCC_TLV_PAR_MOD_IMEI   170
#define ESCC_TLV_PAR_MOD_TYPE   171
#define ESCC_TLV_PAR_MOD_VER    172

#define ESCC_TLV_PAR_SIM_SLOT   180
#define ESCC_TLV_PAR_SIM_TYPE   181
#define ESCC_TLV_PAR_SIM_STAT   182
#define ESCC_TLV_PAR_SIM_LEDACT 183
#define ESCC_TLV_PAR_SIM_LEDMOD 184

#define ESCC_TLV_PAR_SIM_ICCID  190
#define ESCC_TLV_PAR_SIM_IMSI   191
#define ESCC_TLV_PAR_SIM_NUM    192
#define ESCC_TLV_PAR_SIM_BAL    193
#define ESCC_TLV_PAR_SIM_HASH   197
#define ESCC_TLV_PAR_SIM_LEN    198
#define ESCC_TLV_PAR_SIM_DATA   199

/**************************************************************************
 * define the parameters of TLV message for SIM data(WIRELESS)
 */
#define ESCC_TLV_PAR_SIM_PATH   0x91
#define ESCC_TLV_PAR_SIM_CAPDU  0x92

#define ESCC_TLV_PAR_SIM_RAPDU  0x96
#define ESCC_TLV_PAR_SIM_APPEND 0x97

#ifdef __cplusplus
}
#endif

#endif



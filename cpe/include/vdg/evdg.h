/********************************************************
* 
*
* Ejoin SIM Pool Client
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         evdg.h
* PACKAGE:      
*   Header of:          evdg.c
*   Date:               2015.09.10
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __EVDG_H__
#define  __EVDG_H__

#include <utl_log.h>
#include <vdg/vdg_ver.h>
#include <vad_config.h>
#include "utl_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVDG_PROTO_TYPE     1

typedef enum
{
    EVDG_PARAM_LOG2STDOUT = 0,  /* boolean, print the log to stdout */

    EVDG_PARAM_TIMEZONE,
        
    EVDG_PARAM_SERVER,          /* string, the ESP Server address(ip[:port], default port is 20000) */
    EVDG_PARAM_USR,             /* string, the ESP registration username */
    EVDG_PARAM_PWD,             /* string, the ESP registration password */

    EVDG_PARAM_DATA_TRANSPORT,  /* enum, the transport(default is UDPTL) of data link */
}EN_EVDG_PARAM;

typedef enum
{
    EVDG_CMD_NONE = 0x80,       /* 0x80=128 means command */
        
    EVDG_CMD_GET,               /* to get device information */
    EVDG_CMD_SET,               /* to set some parameters */
    
    EVDG_CMD_OPEN,
    EVDG_CMD_CLOSE,

    EVDG_CMD_CONN,              /* connect to the 3G/4G network */
    EVDG_CMD_DISC,              /* disconnect from the 3G/4G network */
    EVDG_CMD_APPSET,            /* app to set some  parameters*/
    EVDG_CMD_GETAPPINFO,        /* APP information for UPDATE */
}EN_EVDG_CMD;

typedef enum
{
    EVDG_EVENT_NONE = 0,        /* no event or useless event is received */

    EVDG_EVENT_DEVINFO,         /* device information for command GET */

    EVDG_EVENT_OPENED,          /* the EVDG session is opened(SIM is allocated) */
    EVDG_EVENT_UPDATED,         /* the EVDG session is updated */
    EVDG_EVENT_CLOSED,          /* the EVDG session is closed(SIM is free) */

    EVDG_EVENT_SIM_ONLINE,      /* SIM is registered to operator */
    EVDG_EVENT_SIM_OFFLINE,     /* SIM is not registered */

    EVDG_EVENT_IFUP,            /* 3G or 4G network is connected */
    EVDG_EVENT_IFDOWN,          /* 3G or 4G network is disconnected */

    /* the call event definition, see the evdg_cdr_t for details */
    EVDG_EVENT_CALL_BEGIN,
    EVDG_EVENT_CALL_ALERT,
    EVDG_EVENT_CALL_ANSWER,
    EVDG_EVENT_CALL_END,

    EVDG_EVENT_CMD_CONN,        /* connect option is selected */
    EVDG_EVENT_CMD_DISC,        /* disconnect option is selected */
    EVDG_EVENT_GETAPPINFO,      /* APP information for UPDATE */
    EVDG_EVENT_DOUBLE_KEY,      /* double_key event */
}EN_EVDG_EVENT;

typedef enum
{
    EVDG_TRANSPORT_UDPTL = 0,
    EVDG_TRANSPORT_UDP,
    EVDG_TRANSPORT_TCP,
}EN_EVDG_TRANSPORT;

typedef enum
{
    EVDG_SIGTYPE_2G = 0,
    EVDG_SIGTYPE_3G,
    EVDG_SIGTYPE_4G,
}EN_EVDG_SIGTYPE;

typedef enum
{
    EVDG_APN_3GNET = 0,
    EVDG_APN_CMNET,
}EN_EVDG_APN;

typedef enum
{
    EVDG_UE_TYPE_NONE = 0,
    EVDG_UE_TYPE_IFUP,              /* evdg_evt_ifup_t */
    EVDG_UE_TYPE_IFDOWN,            /* evdg_evt_ifdwn_t */
    EVDG_UE_TYPE_CONN,              /* the connect option is selected */
    EVDG_UE_TYPE_DISC,              /* the disconnect option is selected */
    EVDG_UE_TYPE_DBKEY,              /* double_key event */
}EN_EVDG_UE_TYPE;

typedef enum
{
    EVDG_SALINFO_NONE = 0,
    EVDG_SALINFO_RECOVERY,
    EVDG_SALINFO_SHUTDOWN,
    EVDG_SALINFO_REBOOT,
    EVDG_SALINFO_DEVID,
    EVDG_SALINFO_CHARGE,
    EVDG_SALINFO_LOCALSIM,
    EVDG_SALINFO_LANIP,
    EVDG_SALINFO_WANIP,
    EVDG_SALINFO_PRIMARYDNS,
    EVDG_SALINFO_SECONDARYDNS,
    EVDG_SALINFO_WIFISTATUS,
    EVDG_SALINFO_WIFIRESTART,
    EVDG_SALINFO_WIFISSID,
    EVDG_SALINFO_WIFIKEY,
    EVDG_SALINFO_WIFIHIDDEN,
    EVDG_SALINFO_WIFICHANNEL,
    EVDG_SALINFO_WIFIENCRYPTION,
    EVDG_SALINFO_WIFICLIENTS,
    EVDG_SALINFO_WIFIRESET,
    EVDG_SALINFO_WIFIMAXASSOC,    
    EVDG_SALINFO_IFTXBYTES,
    EVDG_SALINFO_IFRXBYTES,
    EVDG_SALINFO_CLEAR_FLOW,
    EVDG_SALINFO_KEYEVENT,
}EVDG_SALINFO_ITEM;

/* for the event DEVINFO */
typedef struct tagstEvdgChannelInfo
{
    char iccid[24];
    
    int op_id;
    char op_name[32];
    int sigint;             /* 0~31 */
    int sigtype;            /* 2G/3G/4G */

    int mcc;
    int mnc;
    int lac;
    int cellid;
    
    int rxKbs;
    int txKbs;
    int rxSpeed;
    int txSpeed;
}evdg_chninfo_t;

/* for the event DEVINFO */
typedef struct tagstEvdgDeviceInfo
{
    char dev_id[36];
    
    int battery;
    int clients;

    evdg_chninfo_t chis[2];     /* boot(0) and data(1) channel information */
}evdg_devinfo_t;

/* for the event ONLINE/OFFLINE, SIM_ONLINE/SIM_OFFLINE */
typedef struct tagstEvdgStatus
{
    int status;
    char reason[32];
}evdg_status_t;

/* for the event CALL_XXX */
typedef struct tagstEvdgCallDetails
{
    int dir;                    /* call directory, 0: received from GSM, 1: sent to GSM */
    char caller[32];
    char callee[32];

    int ts_begin;               /* timestamp when the call begin */
    int ts_alert;
    int ts_answer;
    int ts_end;
}evdg_cdr_t;

/* for the event IFUP */
typedef struct tagstEvdgEventIfup
{
    int channel;                /* 0: 3G booting SIM, 1: 4G remote SIM */
    char ifname[32];

    char ip[32];
    char mask[32];
    char gw[32];
    char dns[32];
}evdg_evt_ifup_t;

/* for the event IFUP */
typedef struct tagstEvdgEventIfdown
{
    int channel;                /* 0: 3G booting SIM, 1: 4G remote SIM */
    char ifname[32];
}evdg_evt_ifdown_t;

/* for the event OPENED/UPDATED/CLOSED */
typedef struct tagstEvdgEventEspMsg
{
    int len;
    char msg[2048];
}evdg_evt_espmsg_t;

typedef struct tagstEvdgEvent
{
    EN_EVDG_EVENT evt;
    int channel;
    int len;
    
    union
    {
        evdg_devinfo_t di;
        evdg_status_t st;
        evdg_cdr_t cdr;

        evdg_evt_ifup_t ifup;
        evdg_evt_ifdown_t ifdown;

        evdg_evt_espmsg_t esp_msg;
    }u;
}evdg_evt_t;

typedef struct tagstEvdgCmdOpen
{
    char sid[36];       /* sessison id */
    char sip[32];       /* server ip address */
    int  sport;         /* server port */
    char imei[16];
    int  conn_proto;
}evdg_cmd_open_t;

typedef struct tagstEvdgCmdClose
{
    char sid[36];       /* sessison id */
    int code;
    char reason[32];        
}evdg_cmd_close_t;

typedef struct tagstEvdgCmdConnect
{
    int channel;
    
    int op_id;
    char op_name[32];

    char apn[16];
    char num[16];
    char usr[32];
    char pwd[32];

    char action[256];
}evdg_cmd_conn_t;

typedef struct tagstEvdgCmdDisconnect
{
    int channel;
}evdg_cmd_disc_t;

typedef struct tagstEvdgCmdSet
{
    int channel;

    int plen;           /* the length of pars */
    char pars[512];     /* parameters in json format for command SET */
}evdg_cmd_set_t;


typedef struct tagstEvdgCmdGetAppInfo
{
    int channel;

    int plen;           /* the length of pars */
    char *pars_ptr;     /* parameters in json format for APP information */
}evdg_cmd_getappinfo_t;


/* used to send command to evdg from other internal process */
typedef struct tagstEvdgCommand
{
    EN_EVDG_CMD cmd;

    union
    {
        evdg_cmd_open_t open;
        evdg_cmd_close_t close;
        evdg_cmd_conn_t conn;
        evdg_cmd_disc_t disc;

        evdg_cmd_set_t set;
        evdg_cmd_getappinfo_t appinfo;
    }u;
}evdg_cmd_t;

/**
 * INITIALIZE the ejoin simpool client
 * @param home_dir: the home directory of log
 * @param logsize:  the size in MBs of each log file
 * @param logcnt:   the count of log files
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int evdg_init(const char* home_dir, int logsize, int logcnt);

/**
 * SET the configuration of the evdg
 * @param param:    the param to be set
 * @param index:    the param index for the array configuration
 * @param val:      the pointer to the param value
 * @param len:      the length of param value
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int evdg_set(int param, int index, const void* val, int len);

/**
 * START the ejoin simpool client
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int evdg_start();

/**
 * RECV a evdg event within the specified time
 * @param evt:      used to store the event received
 * @param tmo:      the specified timeout in milli-seconds
 *                  > 0: blocking specified time till a event is received
 *                  < 0: blocking forever till a event is received
 *                  = 0: no blocking event if no event is received
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 * 
 * @note:           if the event receiver is set, no event will be received
 */
int evdg_recv(evdg_evt_t* evt, int tmo);

/**
 * STOP the ejoin simpool client
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
void evdg_stop();

int evdg_send_cmd(evdg_cmd_t* cmd);

evdg_evt_t* evdg_evt_recv(int tmo);
void evdg_evt_free(evdg_evt_t* pEvt);

utl_log_t* evdg_getlog();
ST_VAD_PUB_DATA * evdg_get_pubdata();
ST_VAD_FLASH_DATA* evdg_get_flashdata();

void evdg_flush_flashdata();

int* evdg_get_dst();
int* evdg_get_dpc();
int* evdg_get_syserr();
int* evdg_get_syswarn();
int evdg_islocalcard();
int evdg_atsenddata(const char *pDstIp, int DstPort, const void* pData, int Len, int Reuse, int TimeOut);
int evdg_atrcvdata(void* pData, int pSize, int *pLen, int TimeOut);
int evdg_atqiclose(int index);
int evdg_simpinset(int index);
void evdg_sleep_to_start(void);
void evdg_working_to_sleep(void);
int evdg_set_param(EVDG_SALINFO_ITEM index, int value, const void* string);
int evdg_get_param(EVDG_SALINFO_ITEM index, int channel, void* buf, int size);
int evdg_get_simstatus(int channel);

#if ((ACOM_BOARD_TYPE == MT673X) || (ACOM_BOARD_SUBTYPE == MDM_01))
int evdg_set_factest(EN_VAD_FACTEST_CFG index, int value);
int evdg_get_factest(EN_VAD_FACTEST_CFG index);
#endif
utl_cfg_h evdg_get_cfghdl(void);

ST_VAD_VER* evdg_get_ver_vdc();
ST_VAD_VER* evdg_get_ver_vdg();
ST_VAD_VER* evdg_get_ver_vds();
ST_VAD_APN_INFO* evdg_get_apn();

#define evdg_pubdata    (evdg_get_pubdata())
#define evdg_dst        (*evdg_get_dst())
#define evdg_dpc        (*evdg_get_dpc())
#define evdg_syserr     (*evdg_get_syserr())
#define evdg_syswarn    (*evdg_get_syswarn())

void evdg_send_update(void);/*for vdc romate_sim online*/
void evdg_online_run_updatetimer(void);
void evdg_remote_to_local(void);
void evdg_local_to_remote(void);


#ifdef __cplusplus
}
#endif

#endif


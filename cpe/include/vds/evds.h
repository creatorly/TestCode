/********************************************************
* 
*
* Ejoin SIM Pool Client
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         evds.h
* PACKAGE:      
*   Header of:          evds.c
*   Date:               2015.09.10
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __EVDS_H__
#define  __EVDS_H__

#include <vds/vds_ver.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    EVDS_PARAM_SERVER,          /* string, the ESP Server address(ip[:port], default port is 20000) */
    EVDS_PARAM_USR,             /* string, the ESP registration username */
    EVDS_PARAM_PWD,             /* string, the ESP registration password */
    EVDS_PARAM_SIM_TYPE,        /* enum(EN_EVDS_SIM_TYPE), the SIM type */    
}EN_EVDS_PARAM;

typedef enum
{
    EVDS_CMD_NONE = 0x80,       /* no event is received */
    EVDS_CMD_OPEN,
    EVDS_CMD_CLOSE,
    EVDS_CMD_GET,
    EVDS_CMD_SET,
}EN_EVDS_CMD;

typedef enum
{
    EVDS_EVENT_NONE = 0,        /* no event is received */
        
    EVDS_EVENT_ONLINE,          /* the EVDS is registered to ESPS */
    EVDS_EVENT_OFFLINE,         /* the EVDS is not registered */

    EVDS_EVENT_OPENED,          /* the EVDS session is opened(SIM is allocated) */
    EVDS_EVENT_CLOSED,          /* the EVDS session is closed(SIM is free) */

    EVDS_EVENT_SIM_IN,          /* SIM is installed */
    EVDS_EVENT_SIM_OUT,         /* SIM is removed */
    EVDS_EVENT_SIM_ONLINE,      /* SIM is registered to operator */
    EVDS_EVENT_SIM_OFFLINE,     /* SIM is not registered */

    /* the call event definition, see the evds_cdr_t for details */
    EVDS_EVENT_CALL_BEGIN,
    EVDS_EVENT_CALL_ALERT,
    EVDS_EVENT_CALL_ANSWER,
    EVDS_EVENT_CALL_END,

    EVDS_EVENT_MAX
}EN_EVDS_EVENT;

enum
{
    EVDS_SIM_TYPE_GSM = 0,
    EVDS_SIM_TYPE_CDMA,
    EVDS_SIM_TYPE_WCDMA,
    EVDS_SIM_TYPE_LTE,          /* FDD-LTE, NOT supported now */
}EN_EVDS_SIM_TYPE;

/* for the event ONLINE/OFFLINE, SIM_ONLINE/SIM_OFFLINE */
typedef struct tagstEvdsStatus
{
    int status;
    char reason[32];
}evds_status_t;

/* for the event ONLINE/OFFLINE, SIM_ONLINE/SIM_OFFLINE */
typedef struct tagstEvdsEventSimIn
{
    int channel;
    
    char iccid[24];
    char imsi[16];
}evds_evt_simin_t;

/* for the event CALL_XXX */
typedef struct tagstEvdsCallDetails
{
    int dir;                    /* call directory, 0: received from GSM, 1: sent to GSM */
    char caller[32];
    char callee[32];

    int ts_begin;               /* timestamp when the call begin */
    int ts_alert;
    int ts_answer;
    int ts_end;
}evds_cdr_t;

typedef struct tagstEvdsEvent
{
    EN_EVDS_EVENT evt;

    union
    {
        evds_status_t st;
        evds_cdr_t cdr;
        evds_evt_simin_t sin;
    }u;
}evds_evt_t;

typedef struct tagstEvdsCmdOpen
{
    char dev_name[32];
    char sid[36];           /* sessison id */
    char vsw_ip[32];        /* server ip address */
    int  vsw_port;          /* server port */
}evds_cmd_open_t;

typedef struct tagstEvdsCmdClose
{
    char sid[36];       /* sessison id */
    int code;
    char reason[32];        
}evds_cmd_close_t;

typedef struct tagstEvdsCmdSet
{
    int channel;

    int plen;           /* the length of pars */
    char pars[512];        
}evds_cmd_set_t;

/* used to send command to evds from other internal process */
typedef struct tagstEvdsCommand
{
    EN_EVDS_CMD cmd;

    union
    {
        evds_cmd_open_t open;
        evds_cmd_close_t close;

        evds_cmd_set_t set;
    }u;
}evds_cmd_t;

/**
 * WRITE the version of vdg to share memory
 */
void evds_ver_out();

/**
 * INITIALIZE the ejoin simpool client
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int evds_init();

/**
 * SET the configuration of the evds
 * @param param:    the param to be set
 * @param index:    the param index for the array configuration
 * @param val:      the pointer to the param value
 * @param len:      the length of param value
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int evds_set(int param, int index, const void* val, int len);

/**
 * SET the event(sent by utl_transaction) receiver
 * @param rem_ip:   the ip address to which the event is sent
 * @param rem_port: the port to which the event is sent
 */
void evds_set_evt_rcvr(const char* rem_ip, int rem_port);

/**
 * START the ejoin simpool client
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int evds_start();

/**
 * RECV a evds event within the specified time
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
int evds_recv(evds_evt_t* evt, int tmo);

/**
 * STOP the ejoin simpool client
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
void evds_stop();

#ifdef __cplusplus
}
#endif

#endif


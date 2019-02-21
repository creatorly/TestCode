/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:                 ut_vad_connect.c
* PACKAGE:      
*   Implement of:       utils vad connection
*   Date:               2018.07.26
*   Author:             lixx
*   Version:            2.0
*
* Change
* 
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <eos.h>
#include <utl_msgq.h>
#include <utl_tpool.h>
#include <utl_socket.h>

#include "acomsys.h"
#include "vdg/evdg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UT_VAD_CONN_OK                  0
#define UT_VAD_CONN_MAX_CHANNEL         4

#define UT_VAD_CONN_SOCKET_REUSED_INTVL     15      /* in Seconds */
#define UT_VAD_CONN_SOCKET_ATREUSED_INTVL   15      /* in Seconds */

#define UT_TLV_PROTO_HEADER_LEN         sizeof(ST_VAD_TLV_HEADER)

typedef enum
{
    UT_VAD_CONN_STATE_IDLE = 0,
    UT_VAD_CONN_STATE_FIN,
    UT_VAD_CONN_STATE_START,
    UT_VAD_CONN_STATE_CONNECT,
    UT_VAD_CONN_STATE_SEND, 
    UT_VAD_CONN_STATE_RECV,
    UT_VAD_CONN_STATE_CLOSE,
}EN_UT_VAD_CONN_STATE;

typedef enum
{
    UT_VAD_CONN_MSG_NONE = 0,

    UT_VAD_CONN_MSG_RECV_OK,    /* socket receive data complete */
    UT_VAD_CONN_MSG_CREATE_ERR,    /* create socket error */
    UT_VAD_CONN_MSG_CONNECT_ERR,     /* socket connect server error */
    UT_VAD_CONN_MSG_SEND_ERR,     /* socket send data error */
    UT_VAD_CONN_MSG_RECV_ERR,     /* socket receive data error */
    UT_VAD_CONN_MSG_ALLOC_ERR,

    UT_VAD_CONN_MSG_MAX,       
}EN_UT_VAD_CONN_MSG_TYPE;

typedef struct tagstUtilsConnectData
{
    int channel_id;
    utl_sock sock;
    EN_UT_VAD_CONN_STATE soc_state;
    EN_UT_VAD_CONN_STATE last_state;
    
    utl_addr_t ser_addr;
    char ser_host[128];
    char ser_ip[32];
    int ser_port;
    EN_VAD_SOCKET_PROTO proto;
    
    int tmo;
    unsigned int last_time;
    
    _BOOL is_used;
    _BOOL reused_soc;
    _BOOL re_connect;
    _BOOL is_shutdown;

    utl_msgq_t* msgq;
    utl_timer_t timer;
    utl_timer_t reuse_timer;    /* the socket can be reused before the timer fired */
}ST_UT_VAD_CONN_DATA;

typedef struct tagstUtilsConnectMinfo
{
    utl_tpool_t* ptpoolm;

    ST_VAD_ATSOCK_APIS ats_apis;

    char last_sip[128];
    int last_sport;
    int at_is_reused;
    time_t at_last_time;
    
    SEM_ID sockSemId;           
}ST_UT_VAD_CONN_MINFO;

typedef struct tagstUtilsVadConn
{
    utl_select sel;
    utl_timers_t* tmrs;
    
    utl_msgq_t* msgq;

    ST_UT_VAD_CONN_DATA conn_data[UT_VAD_CONN_MAX_CHANNEL];
}ST_VAD_CONN;

typedef struct tagstVadConnMessage
{
    ST_UT_VAD_CONN_DATA* psoc_data;
    
    EN_UT_VAD_CONN_MSG_TYPE evt;
    char* send_buf;
    int send_len;
    char *recv_buf;
    int recv_len;
    EN_VAD_TLV_CMD_TYPE ack_type;
}ST_VAD_CONN_MSG;

void UT_VAD_Conn_Set_State(ST_UT_VAD_CONN_DATA *pdata, EN_UT_VAD_CONN_STATE state);
EN_UT_VAD_CONN_STATE UT_VAD_Conn_Get_State(ST_UT_VAD_CONN_DATA *pdata);

int UT_VAD_Conn_TPool_MsgFree(void* msg);
int UT_VAD_Conn_Tmo(ST_UT_VAD_CONN_DATA* p_soc_data, utl_timers_t* timers, utl_timer_t* timer);
int UT_VAD_Conn_ReuseTmo(ST_UT_VAD_CONN_DATA* p_soc_data, utl_timers_t* timers, utl_timer_t* timer);

ST_UT_VAD_CONN_DATA * UT_VAD_ConnGetSocketChennel(PST_VAD_CONN pConn, ST_VAD_SERVER_ADDR* paddr, char send_type);
void utVadConnChnGive(ST_UT_VAD_CONN_DATA* pch);

#ifdef __cplusplus
}
#endif



/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:                 ut_vad_conn.h
* PACKAGE:      
*   Implement of:       utils vad connection
*   Date:               2018.07.26
*   Author:             lixx
*   Version:            2.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UT_VAD_CONN_H__
#define  __UT_VAD_CONN_H__

#include <utl_select.h>
#include <utl_timer.h>
#include <vad_tlvdef.h>
#include <utils/ut_vad_msg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    VAD_SOCKPROTO_TCP = 0,
    VAD_SOCKPROTO_UDP,
    VAD_SOCKPROTO_AT,
    VAD_SOCKPROTO_SIZE
}EN_VAD_SOCKET_PROTO;

typedef struct tagstUtilsServerAddr
{
    utl_addr_t ser_addr;
    char ser_host[128];
    int ser_port;
    EN_VAD_SOCKET_PROTO proto;
}ST_VAD_SERVER_ADDR;

typedef struct tagstUtilsVadConn* PST_VAD_CONN;
typedef struct tagstVadConnMessage* PST_VAD_CONN_MSG;

typedef int (*FP_VAD_ATSOCK_SEND)(const char* dst_ip, int dst_port, const void* data, int len, int reuse, int tmo);
typedef int (*FP_VAD_ATSOCK_RECV)(void* buf, int size, int *pLen, int tmo);
typedef int (*FP_VAD_ATSOCK_CLOSE)(int channel);
typedef void (*FP_VAD_ATSOCK_STAT)(int rx_bytes, int tx_bytes);

typedef struct tagstEsccAtSockApis
{
    FP_VAD_ATSOCK_SEND ats_send;
    FP_VAD_ATSOCK_RECV ats_recv;
    FP_VAD_ATSOCK_CLOSE ats_close;
    FP_VAD_ATSOCK_STAT ats_stat;
}ST_VAD_ATSOCK_APIS;

/******************************************************************************************************
 * Define the APIs of global connections
 */
void UT_VAD_Conn_Init();
void UT_VAD_Conn_Free();
void UT_VAD_Conn_SetAtSockApis(ST_VAD_ATSOCK_APIS* pAtsApis);

/******************************************************************************************************
 * Define the APIs of vad connection
 */
PST_VAD_CONN UT_VAD_ConnAlloc(utl_select sel, utl_timers_t* tmrs);
void UT_VAD_ConnFree(PST_VAD_CONN pcon);

void UT_VAD_ConnReset(PST_VAD_CONN pConn);

/* return the conn id if successfully, -1 otherwise */
int UT_VAD_ConnSend(PST_VAD_CONN pcon, ST_VAD_TLV_HEADER* data, int len, ST_VAD_SERVER_ADDR* paddr, char send_type, int tmo);
int UT_VAD_ConnClose(PST_VAD_CONN pcon, int cid);

PST_VAD_CONN_MSG UT_VAD_ConnRecv(PST_VAD_CONN pcon, int tmo);

/******************************************************************************************************
 * Define the APIs of vad connection message
 */
const char* VAD_ConnMsgGetData(PST_VAD_CONN_MSG pMsg);

int VAD_ConnMsgGetLen(PST_VAD_CONN_MSG pMsg);

/*  0 : successfully, 1:create socket err, 2:connect err, 3: send err, 4:recv err, 5: memory err*/
int VAD_ConnMsgGetCode(PST_VAD_CONN_MSG pMsg);

EN_VAD_TLV_CMD_TYPE VAD_ConnMsgGetAckType(PST_VAD_CONN_MSG pMsg);

void VAD_ConnMsgFree(PST_VAD_CONN_MSG pMsg);
void VAD_Conn_SetSocket_NewConn();


#ifdef __cplusplus
}
#endif

#endif


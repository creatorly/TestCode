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
#include <utl_typdef.h>

#include <vad_mmap.h>
#include <utils/ut_mem.h>
#include <utils/ut_vad_conn.h>
#include "ut_debug.h"
#include "ut_vad_conn_.h"

#ifdef __cplusplus
extern "C" {
#endif

static ST_UT_VAD_CONN_MINFO g_utVadConnInfo = {NULL};

static void utVadConnSetSvrAddr(const char* sip, int sport)
{
    ST_UT_VAD_CONN_MINFO * pconn_minfo = &g_utVadConnInfo;

    utl_buf_strcpy(pconn_minfo->last_sip, sip);
    pconn_minfo->last_sport = sport;
}

static void utVadConnAtSockSetReused(int is_reused, unsigned int last_time)
{
    ST_UT_VAD_CONN_MINFO * pconn_minfo = &g_utVadConnInfo;
    
    pconn_minfo->at_is_reused = is_reused;
    pconn_minfo->at_last_time = last_time;
}

static int utVadConnAtSockGetReusedPar(const char* sip, int sport)
{
    ST_UT_VAD_CONN_MINFO * pconn_minfo = &g_utVadConnInfo;
    int bReuse = pconn_minfo->at_is_reused;

    if (pconn_minfo->at_is_reused)
    {
        if (strcmp(sip, pconn_minfo->last_sip) || sport != pconn_minfo->last_sport)
        {
            bReuse = false;
        }

        if ((pconn_minfo->at_last_time > 0) 
            && (time(NULL) - pconn_minfo->at_last_time > UT_VAD_CONN_SOCKET_ATREUSED_INTVL))
        {
            bReuse = false;
        }

        if (!bReuse)
        {
            utVadConnAtSockSetReused(0, 0);
        }
    }

    return bReuse;
}

static int utVadConnAtSockSend(ST_UT_VAD_CONN_DATA* p_soc_data, const char* data, int len, int tmo)
{
    FP_VAD_ATSOCK_SEND fpAtsSend = g_utVadConnInfo.ats_apis.ats_send;
    
    if (fpAtsSend != NULL)
    {
        int bReuse = utVadConnAtSockGetReusedPar(p_soc_data->ser_host, p_soc_data->ser_port);

        UT_DBG2(p_soc_data->channel_id, 
            "send AT data", "to = %s:%d, len = %d, reuse = %d", 
            p_soc_data->ser_host, p_soc_data->ser_port, len, bReuse);
        
        return fpAtsSend(p_soc_data->ser_host, p_soc_data->ser_port, data, len, bReuse, tmo);
    }
    else
    {
        return -1;
    }
}

static int utVadConnAtSockRecv(void* buf, int size, int tmo)
{
    FP_VAD_ATSOCK_RECV fpAtsRecv = g_utVadConnInfo.ats_apis.ats_recv;
    int len = -1;
    int ret = 0;

    if (fpAtsRecv != NULL)
    {
        ret = fpAtsRecv(buf, size, &len, tmo);
        if(0 != ret)
        {
            len = -1;
        }
    }

    return len;
}

static void utVadConnAtSockClose(int channel)
{
    FP_VAD_ATSOCK_CLOSE fpAtsClose = g_utVadConnInfo.ats_apis.ats_close;
    UT_DBG2(0, "utVadConnAtSockClose", "start\n");

    if (fpAtsClose != NULL)
    {
        UT_DBG2(0, "utVadConnAtSockClose", "fpAtsClose != NULL\n");
        fpAtsClose(channel);
    }
}

static void utVadConnAtSockStat(int rx_bytes, int tx_bytes)
{
    FP_VAD_ATSOCK_STAT fpAtsStat = g_utVadConnInfo.ats_apis.ats_stat;

    if (fpAtsStat != NULL)
    {
        fpAtsStat(rx_bytes, tx_bytes);
    }
}

void UT_VAD_Conn_Set_State(ST_UT_VAD_CONN_DATA *pdata, EN_UT_VAD_CONN_STATE state)
{
    pdata->last_state = pdata->soc_state;
    pdata->soc_state = state;
}

EN_UT_VAD_CONN_STATE UT_VAD_Conn_Get_State(ST_UT_VAD_CONN_DATA *pdata)
{
    return pdata->soc_state;
}

void UT_VAD_Conn_Init()
{
    ST_UT_VAD_CONN_MINFO* pconn_minfo = &g_utVadConnInfo;

    if (NULL == pconn_minfo->ptpoolm)
    {
        memset(pconn_minfo, 0, sizeof(ST_UT_VAD_CONN_MINFO));
        
        pconn_minfo->ptpoolm = utl_tpool_create(1, 1);
        if (NULL == pconn_minfo->ptpoolm)
        {
            return;
        }
        
        utl_tpool_set_stacksize(pconn_minfo->ptpoolm, VAD_TPOOL_MAX_STACK_SIZE);
        utl_tpool_start(pconn_minfo->ptpoolm);
        
        pconn_minfo->sockSemId = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    }
}


void UT_VAD_Conn_Free()
{
    ST_UT_VAD_CONN_MINFO * pconn_minfo = &g_utVadConnInfo;
    
    if(NULL != pconn_minfo->ptpoolm)
    {
        utl_tpool_stop(pconn_minfo->ptpoolm);
        utl_tpool_destroy(pconn_minfo->ptpoolm);
    }
}

void UT_VAD_Conn_SetAtSockApis(ST_VAD_ATSOCK_APIS* pAtsApis)
{
    if (NULL == g_utVadConnInfo.ats_apis.ats_send)
    {
        memcpy(&g_utVadConnInfo.ats_apis, pAtsApis, sizeof(*pAtsApis));
    }
}

int UT_VAD_Conn_TPool_MsgFree(void* msg)
{
    //ST_UT_VAD_CONN_MINFO * pconn_minfo = &g_utVadConnInfo;
    ST_UT_VAD_CONN_DATA *pconn_data = (ST_UT_VAD_CONN_DATA *)msg;
    int ret = UT_VAD_CONN_OK;
    
    UT_VAD_Conn_Set_State(pconn_data, UT_VAD_CONN_STATE_IDLE);
    
    pconn_data->is_used = FALSE;
    return ret;
}


int UT_VAD_Conn_Tmo(ST_UT_VAD_CONN_DATA* p_soc_data, utl_timers_t* timers, utl_timer_t* timer)
{
    UT_DBG2(0, "vad conn timeout", "tmo = %d, so_state = %d", 
        timer->period/1000, p_soc_data->soc_state);
    
    if(p_soc_data->soc_state > UT_VAD_CONN_STATE_FIN)
    {
        if(VAD_SOCKPROTO_AT != p_soc_data->proto)
        {
            if(p_soc_data->sock > 0)
            {
                utl_sock_shutdown(p_soc_data->sock);
            }
        }
        else
        {
            //for V:U3
        }
    }
    
    return 0;
}

int UT_VAD_Conn_ReuseTmo(ST_UT_VAD_CONN_DATA* p_soc_data, utl_timers_t* timers, utl_timer_t* timer)
{
    UT_DBG2(p_soc_data->channel_id, "conn reuse timeout", "duration = %ds, fd = %d, used = %d", 
        timer->period/1000, p_soc_data->sock, p_soc_data->is_used);
    
    if (!p_soc_data->is_used)
    {
        if (p_soc_data->sock > 0)
        {
            UT_DBG2(p_soc_data->channel_id, "conn close", "fd = %d", p_soc_data->sock);

            utl_sock_free(p_soc_data->sock, false);
            p_soc_data->sock = 0;
        }

        utVadConnAtSockSetReused(0, 0);
    }

    return 0;
}

static int UT_VAD_Conn_Socket_Connect(ST_UT_VAD_CONN_DATA* p_soc_data)
{
    ST_UT_VAD_CONN_MINFO * pconn_minfo = &g_utVadConnInfo;
    if(VAD_SOCKPROTO_AT == p_soc_data->proto)
    {
        return UT_VAD_CONN_OK;
    }
    
    if(p_soc_data->sock > 0)
    {
        if(p_soc_data->re_connect)
        {
            UT_DBG2(p_soc_data->channel_id, "network changed, re-connect ...", UT_NOARG);
            
            utl_sock_free(p_soc_data->sock, false);
            p_soc_data->sock = 0;
            p_soc_data->re_connect = FALSE;
        }
        else
        {
            //if (utl_addr_equ(&p_soc_data->ser_addr, &p_soc_data->cookie_addr)) 
            if (p_soc_data->reused_soc) 
            {
                UT_DBG2(p_soc_data->channel_id, "conn reused", "sock = %d", p_soc_data->sock);
                //p_soc_data->reused_soc = TRUE;
                return UT_VAD_CONN_OK;
            }
            else
            {
                UT_DBG2(p_soc_data->channel_id, "server not same, close sock", "sock = %d", p_soc_data->sock);

                utl_sock_free(p_soc_data->sock, false);
                p_soc_data->sock = 0;
            }
        }
    }
    
    if (utl_sock_init(&p_soc_data->sock, UTL_ADDRTYPE_IPV4, UTL_SOCKPROTO_TCP))
    {
        UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_CREATE_ERR, "sock init failed", UT_NOARG);
        return UT_VAD_CONN_MSG_CREATE_ERR;
    }
    //utl_sock_non_block(p_soc_data->sock);
    //utl_addr_init(&p_soc_data->ser_addr, UTL_ADDRTYPE_IPV4, p_soc_data->ip, p_soc_data->port);
    
    UT_DBG2(p_soc_data->channel_id, "connect ...", "sock = %d, to = %s:%d", 
        p_soc_data->sock, p_soc_data->ser_ip, p_soc_data->ser_port);

    unsigned end_tick = tickGet() + 30*sysClkRateGet();
    int retry_after = 0;
    int so_errno = ENETUNREACH;
    
    while ((!p_soc_data->is_shutdown) 
        && (ENETUNREACH == so_errno || ECONNABORTED == so_errno) && (tickGet() < end_tick))
    {
        int ret = -1;
        if (retry_after > 0)
        {
            // taskDelay(EOS_MS2TICK(retry_after));
            ret = semTake(pconn_minfo->sockSemId, EOS_MS2TICK(retry_after));
            retry_after = EOS_MIN(1000, 2*retry_after);
        }
        else
        {
            retry_after = 500;
        }
        
        if (ret == 0 || !utl_sock_connect(p_soc_data->sock, &p_soc_data->ser_addr))
        {
            return UT_VAD_CONN_OK;
        }

        so_errno = utl_sock_connect_errno(p_soc_data->sock);
        
        UT_DBG3(p_soc_data->channel_id, 
            "sock connect failed", "server = %s:%d, error = %d %s", 
            p_soc_data->ser_ip, p_soc_data->ser_port, so_errno, strerror(so_errno));
    } 

    utl_sock_free(p_soc_data->sock, false);
    p_soc_data->sock = 0;
    
    UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_CONNECT_ERR, 
        "sock connect failed finally", "server = %s:%d", p_soc_data->ser_ip, p_soc_data->ser_port);

    return UT_VAD_CONN_MSG_CONNECT_ERR;
}


static int UT_VAD_Conn_Socket_Send(ST_UT_VAD_CONN_DATA* p_soc_data, ST_VAD_CONN_MSG *pEvt)
{
    int len, finish_len = 0;
    int ret = UT_VAD_CONN_OK;

    UT_VAD_Conn_Set_State(p_soc_data, UT_VAD_CONN_STATE_SEND);
    
    if(VAD_SOCKPROTO_AT == p_soc_data->proto)
    {
        int passed_time = time(NULL) - p_soc_data->last_time;
        if(passed_time >= p_soc_data->tmo)
        {
             UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_SEND_ERR, 
                 "sock send failed: TIME_OUT", "server = %s:%d", p_soc_data->ser_host, p_soc_data->ser_port);
            return UT_VAD_CONN_MSG_SEND_ERR;
        }
        
        len = utVadConnAtSockSend(p_soc_data, pEvt->send_buf, pEvt->send_len, (p_soc_data->tmo - passed_time));
        if(len != 0)
        {
            UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_SEND_ERR, 
                "sock send failed", "server = %s:%d", p_soc_data->ser_host, p_soc_data->ser_port);
           return UT_VAD_CONN_MSG_SEND_ERR;
        }
    }
    else
    {
        UT_DBG2(p_soc_data->channel_id, "send sock data", 
            "sock = %d, len = %d", p_soc_data->sock, pEvt->send_len);
        
        do
        {
            len = utl_sock_send(p_soc_data->sock, pEvt->send_buf + finish_len,
                                pEvt->send_len -finish_len, NULL);

            if(len > 0)
            {
                finish_len += len;
            }
            else
            {
                if(p_soc_data->reused_soc && 0 == finish_len)
                {
                    p_soc_data->reused_soc = FALSE;
                    utl_sock_free(p_soc_data->sock, false);
                    p_soc_data->sock = 0;
                    ret = UT_VAD_Conn_Socket_Connect(p_soc_data);
                    if(ret)
                    {
                        UT_WARN(p_soc_data->channel_id, ret, 
                            "sock connect failed", "server = %s:%d", p_soc_data->ser_ip, p_soc_data->ser_port);
                        return ret;
                    }
                }
                else
                {
                    utl_sock_free(p_soc_data->sock, false);
                    p_soc_data->sock = 0;
                    
                    UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_SEND_ERR, 
                        "sock send failed", "server = %s:%d", p_soc_data->ser_ip, p_soc_data->ser_port);
                    return UT_VAD_CONN_MSG_SEND_ERR;
                }
            }
        
        }while(finish_len < pEvt->send_len);
    }
    
    return UT_VAD_CONN_OK;
}



static int UT_VAD_Conn_Socket_Recv(ST_UT_VAD_CONN_DATA* p_soc_data, ST_VAD_CONN_MSG *pEvt)
{
    int  len, finish_len= 0; 
    unsigned char head_buf[UT_TLV_PROTO_HEADER_LEN];
    ST_VAD_TLV_HEADER* pheader;
    int passed_time = 0;

    UT_VAD_Conn_Set_State(p_soc_data, UT_VAD_CONN_STATE_RECV);
    
    UT_DBG2(p_soc_data->channel_id, "recv data ...", 
        "sock = %d", p_soc_data->sock);
        
    do{
        
        if(VAD_SOCKPROTO_AT != p_soc_data->proto)
        {
            len = utl_sock_recv(p_soc_data->sock, (char *)head_buf+finish_len, sizeof(head_buf) - finish_len, NULL);
        }
        else
        {
            passed_time = time(NULL) - p_soc_data->last_time;
            if(passed_time >= p_soc_data->tmo)
            {
                UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_RECV_ERR, 
                    "sock recv failed: TIME_OUT", "server = %s:%d",p_soc_data->ser_ip, p_soc_data->ser_port);
                return UT_VAD_CONN_MSG_RECV_ERR;
            }
            len = utVadConnAtSockRecv(head_buf+finish_len, sizeof(head_buf) - finish_len, (p_soc_data->tmo - passed_time));
        }
        
        if(len > 0)
        {
            finish_len += len;
        }
        else
        {
            if(VAD_SOCKPROTO_AT != p_soc_data->proto)
            {
                utl_sock_free(p_soc_data->sock, false);
                p_soc_data->sock = 0;
            }
            UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_RECV_ERR, 
                "sock recv head failed", "server = %s:%d",p_soc_data->ser_ip, p_soc_data->ser_port);
            return UT_VAD_CONN_MSG_RECV_ERR;
        }

    }while(finish_len < sizeof(head_buf));
    
    pheader = (ST_VAD_TLV_HEADER *)head_buf;
    VAD_TLV_HEADER_Net2Host(pheader);
    
    pEvt->recv_len = pheader->len + sizeof(head_buf);
    
    pEvt->recv_buf =(char *) eos_malloc(pEvt->recv_len);
    if(NULL == pEvt->recv_buf)
    {
        UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_ALLOC_ERR, 
            "sock malloc failed", "server = %s:%d",p_soc_data->ser_ip, p_soc_data->ser_port);
        return UT_VAD_CONN_MSG_ALLOC_ERR;
    }
    memcpy(pEvt->recv_buf, head_buf, sizeof(head_buf));

    do
    {
        if(VAD_SOCKPROTO_AT != p_soc_data->proto)
        {
            len = utl_sock_recv(p_soc_data->sock, pEvt->recv_buf + finish_len, 
                pEvt->recv_len - finish_len, &p_soc_data->ser_addr);
        }
        else
        {
            passed_time = time(NULL) - p_soc_data->last_time;
            if(passed_time >= p_soc_data->tmo)
            {
                UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_RECV_ERR, 
                    "sock recv failed: TIME_OUT", "server = %s:%d",p_soc_data->ser_ip, p_soc_data->ser_port);
                return UT_VAD_CONN_MSG_RECV_ERR;
            }
            len = utVadConnAtSockRecv(pEvt->recv_buf + finish_len, pEvt->recv_len - finish_len, (p_soc_data->tmo - passed_time));
        }
        
        if(len > 0)
        {
            finish_len += len;
        }
        else
        {
            if(VAD_SOCKPROTO_AT != p_soc_data->proto)
            {
                utl_sock_free(p_soc_data->sock, false);
                p_soc_data->sock = 0;
            }
            
            UT_WARN(p_soc_data->channel_id, UT_VAD_CONN_MSG_RECV_ERR, 
                "sock recv data failed", "server = %s:%d", p_soc_data->ser_ip, p_soc_data->ser_port);
            return UT_VAD_CONN_MSG_RECV_ERR;
        }
    }while(finish_len < pEvt->recv_len);

    UT_DBG2(p_soc_data->channel_id, "recv data OK", 
        "sock = %d, len = %d", p_soc_data->sock, finish_len);

    return UT_VAD_CONN_OK;
}

static int UT_VAD_CONN_Socket_Main(void* msg, void* data)
{
    ST_UT_VAD_CONN_DATA* p_soc_data = (ST_UT_VAD_CONN_DATA*)msg;
    ST_VAD_CONN_MSG *pEvt = (ST_VAD_CONN_MSG*)data;
    EN_UT_VAD_CONN_MSG_TYPE ret = UT_VAD_CONN_MSG_NONE;
    int recvfail_count = 0;
    
    do
    {
        ret = UT_VAD_Conn_Socket_Connect(p_soc_data);
        if(ret)
        {
            pEvt->evt = ret;
            break;
        }
        
        ret = UT_VAD_Conn_Socket_Send(p_soc_data, pEvt);
        if(ret)
        {
            pEvt->evt = ret;
            break;
        }
        
        do
        {
            ret = UT_VAD_Conn_Socket_Recv(p_soc_data, pEvt);
            if(UT_VAD_CONN_OK != ret)
            {
                break;
            }
            
            ST_VAD_TLV_HEADER* pheader = (ST_VAD_TLV_HEADER*)pEvt->recv_buf;
            if(pheader->type == (pEvt->ack_type | 0x80))
            {
                break;
            }
            eos_free(pEvt->recv_buf);
            pEvt->recv_buf = NULL;
            pEvt->recv_len = 0;
            recvfail_count++;
            
            ret = UT_VAD_CONN_MSG_RECV_ERR;
            
        }while(recvfail_count > 0 && recvfail_count < 5);

        if(ret)
        {
            pEvt->evt = ret;
            break;
        }
        
        pEvt->evt = UT_VAD_CONN_MSG_RECV_OK;

        UT_VAD_Conn_Set_State(p_soc_data, UT_VAD_CONN_STATE_FIN);
    }while(0);

    if (VAD_SOCKPROTO_AT == p_soc_data->proto)
    {
        if (UT_VAD_CONN_MSG_RECV_OK == pEvt->evt)
        {
            utVadConnAtSockStat(pEvt->recv_len, pEvt->send_len);
            utVadConnSetSvrAddr(p_soc_data->ser_host, p_soc_data->ser_port);
            utVadConnAtSockSetReused(1, time(NULL));
        }
        else
        {
            utVadConnAtSockSetReused(0, 0);
        }
    }
    else
    {
        utVadConnSetSvrAddr(p_soc_data->ser_ip, p_soc_data->ser_port);
    
        p_soc_data->reused_soc = FALSE;
        p_soc_data->last_time = time(NULL);
    }
    
    utl_msgq_send(p_soc_data->msgq, pEvt);

    return 0;
}

ST_UT_VAD_CONN_DATA * UT_VAD_ConnGetSocketChennel(PST_VAD_CONN pConn, ST_VAD_SERVER_ADDR* paddr, char send_type)
{
    ST_UT_VAD_CONN_DATA *p_g_conn_data = &pConn->conn_data[0];
    ST_UT_VAD_CONN_DATA *psoc_data = NULL;
    
    int i;

    if(VAD_SOCKPROTO_AT == paddr->proto)
    {
        if(VAD_TLV_CMD_SIMIO == send_type || VAD_TLV_CMD_OPEN == send_type || VAD_TLV_CMD_CLOSE == send_type)
        {
            for(i = 0; i < UT_VAD_CONN_MAX_CHANNEL; i++)
            {
                if(p_g_conn_data[i].is_used)
                {
                    UT_DBG2(0, "UT_VAD_ConnGetSocketChennel", "channel:%d is_used:%d", i, p_g_conn_data[i].is_used);
                    if(VAD_SOCKPROTO_AT == p_g_conn_data[i].proto)
                    {
                        if(UT_VAD_CONN_STATE_SEND == p_g_conn_data[i].soc_state)
                        {
                            UT_DBG2(0, "UT_VAD_ConnGetSocketChennel", "channel:%d AT send running,close", i);
                            utVadConnAtSockClose(0);
                        }
                        else if(UT_VAD_CONN_STATE_RECV == p_g_conn_data[i].soc_state)
                        {
                            UT_DBG2(0, "UT_VAD_ConnGetSocketChennel", "channel:%d AT recv running,close", i);
                            utVadConnAtSockClose(1);
                        }
                    }
                    else
                    {
                        UT_DBG2(0, "UT_VAD_ConnGetSocketChennel", "channel:%d is_used:tcp", i);
                        if(p_g_conn_data[i].sock > 0)
                        {
                            utl_sock_shutdown(p_g_conn_data[i].sock);
                        }
                    }
                }
            }
        }
    }
    else
    {
        if(VAD_TLV_CMD_SIMIO == send_type || VAD_TLV_CMD_OPEN == send_type || VAD_TLV_CMD_CLOSE == send_type)
        {
            for(i = 0; i < UT_VAD_CONN_MAX_CHANNEL; i++)
            {
                if(p_g_conn_data[i].is_used && p_g_conn_data[i].sock > 0)
                {
                    utVadConnChnGive(&p_g_conn_data[i]);
                }
            }
        }
        

        for( i = 0; i < UT_VAD_CONN_MAX_CHANNEL; i++)
        {
                if(utl_addr_equ(&paddr->ser_addr, &p_g_conn_data[i].ser_addr))
            {
                if(!p_g_conn_data[i].is_used && p_g_conn_data[i].sock > 0)
                {
                    p_g_conn_data[i].is_used = TRUE;
                    psoc_data = &p_g_conn_data[i];
                    psoc_data->reused_soc = TRUE;
                    UT_DBG2(0, "UT_VAD_ConnGetSocketChennel", "channel:%d same addr and can be used", i);
                    break;
                }
            }
        }
    }

    if(NULL == psoc_data)
    {
        for( i = 0; i < UT_VAD_CONN_MAX_CHANNEL; i++)
        {
            if(!p_g_conn_data[i].is_used)
            {
                p_g_conn_data[i].is_used = TRUE;
                psoc_data = &p_g_conn_data[i];
                
                UT_DBG2(0, "UT_VAD_ConnGetSocketChennel", "get idle channel:%d", i);
                break;
            }
        }
    }

    if(NULL != psoc_data && VAD_SOCKPROTO_AT == paddr->proto)
    {
        if(p_g_conn_data[i].sock > 0)
        {
                UT_DBG2(0, "UT_VAD_ConnGetSocketChennel", "channel:%d same addr and can be used", i);
            utl_sock_free(p_g_conn_data[i].sock, false);
            p_g_conn_data[i].sock = 0;
        }
    }

    return psoc_data;

}

PST_VAD_CONN UT_VAD_ConnAlloc(utl_select sel, utl_timers_t* tmrs)
{
    PST_VAD_CONN pConn;
    int i;

    pConn = UT_TALLOC(ST_VAD_CONN);
    if (NULL == pConn)
    {
        UT_ERROR(EOS_ERROR, "out of memory for type %s", "ST_VAD_CONN");
        return NULL;
    }

    memset(pConn, 0, sizeof(*pConn));

    pConn->sel = sel;
    pConn->tmrs = tmrs;
    
    pConn->msgq = utl_msgq_create(8);
    if(NULL == pConn->msgq)
    {
        UT_ERROR(EOS_ERROR, "create msgq failed for %s", "ST_VAD_CONN");
        UT_FREE(pConn);
        
        return NULL;
    }

    if (sel != NULL)
    {
        utl_msgq_set_hook_notify(pConn->msgq, (utl_msgq_hook_notify_t) utl_select_stop, sel);
    }
    
    for(i = 0; i < UT_VAD_CONN_MAX_CHANNEL; i++)
    {
        ST_UT_VAD_CONN_DATA* pConnData = &pConn->conn_data[i];
        
        pConnData->channel_id = i;

        utl_timer_init(&pConnData->timer, 30 * 1000,
            UTL_TIMERMODE_ONESHOT, (utl_timer_func) UT_VAD_Conn_Tmo, pConnData);

        utl_timer_init(&pConnData->reuse_timer, UT_VAD_CONN_SOCKET_REUSED_INTVL * 1000,
            UTL_TIMERMODE_ONESHOT, (utl_timer_func) UT_VAD_Conn_ReuseTmo, pConnData);
    }

    return pConn;
}

void UT_VAD_ConnFree(PST_VAD_CONN pConn)
{
    int i;

    for (i = 0; i < UT_VAD_CONN_MAX_CHANNEL; i++)
    {
        ST_UT_VAD_CONN_DATA* pch = &pConn->conn_data[i];

        utl_timer_stop(pConn->tmrs, &pch->timer);

        if (pch->sock > 0)
        {
            utVadConnChnGive(pch);
            
            utl_sock_free(pch->sock, false);
        }
    }

    if (pConn->msgq != NULL)
    {
        utl_msgq_free(pConn->msgq);
    }

    UT_FREE(pConn);
}

int UT_VAD_ConnSend(PST_VAD_CONN pConn, ST_VAD_TLV_HEADER* data, int len, ST_VAD_SERVER_ADDR* paddr, char send_type, int tmo)
{
    ST_UT_VAD_CONN_DATA *psoc_data = NULL;
    ST_UT_VAD_CONN_MINFO * pconn_minfo = &g_utVadConnInfo;
    
    ST_VAD_CONN_MSG *pmsg = NULL;

#if 1    
    char print_buf[2048];
    UT_VAD_MsgToStr(data, print_buf,sizeof(print_buf));
    UT_LDBG1(data->sid, "vad conn send message", 
        "to = %s:%d, data = \n\n%s\n\n", paddr->ser_host, paddr->ser_port, print_buf);
#endif    
    
    if(NULL == data || len < UT_TLV_PROTO_HEADER_LEN || NULL == paddr )
    {
        UT_DBG2(0, "UT_VAD_ConnSend", "data error");
        return -1;
    }
    
    VAD_TLV_HEADER_Host2Net(data);

    psoc_data = UT_VAD_ConnGetSocketChennel(pConn, paddr, send_type);

    if (NULL == psoc_data)
    {
        UT_DBG2(0, "UT_VAD_ConnSend", "channel is busy");
        return -1;
    }
    
    pmsg =(ST_VAD_CONN_MSG *) eos_malloc(sizeof(ST_VAD_CONN_MSG));
    if(NULL == pmsg)
    {
        psoc_data->is_used = FALSE;
        UT_WARN(psoc_data->channel_id, UT_VAD_CONN_MSG_ALLOC_ERR, 
            "msg malloc failed", "server = %s:%d",psoc_data->ser_ip, psoc_data->ser_port);
        return -1;
    }
    
    memset(pmsg, 0, sizeof(ST_VAD_CONN_MSG));

    pmsg->psoc_data = psoc_data;
    pmsg->ack_type = send_type;
    
    pmsg->send_buf =(char *) eos_malloc(len + 1);
    if(NULL == pmsg->send_buf)
    {
        psoc_data->is_used = FALSE;
        eos_free(pmsg);
        UT_WARN(psoc_data->channel_id, UT_VAD_CONN_MSG_ALLOC_ERR, 
            "send malloc failed", "server = %s:%d",psoc_data->ser_ip, psoc_data->ser_port);
        return -1;
    }
    memcpy(pmsg->send_buf, data, len);
    pmsg->send_buf[len] = 0;
    pmsg->send_len = len;
    
    if(VAD_SOCKPROTO_AT != paddr->proto)
    {
        utl_addr_copy(&psoc_data->ser_addr, &paddr->ser_addr);
        utl_addr_get_ip(&paddr->ser_addr, psoc_data->ser_ip);
        psoc_data->ser_port = utl_addr_get_port(&paddr->ser_addr);
        
        utl_timer_reset(pConn->tmrs, &psoc_data->reuse_timer, UT_VAD_CONN_SOCKET_REUSED_INTVL*1000);
    }
    else
    {
        psoc_data->ser_port = paddr->ser_port;
        memset(psoc_data->ser_host, 0, sizeof(psoc_data->ser_host));
        strncpy(psoc_data->ser_host, paddr->ser_host, strlen(paddr->ser_host));
        psoc_data->tmo = tmo;
        psoc_data->last_time = time(NULL);

        if(VAD_TLV_CMD_OPEN == pmsg->ack_type)
        {
            utVadConnAtSockSetReused(0, 0);
        }

        utl_timer_reset(pConn->tmrs, &psoc_data->reuse_timer, UT_VAD_CONN_SOCKET_ATREUSED_INTVL*1000);
    }

    psoc_data->last_time = time(NULL);
    
    psoc_data->proto = paddr->proto;
    psoc_data->msgq = pConn->msgq;
    psoc_data->is_shutdown = false;
    
    UT_VAD_Conn_Set_State(psoc_data, UT_VAD_CONN_STATE_START);
    
    if(tmo > 0 && NULL != pConn->tmrs)
    {
        utl_timer_reset(pConn->tmrs, &psoc_data->timer, tmo * 1000);
    }

    utl_tpool_add(pconn_minfo->ptpoolm, psoc_data, pmsg, UT_VAD_CONN_Socket_Main, UT_VAD_Conn_TPool_MsgFree);
    
    return psoc_data->channel_id;
}

int UT_VAD_ConnClose(PST_VAD_CONN pConn, int cid)
{
    if(pConn->conn_data[cid].sock > 0)
    {
        //if(UT_VAD_Conn_Get_State() > UT_VAD_CONN_STATE_FIN)
        {
            pConn->conn_data[cid].is_shutdown = true;
            utl_sock_shutdown(pConn->conn_data[cid].sock);
        }
    }
    
    return 0;
}

PST_VAD_CONN_MSG UT_VAD_ConnRecv(PST_VAD_CONN pConn, int tmo)
{    
    ST_VAD_CONN_MSG* pmsg = utl_msgq_recv_ms(pConn->msgq, tmo);

    if (pmsg != NULL)
    {
        /* reuse the socket */
        if (UT_VAD_CONN_MSG_RECV_OK == pmsg->evt)
        {
            ST_UT_VAD_CONN_DATA *psoc_data =  pmsg->psoc_data;
        
            /* not change the length of timer, just reset it */
            utl_timer_reset(pConn->tmrs, &psoc_data->reuse_timer, -1);
            psoc_data->last_time = time(NULL);

            ST_VAD_TLV_HEADER* mhdr = (ST_VAD_TLV_HEADER*)pmsg->recv_buf;
            char print_buf[2048];
            
            UT_VAD_MsgToStr(mhdr, print_buf, sizeof(print_buf));
            
            UT_LDBG1(mhdr->sid, "vad conn received message", "from = %s:%d, data = \n\n%s\n\n", 
                psoc_data->ser_ip[0] ? psoc_data->ser_ip : psoc_data->ser_host, 
                psoc_data->ser_port, print_buf);
        }
    }

    return pmsg;
}

void UT_VAD_ConnReset(PST_VAD_CONN pConn)
{
    int i;
    
    UT_DBG2(0, "utils connect reset", "conn = %p", pConn);

    for (i = 0; i< UT_VAD_CONN_MAX_CHANNEL; i++)
    {
        if(pConn->conn_data[i].sock > 0)
        {
            pConn->conn_data[i].re_connect = 1;
        }
    }
}

void utVadConnChnGive(ST_UT_VAD_CONN_DATA* pch)
{
    if (pch->sock > 0)
    {
        utl_sock_shutdown(pch->sock);
        semGive(g_utVadConnInfo.sockSemId);

        while (pch->soc_state != UT_VAD_CONN_STATE_IDLE)
        {
            taskDelay(1);
        }
    }
}

const char* VAD_ConnMsgGetData(ST_VAD_CONN_MSG* pMsg)
{
    if(NULL != pMsg)

    {
        return pMsg->recv_buf;
    }
    else
    {
        return NULL;
    }

}

int VAD_ConnMsgGetLen(ST_VAD_CONN_MSG* pMsg)
{
    if(NULL != pMsg)

    {
        return pMsg->recv_len;
    }
    else
    {
        return 0;
    }
}

void VAD_ConnMsgFree(ST_VAD_CONN_MSG* pMsg)
{
    if(NULL == pMsg)
    {
        return ;
    }

    
    if(NULL != pMsg->send_buf)
    {
        eos_free(pMsg->send_buf);
        pMsg->send_buf = NULL;
    }

    if(NULL != pMsg->recv_buf)
    {
        eos_free(pMsg->recv_buf);
        pMsg->recv_buf = NULL;
    }
    eos_free(pMsg);
}

int VAD_ConnMsgGetCode(ST_VAD_CONN_MSG* pMsg)
{
    if(NULL != pMsg)

    {
        return (pMsg->evt - 1);
    }
    else
    {
        return - 1;
    }
}

EN_VAD_TLV_CMD_TYPE VAD_ConnMsgGetAckType(ST_VAD_CONN_MSG* pMsg)
{
    
    if(NULL != pMsg)
    {
        return pMsg->ack_type;
    }
    else
    {
        return VAD_TLV_CMD_NONE;
    }
}

#ifdef __cplusplus
}
#endif



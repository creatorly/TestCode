/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         cwa_sock.c
* PACKAGE:      
*   Implement of:       cwa_sock daemon
*   Date:               2018.09.14
*   Author:             XieKunHong
*   Version:            1.0
*
* Change
*********************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include <eos.h>
#include <utl_cfg.h>
#include <utl_select.h>

#include <utl_msgq.h>
#include <utl_socket.h>

#include "cwa_sock.h"
#include "sys_debug.h"


typedef enum
{
    CWA_SOCK_MSG_NONE = 0,

    CWA_SOCK_MSG_ACCPET,    /* socket receive data complete */
    CWA_SOCK_MSG_SENDACK,    /* socket receive data complete */
    CWA_SOCK_MSG_SENDINFO,    /* create socket error */
    CWA_SOCK_MSG_CONNECT_ERR,     /* socket connect server error */
    CWA_SOCK_MSG_SEND_ERR,     /* socket send data error */
    CWA_SOCK_MSG_RECV_ERR,     /* socket receive data error */
    CWA_SOCK_MSG_ALLOC_ERR,

    CWA_SOCK_MAX,       
}EN_CWA_SOCK_MSG_TYPE;

typedef struct tagstCwa_Sock_Minfo
{
    utl_sock listen_soc;
    utl_addr_t loc_addr;
    utl_select recv_se;
    char recv_buf[4096];
    char recv_size;
}ST_CWA_SOCK_MINFO;

static ST_CWA_SOCK_MINFO *g_cwaSock = NULL;
static CWA_SOCK_HANDLER_RECV pCwaSockHandlerRecvFuction;

ST_CWA_SOCK_MINFO *Cwa_Sock_Create(const char *uisock_url)
{
    ST_CWA_SOCK_MINFO *pcwasock_minfo = NULL;
    pcwasock_minfo =(ST_CWA_SOCK_MINFO *)eos_malloc(sizeof(ST_CWA_SOCK_MINFO));
    if (NULL == pcwasock_minfo)
    {
        SYS_ERROR(EOS_ERROR, "cwaSock_Init malloc failed");
        return NULL;
    }
    memset(pcwasock_minfo, 0, sizeof(ST_CWA_SOCK_MINFO));

    if (utl_sock_init(&pcwasock_minfo->listen_soc, UTL_ADDRTYPE_IPV4, UTL_SOCKPROTO_TCP))
    {
        eos_free(pcwasock_minfo);
        
        SYS_ERROR(EOS_ERROR, "cwaSock init failed");
        return NULL;
    }
    utl_sock_reuse(pcwasock_minfo->listen_soc, true);

    memset(&pcwasock_minfo->loc_addr, 0, sizeof(utl_addr_t));
    utl_addr_init(&pcwasock_minfo->loc_addr, UTL_ADDRTYPE_IPV4, uisock_url, HOOKSOCKPORT);
    utl_addr_set_proto(&pcwasock_minfo->loc_addr, UTL_SOCKPROTO_TCP);

    utl_sock_non_block(pcwasock_minfo->listen_soc);

    utl_select_init(&pcwasock_minfo->recv_se, NULL, NULL);

    int bindRet = utl_sock_bind(pcwasock_minfo->listen_soc, &pcwasock_minfo->loc_addr);
    if (bindRet < 0)
    {
        SYS_ERROR(EOS_ERROR, "Cwa_Sock_Create bind error, bindRet = %d", bindRet);
        return NULL;
    }

    SYS_INFO("cwaSock_Create, sock_url = %s\n", uisock_url);

    return pcwasock_minfo;
}

void Cwa_Sock_Free(ST_CWA_SOCK_MINFO * pcwasock_minfo)
{
    if(NULL == pcwasock_minfo)
    {
        return;
    }
    
    utl_sock_free(pcwasock_minfo->listen_soc, false);
    pcwasock_minfo->listen_soc = 0;
    utl_addr_free(&pcwasock_minfo->loc_addr);
    
    eos_free(pcwasock_minfo);
    pcwasock_minfo = NULL;
}


void CWA_SOCKHookRecvData(CWA_SOCK_HANDLER_RECV recvHandler)
{
    pCwaSockHandlerRecvFuction = recvHandler;
}

void cwaSockOnRead(utl_select sel, utl_sock sock, utl_soevt_e evt, void* data)
{
    char req[2048];
    int len;

    len = utl_sock_recv(sock, req, sizeof(req), NULL);
    if (len > 0)
    {
        SYS_INFO("cwaSockAcceptRun, sock = %d, req = %s", sock, req);

        if (g_cwaSock->recv_size + len < sizeof(g_cwaSock->recv_buf)) {
            strncpy(g_cwaSock->recv_buf + g_cwaSock->recv_size, req, len);
            g_cwaSock->recv_size = strlen(g_cwaSock->recv_buf);
        }
        SYS_INFO("cwaSockAcceptRun, sock = %d, g_cwaSock->recv_buf = %s, req = %s", sock, g_cwaSock->recv_buf, req);

        if (strstr(req, "\r\n")) {

            pCwaSockHandlerRecvFuction(g_cwaSock->recv_buf, g_cwaSock->recv_size);

            const char *notifyAckStr = "{\"msg\": \"NOTIFY-ACK\", \"code\":0, \"desc\":\"OK\"}\r\n";
            // utl_sock_send(g_cwaSock->listen_soc, notifyAckStr, sizeof(notifyAckStr), NULL);
            int sendLen = strlen(notifyAckStr);
            int finish_len = 0, sendedLen = 0;
            do
            {
                sendedLen = utl_sock_send(g_cwaSock->listen_soc, notifyAckStr + finish_len, sendLen -finish_len, NULL);

                if(sendedLen > 0) {
                    finish_len += sendedLen;
                } else {
                    SYS_WARN(0, "sock send failed, g_cwaSock->listen_soc = %d", g_cwaSock->listen_soc);
                    break;
                }
            }while(finish_len < sendLen);

            memset(g_cwaSock->recv_buf, 0, sizeof(g_cwaSock->recv_buf));
            g_cwaSock->recv_size = 0;

        }
    }
}

void cwaSockOnClose(utl_select sel, utl_sock sock, utl_soevt_e evt, void* data)
{
    utl_select_del(sel, sock);
}

void cwaSockOnAccept(utl_select sel, utl_sock sock, utl_soevt_e evt, void* data)
{
    utl_sock sacc;

    if (!utl_sock_accept(sock, &sacc, NULL))
    {
        /* utl_sock_non_block(sock); */
        
        utl_select_add(sel, sacc, UTL_SOEVT_READ, cwaSockOnRead, sel);
        utl_select_add(sel, sacc, UTL_SOEVT_CLOSE, cwaSockOnClose, sel);
    }
}

void cwaSockAcceptRun(void* data)
{
    ST_CWA_SOCK_MINFO *pcwasock_minfo = (ST_CWA_SOCK_MINFO *) data;
    
    int listenRet = utl_sock_listen(pcwasock_minfo->listen_soc, 8);
    if (listenRet < 0) 
    {
        SYS_WARN(0, "Cwa_Sock_Create listen error, listenRet = %d", listenRet);
        return ;
    }

    utl_select_add(pcwasock_minfo->recv_se, pcwasock_minfo->listen_soc, UTL_SOEVT_ACCEPT, cwaSockOnAccept, pcwasock_minfo->recv_se);
    while(1){
        utl_select_run(pcwasock_minfo->recv_se, 1000);
    }

}

int cwaSock_Start(const char *uisock_url)
{
    
    SYS_INFO("cwaSock_Start, uisock_url = %s", uisock_url);
    g_cwaSock = Cwa_Sock_Create(uisock_url);
    if(NULL == g_cwaSock)
    {
        SYS_WARN(0, "cwa_sock_Create receive failed");
        return -1;
    }
    eos_task_start("cwasock_accept", 150, 32*1024, cwaSockAcceptRun, g_cwaSock);

    return 0;
}

int cwaSock_SendData(char * data, int len, char *resultData, int retBufSize)
{
   utl_sock sendSocket = (utl_sock)taskVarGet("cwaSockSend");
   if (sendSocket == 0) {
        if (utl_sock_init(&sendSocket, UTL_ADDRTYPE_IPV4, UTL_SOCKPROTO_TCP))
        {
            SYS_WARN(0, "cwaSock_SendData create failed");
            return -1;
        }
        taskVarSet("cwaSockSend", (void *)sendSocket);
   }

    if (utl_sock_get_state(sendSocket) == UTL_SOSTATE_CLOSED) {
        if (utl_sock_connect(sendSocket, &g_cwaSock->loc_addr)) {
            utl_sock_free(sendSocket, false);
            SYS_WARN(0, "cwaSock_SendData connect failed");
            return -1;
        }
    }

    int finish_len = 0, sendedLen = 0;
    do
    {
        sendedLen = utl_sock_send(sendSocket, data + finish_len, len -finish_len, NULL);

        if(sendedLen > 0)
        {
            finish_len += sendedLen;
        }
        else
        {
            utl_sock_free(sendSocket, false);
            SYS_WARN(0, "sock send failed, len = %d, sendedLen = %d", len, sendedLen);
            return -1;
        }
    }while(finish_len < len);

    int recvLen = 0;
    finish_len = 0;
    do
    {
        recvLen = utl_sock_recv(sendSocket, resultData+finish_len, retBufSize-finish_len, NULL);
        if (recvLen < 1) {
            SYS_WARN(sendSocket, "cwaSock_SendData recv error, sendSocket = %d, recvLen = %d", sendSocket, recvLen);
            return -1;
        }
        finish_len += recvLen;
    }while(utl_sock_bytes(sendSocket) > 0);

    SYS_INFO("cwaSock_SendData recv data, sendSocket = %d, resultData = %s", sendSocket, resultData);

    return 0;
}


#ifdef __cplusplus 
}
#endif
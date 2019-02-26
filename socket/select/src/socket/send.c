/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin XP Development Team.
*   Communication with Android ufi
* 
* File:         cwa_ufi.c
* PACKAGE:      
*   Implement of:       cwa_ufi daemon
*   Date:               2018.09.17
*   Author:             XieKunHong
*   Version:            1.0
*
* Change
*********************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <eos.h>
#include <utl_json.h>
#include <utl_token.h>

#include "cwa_sock.h"
#include "sys_debug.h"

static int cwaUfiSendSetDevDataConfigReq(char *dataBuf, int dataLen)
{
    char recvBuf[1024];
    int ret = 0;
    memset(recvBuf, 0, sizeof(recvBuf));
    ret = cwaSock_SendData(dataBuf, dataLen, recvBuf, sizeof(recvBuf));
    if (ret != 0) {
        return ret;
    }

    int resultCode = 0;
    resultCode = utl_json_get_int(recvBuf, "code");

    return resultCode;
}

static int CWA_UFISetSockPath(char *sockPath)
{
    char sendBuf[1024];
    int sendLen = 0;
    sendLen = snprintf(sendBuf, sizeof(sendBuf), "{\"msg\": \"SET\", \"par\":%d, \"data\":\"%s\"}\r\n", 100, sockPath);

    return cwaUfiSendSetDevDataConfigReq(sendBuf, sendLen);
}

// get base request
static int cwaUfiSendGetDevStringInfoReq(int index, char *value, int size)
{
    int ret = 0, sendLen = 0;
    char sendBuf[1024], recvBuf[1024];
    sendLen = snprintf(sendBuf, sizeof(sendBuf), "{\"msg\": \"GET\", \"par\":%d}\r\n", index);
    memset(recvBuf, 0, sizeof(recvBuf));
    ret = cwaSock_SendData(sendBuf, sendLen, recvBuf, sizeof(recvBuf));
    if (ret != 0) {
        return ret;
    }

    int resultCode = 0;
    resultCode = utl_json_get_int(recvBuf, "code");
    utl_json_get_str(recvBuf, "data", value, size);
    return resultCode;
}

static int cwaUfiSendGetDevIntInfoReq(int index, int *value)
{
    int ret = 0, sendLen = 0;
    char sendBuf[1024], recvBuf[1024];
    sendLen = snprintf(sendBuf, sizeof(sendBuf), "{\"msg\": \"GET\", \"par\":%d}\r\n", index);
    memset(recvBuf, 0, sizeof(recvBuf));
    ret = cwaSock_SendData(sendBuf, sendLen, recvBuf, sizeof(recvBuf));
    if (ret != 0) {
        return ret;
    }

    int resultCode = 0;
    resultCode = utl_json_get_int(recvBuf, "code");
    *value = utl_json_get_int(recvBuf, "data");

    return resultCode;
}

_UINT32 Send_Start()
{
    char hookSockPath[128];
    snprintf(hookSockPath, sizeof(hookSockPath), "127.0.0.1:%d", HOOKSOCKPORT);
    SYS_INFO("Send_Start:%s\n",hookSockPath);
    CWA_UFISetSockPath(hookSockPath);

    return EOS_OK;
}

_VOID Send_Delete()
{
    SYS_INFO("send delete\r\n");
}

#ifdef __cplusplus 
}
#endif

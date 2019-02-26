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

_UINT32 Recv_Start()
{
    SYS_INFO("Recv_Start\n");
    cwaSock_Start("127.0.0.1");

    return EOS_OK;
}

_VOID Recv_Delete()
{
    SYS_INFO("recv delete\r\n");
}

#ifdef __cplusplus 
}
#endif

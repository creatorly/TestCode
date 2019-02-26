/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin XP Development Team.
*   Communication with Android socket
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
extern "C" {
#endif

#define HOOKSOCKPORT 10020

typedef void (*CWA_SOCK_HANDLER_RECV)(void* buf, int size);

int cwaSock_Start(const char *uisock_url);
void CWA_SOCKHookRecvData(CWA_SOCK_HANDLER_RECV recvHandler);
 
int cwaSock_SendData(char * data, int len, char *resultData, int retBufSize);

#ifdef __cplusplus 
}
#endif
/********************************************************
* 
*
* Ejoin SecureTalk platform
* Copyright (C) 2013-2013, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_pudp.h
* PACKAGE:      
*   Header of:          utl_pudp.c
*   Date:               2013.11.13
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_PUDP_H__
#define  __UTL_PUDP_H__

#ifdef __cplusplus
extern "C" {
#endif

int utl_pudp_init(int maxSocks, int recvBuf);

int utl_pudp_sock(int port);
int utl_pudp_close(int fd);

int utl_pudp_send(int fd, const void* pkt, int len, int to_port);
int utl_pudp_recv(int fd, char* buf, int size, int* from_port);

int utl_pudp_bytes(int fd);

#ifdef __cplusplus
}
#endif

#endif


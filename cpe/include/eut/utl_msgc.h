/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2012-2012, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_msgc.h
* PACKAGE:      libeut
*   Header of:          utl_msgc.c
*   Date:               2012.09.12
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_MSGC_H__
#define  __UTL_MSGC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* now, only support udp transport */
typedef enum
{
    UTL_MSGC_TRANSPORT_UDP = 0,
    UTL_MSGC_TRANSPORT_TCP,
    UTL_MSGC_TRANSPORT_UDPTR,
}EN_UTL_MSGC_TRANSPORT;

typedef struct tagstUtlMessageClient utl_msgc_t;

utl_msgc_t* utl_msgc_create(const char* loc_ip, int loc_port);
void utl_msgc_destroy(utl_msgc_t* msgc);

/* set default remote address */
int utl_msgc_set_remote(utl_msgc_t* msgc, const char* host, int port);
int utl_msgc_add(utl_msgc_t* msgc, int type, 
        const char* host, int port, int transport);

int utl_msgc_send(utl_msgc_t* msgc, int type, const void* msg, int len);

#ifdef __cplusplus
}
#endif

#endif


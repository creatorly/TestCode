/********************************************************
* 
*
* Ejoin SIP Proxy Lib
* Copyright (C) 2013-2013, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_udptr.h
* PACKAGE:      libeut
*   Header of:          utl_udptr.c
*   Date:               2013.03.31
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_UDPTR_H__
#define __UTL_UDPTR_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlUdpTransMgr utl_udptrs_t;

typedef enum
{
    UTL_UDPTR_TYPE_REQ = 0,     /* got a request */
    UTL_UDPTR_TYPE_RESP,        /* got a response */
    UTL_UDPTR_TYPE_RETRY,       /* retry to another server */
    UTL_UDPTR_TYPE_TIMEOUT,     /* timeout after trying all servers */
}EN_UTL_UDPTR_TYPE;

typedef struct tagstUtlUdpTransMessage
{
    int tid;
    
    EN_UTL_UDPTR_TYPE type;
    size_t len;
    char data[0];
}utl_udptr_msg_t;

utl_udptrs_t* utl_udptr_create(const char* loc_ip, int loc_port);
void utl_udptr_destroy(utl_udptrs_t* trs);

/* ADD other destination address which worked as a backup */
int utl_udptr_add_dest(utl_udptrs_t* trs, 
                        const char* host, int port, 
                        int expires, int noresptime);

int utl_udptr_start(utl_udptrs_t* trs);
int utl_udptr_stop(utl_udptrs_t* trs);

int utl_udptr_get_tid(utl_udptrs_t* trs);
bool utl_udptr_is_connected(utl_udptrs_t* trs);

/* 
 * SEND a request of a transaction 
 * param tid:   the transaction id
 * param data:  the pointer to the request data
 * param len:   the length of data
 *
 * return:      0 if successful, otherwise negative
 */
int utl_udptr_send_req(utl_udptrs_t* trs, int tid, const void* data, int len);

/* 
 * SEND a response for a transaction 
 * param tid:   the transaction id
 * param data:  the pointer to the response data
 * param len:   the length of data
 *
 * return:      0 if successful, otherwise negative
 */
int utl_udptr_send_resp(utl_udptrs_t* trs, int tid, const void* data, int len);

/* Receive a message from the transactions message queue
 * > 0: wait a moment within some time in milli-seonds
 * = 0: do not wait
 * < 0: wait for ever
 */
utl_udptr_msg_t* utl_udptr_recv(utl_udptrs_t* trs, int ms);

void utl_udptr_msg_free(utl_udptr_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif


/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_rudp.h
* PACKAGE:      libeut
*   Header of:          utl_rudp.c
*   Date:               01/25/10
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_RUDP_H__
#define  __UTL_RUDP_H__

#ifdef __VXWORKS_OS__
#include <time.h>
#include <sys/times.h>
#else
#include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlRudp utl_rudp_t;
typedef struct tagstUtlRudpLink utl_rudp_link_t;
typedef struct tagstUtlRudpMsg utl_rudp_msg_t;

typedef void (*utl_rudp_recv_func)(utl_rudp_msg_t* msg, void* data);
typedef int (*utl_rudp_stat_func)(void* data, const char* fmt, ...);

/**
 * Initialize a reliable udp channel
 * @param host:     the local host address(NULL means "0.0.0.0")
 * @param port:     the local port
 * @param links:    the count of links that can be used, the default value is 16
 *
 * @return:         pointer to the rudp created
 */
utl_rudp_t* utl_rudp_init(const char* host, int port, int links);

/**
 * RELEASE the resource of a rudp
 * @param rudp:     the rudp to be freed
 */
void utl_rudp_free(utl_rudp_t* rudp);

/**
 * STATISTIC the rudp infomation
 * @param rudp:     the rudp to be statistics
 * @param stat:     the statistic function
 * @param data:     the data used when invoking the function "stat"
 *                  if NULL eos_fprintf(stdout, ...) will be invoked
 *
 * @return:         the length of statistics information
 */
int utl_rudp_stat(utl_rudp_t* rudp, utl_rudp_stat_func stat, void* data);

/**
 * Link to remote address(create a virtual link)
 * @param rudp:     the rudp to be used
 * @param host:     the host address of remote
 * @param port:     the port of remote
 *
 * @return:         pointer to the link created
 */
utl_rudp_link_t* utl_rudp_link(utl_rudp_t* rudp, const char* host, int port);

/**
 * SET the message receive function of a link
 * @param link:     the link to be used
 * @param recv:     the function to be invoked when message received
 * @param data:     the data to be used when the recv funcion invoking
 *
 * @note:           if the recv function is set to non-NULL value, 
 *                  the msg received will not be put to it's own queue
 */
void utl_rudp_set_recv(utl_rudp_link_t* link, utl_rudp_recv_func recv, void* data);

/**
 * SEND a message via a link
 * @param link:     the link to be used
 * @param buf:      the pointer to the message to be sent
 * @param len:      the length of the message to be sent
 *
 * @return:         the bytes sent
 */
int utl_rudp_send(utl_rudp_link_t* link, const char* buf, int len);

/**
 * RECEIVE a message in blocking mode
 * @param link:     the link from which the message received
 * @param timeout:  the time for waiting till a message received
 *                  NULL: wait for ever till a message received
 *
 * @return:         the rudp message received or NULL if timeout
 */
utl_rudp_msg_t* utl_rudp_recv(utl_rudp_link_t* link, struct timeval* timeout);

/**
 * RECEIVE a message in non-blocking mode
 * @param link:     the link from which the message received
 *
 * @return:         the rudp message received or NULL
 */
utl_rudp_msg_t* utl_rudp_grab(utl_rudp_link_t* link);

/**
 * STATISTIC the infomation of a specific link
 * @param link:     the link to be statistics
 * @param stat:     the statistic function
 * @param data:     the data used when invoking the function "stat"
 *                  if NULL eos_fprintf(stdout, ...) will be invoked
 *
 * @return:         the length of statistics information
 */
int utl_rudp_link_stat(utl_rudp_link_t* link, utl_rudp_stat_func stat, void* data);

/**
 * GET the data pointer of a message
 * @param msg:      the message whose data will be got
 *
 * @return:         the data pointer of the message
 */
void* utl_rudp_msg_data(utl_rudp_msg_t* msg);

/**
 * GET the length of a message
 * @param msg:      the message whose length will be got
 *
 * @return:         the data length of the message
 */
int utl_rudp_msg_len(utl_rudp_msg_t* msg);

/**
 * RELEASE the resource of a message
 * @param msg:      the message to be freed
 */
void utl_rudp_msg_free(utl_rudp_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif


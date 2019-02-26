/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_mbuf.h
* PACKAGE:      libeut
*   Header of:          utl_mbuf.c
*   Date:               2016.09.26
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_MBUF_H__
#define  __UTL_MBUF_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlMsgBuffer utl_mbuf_t;

/**
 * Create a mesage buffer with a specified initialized size
 * @param size:     the initialized buffer size in bytes
 * @param auto_inc: auto increase the buffer size when not enough
 *
 * @return:         the pointer to the mbuf created, or NULL if failed
 *
 * @NOTE: the created mbuf DON'T use mutex!!! if you want pls add yourself or use msgq
 */
utl_mbuf_t* utl_mbuf_create(int size, int auto_inc);

/**
 * RELEASE the resource of a message buffer
 * @param mbuf:     the message buffer to be freed
 */
void utl_mbuf_destroy(utl_mbuf_t* mbuf);

/**
 * SEND a message to a message buffer
 * @param mbuf:     the message buffer to which the message send
 * @param msg:      the message to be sent
 * @param len:      the length of the message to be sent
 *
 * @return:         0 if successfully, negative otherwise
 */
int utl_mbuf_send(utl_mbuf_t* mbuf, const void* msg, int len);

/**
 * RECEIVE a message from a message buffer
 * @param mbuf:     the message buffer from which the message received
 * @param buf:      the buffer used to store the received message
 * @param size:     the size of buffer specified by param "buf"
 *
 * @return:         the length of received message or zero if failed
 */
int utl_mbuf_recv(utl_mbuf_t* mbuf, void* buf, int size);

#ifdef __cplusplus
}
#endif

#endif


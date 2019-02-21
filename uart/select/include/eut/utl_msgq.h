/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_msgq.h
* PACKAGE:      libeut
*   Header of:          utl_msgq.c
*   Date:               01/27/10
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_MSGQ_H__
#define  __UTL_MSGQ_H__

#ifdef __VXWORKS_OS__
#include <time.h>
#include <sys/times.h>
#else
#include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlMsgQueue utl_msgq_t;

typedef void (*utl_msgq_hook_notify_t)(void* user_data);

/**
 * Initialize a mesage queue
 * @param p_msgq:   the message queue to be initialized
 *
 * @return:         0 if successfully, negative otherwise
 */
int utl_msgq_init(utl_msgq_t** p_msgq);

/**
 * Create a mesage queue with specified message count
 * @param initMsgs: the initial message count of msgq
 *
 * @return:         the pointer to the msgq created, or NULL if failed
 */
utl_msgq_t* utl_msgq_create(int initMsgs);

/**
 * RELEASE the resource of a message queue
 * @param msgq:     the message queue to be freed
 */
void utl_msgq_free(utl_msgq_t* msgq);

/**
 * SET the maximum of the message queue size
 * @param msgq:     the message queue to which the message send
 * @param maxMsgs:  the maximum of msgq size
 *
 * @return:         0 if successfully, negative otherwise
 */
void utl_msgq_set_max_msgs(utl_msgq_t* msgq, int maxMsgs);

/**
 * SET a notify hook function to a message queue
 * @param msgq:         the message queue to be worked on
 * @param fp_notify:    the notify function to be set, it will be invoked after a message is sent
 * @param user_data:    the user data used by function fp_notify
 *
 */
void utl_msgq_set_hook_notify(utl_msgq_t* msgq, utl_msgq_hook_notify_t fp_notify, void* user_data);

/**
 * SEND a message to a message queue
 * @param msgq:     the message queue to which the message send
 * @param msg:      the message to be sent
 *
 * @return:         0 if successfully, negative otherwise
 */
int utl_msgq_send(utl_msgq_t* msgq, void* msg);

/**
 * RECEIVE a message in blocking mode
 * @param msgq:     the message queue from which the message received
 * @param timeout:  the time for waiting till a message recieved
 *                  NULL: wait for ever till a message received
 *
 * @return:         the message received and the timeout left or NULL if timeout
 */
void* utl_msgq_recv(utl_msgq_t* msgq, struct timeval* timeout);

/**
 * RECEIVE a message in blocking mode within a specified time in milli-second
 * @param msgq:     the message queue from which the message received
 * @param ms:       the time for waiting till a message recieved
 *                  >0: blocking specified time till a message received
 *                  =0: no blocking
 *                  <0: blocking forever till a message received
 *
 * @return:         the message received or NULL if timeout
 */
void* utl_msgq_recv_ms(utl_msgq_t* msgq, int ms);

/**
 * RECEIVE a message in non-blocking mode
 * @param msgq:     the message queue from which the message received
 *
 * @return:         the message received or NULL
 */
void* utl_msgq_grab(utl_msgq_t* msgq);

/**
 * GET the number of unreceived messages in message queue
 * @param msgq:     the message queue to be worked on
 *
 * @return:         the message number
 */
int utl_msgq_size(utl_msgq_t* msgq);

/**
 * Create a mesage buffer queue, you can send a message buffer to it
 * @param initMsgs: the initial message count of msgq
 * @param msgLen:   the max length of message
 *
 * @return:         the pointer to the msgq created, or NULL if failed
 */
utl_msgq_t* utl_msgq_buf_create(int initMsgs, int msgLen);

/**
 * SEND a message buffer to a message queue
 * @param msgq:     the message queue to which the message send
 * @param msg:      the message to be sent
 * @param len:      the length of message to be sent
 *
 * @return:         0 if successfully, negative otherwise
 */
int utl_msgq_buf_send(utl_msgq_t* msgq, const void* msg, int len);

/**
 * RECEIVE a message buffer in blocking mode within a specified time in milli-second
 * @param msgq:     the message queue from which the message received
 * @param buf:      the message buffer used to store the received message
 * @param size:     the message buffer size
 * @param ms:       the time for waiting till a message recieved
 *                  >0: blocking specified time till a message received
 *                  =0: no blocking
 *                  <0: blocking forever till a message received
 *
 * @return:         0 if successfully, negative otherwise
 */
int utl_msgq_buf_recv(utl_msgq_t* msgq, void* buf, int size, int ms);

#ifdef __cplusplus
}
#endif

#endif


/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_select.h
* PACKAGE:      libeut
*   Implement of:       utl_select.c
*   Date:               07/02/08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_SELECT_H__
#define __UTL_SELECT_H__

#include "utl_socket.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
    UTL_SOEVT_READ = 0,
    UTL_SOEVT_WRITE,
    UTL_SOEVT_CONNECT,

    UTL_SOEVT_ACCEPT,
    UTL_SOEVT_CLOSE,

    UTL_SOEVT_SIZE
}utl_soevt_e;

/* the utl socket select engine handle */
typedef void* utl_select;
/* the utl select engine callback funtion */
typedef void (*utl_select_f)
    (utl_select sel, utl_sock sock, utl_soevt_e evt, void* data);

typedef int (*utl_select_lock_f)(void* mutex);
typedef int (*utl_select_unlock_f)(void* mutex);

/**
 * INITIALIZE a select engine
 * @param pSel:     restore the select engine generated
 * @param cbMsg:    the message(read from param "sock") process callback function
 * @param data:     the user data
 *
 * @return:         0 if successful, none zero otherwise
 */
int utl_select_init(utl_select* pSel, utl_select_f cbMsg, void* data);
/**
 * RELEASE a select engine
 * @param sel:      the select engine to be released
 *
 * @return:         0 if successful, none zero otherwise
 */
int utl_select_free(utl_select sel);
/**
 * SET a sem to a select engine
 * @param sel:      the select engine to be set
 * @param mutex:    the user mutex the engine should be used later
 * @param fpLock:   the user lock routine
 * @param fpUnLock: the user unlock routine
 *
 * @return:         0 if successful, none zero otherwise
 * @note:           the rouine must be called before utl_select_run();
 *                  the sem must be a pointer to CsMutex
 */
void utl_select_set_sem(utl_select sel, void* mutex,
                        utl_select_lock_f fpLock,
                        utl_select_unlock_f fpUnLock);

/**
 * SET select engine to background mode if macro __ACOM_MACOS defined
 * @param sel:      the select engine to be set
 * @param bgmode:   set to background or not
  */
void utl_select_set_background(utl_select sel, bool bgmode);

/**
 * GET the user data of a select engine
 * @param sel:      the select engine to be worked on
 *
 * @return:         the pointer to user data
 */
void* utl_select_get_data(utl_select sel);
/**
 * SEND a message via the internal pipe of a select engine 
 * @param sel:      the select engine to be worked on
 * @param msg:      the pointer to the message to be sent
 * @param len:      the length of the message
 *
 * @return:         the length sent, if failed less than "len"
 */
int utl_select_send(utl_select sel, const char* msg, int len);
/**
 * RECEIVE a message via the internal pipe of a select engine when message received
 * @param sel:      the select engine to be worked on
 * @param buf:      the pointer to the buffer to restore the messge
 * @param size:     the size of the buffer
 *
 * @return:         the length sent, if failed less than "len"
 */
int utl_select_recv(utl_select sel, char* buf, int size);

/**
 * ADD a socket to a select engine
 * @param sel:      the select engine to be worked on
 * @param sock:     the socket(initialized before) to be added
 * @param evt:      the socket event
 * @param cbSock:   the socket process callback function
 * @param data:     the user data
 *
 * @return:         0 if successful, none zero otherwise
 */
int utl_select_add(utl_select sel, utl_sock sock, utl_soevt_e evt, 
                   utl_select_f cbSock, void* data);
/**
 * DELETE a socket from a select engine and close it
 * @param sel:      the select engine to be worked on
 * @param sock:     the socket to be deleted
 *
 * @return:         true if deleted, false if none
 */
bool utl_select_del(utl_select sel, utl_sock sock);

/**
 * DELETE a socket from a select engine without closing it
 * @param sel:      the select engine to be worked on
 * @param sock:     the socket to be deleted
 *
 * @return:         true if deleted, false if none
 */
bool utl_select_del_safe(utl_select sel, utl_sock sock);

/**
 * CONNECT(blocking, for client) to a select engine
 * @param sel:      the select engine to be worked on
 *
 * @return:         the socket used to connect
 */
utl_sock utl_select_connect(utl_select sel);

/**
 * SET the ACCEPT(for server) hook of a select engine
 * @param sel:      the select engine to be worked on
 * @param handler:  the handler for the accept event
 * @param data:     the user data for the hook
 *
 * @return:         0 if successful, none zero otherwise
 */
int utl_select_set_hook_accept(utl_select sel, utl_select_f handler, void* data);

/**
 * RUN(blocking) a select engine within specifed period
 * @param sel:      the select engine to be worked on
 * @param ms:       the specified period time in milliseconds,
 *                  ms > 0: blocking within this period unless 
 *                  ms = 0: non blocking
 *                  ms < 0: blocking always
 *                  exit if some message received or utl_select_stop() invoked
 */
void utl_select_run(utl_select sel, int ms);
/**
 * STOP a select engine
 * @param sel:      the select engine to be worked on
 */
void utl_select_stop(utl_select sel);

#if defined(__cplusplus)
}
#endif

#endif


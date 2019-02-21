/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_session.h
* PACKAGE:      libeut
*   Header of:          utl_session.c
*   Date:               09/09/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_SESSION_H__
#define  __UTL_SESSION_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int type;
    void* msg;
}utl_sess_msg_t;

typedef struct tagstUtlSession utl_sess_t;

typedef int (*utl_sess_msg_proc_f)(void* msg, void* data, utl_sess_t** sess);
typedef void (*utl_sess_wakeup_f)(void* data);

int utl_sess_init(utl_sess_t** p_sess, bool wom, int wakeup_time);
void utl_sess_free(utl_sess_t* sess);

int utl_sess_set_hander(utl_sess_t* sess, int type,
                        utl_sess_msg_proc_f proc, void* data);

void utl_sess_enable_wom(utl_sess_t* sess, bool enable);
void utl_sess_set_wakeup_time(utl_sess_t* sess, int wakeup_time);
void utl_sess_set_wakeup_hander(utl_sess_t* sess, utl_sess_wakeup_f wakeup, void* data);

int utl_sess_start(utl_sess_t* sess);
void utl_sess_stop(utl_sess_t* sess);

/* Send a message without signal */
int utl_sess_send(utl_sess_t* sess, int type, void* msg);
/* Send n messages, with signal if wakeup-on-msg enabled*/
int utl_sess_nsend(utl_sess_t* sess, utl_sess_msg_t* msgs, unsigned short n);
/* Send a signal to wakeup the session */
void utl_sess_wakeup(utl_sess_t* sess);

#ifdef __cplusplus
}
#endif

#endif


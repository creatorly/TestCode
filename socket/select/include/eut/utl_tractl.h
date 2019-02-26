/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_congctl.h
* PACKAGE:      libeut
*   Header of:          utl_tractl.c
*   Date:               09/04/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_TRACTL_H__
#define  __UTL_TRACTL_H__

#include "utl_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlTrafficControl utl_tractl_t;

typedef void (*FP_UTL_TRACTL_AVAIL)(utl_tractl_t* ctl, int allowed_trfcs, void* user_data);

typedef void (*FP_UTL_TRACTL_READY)(void* data);
typedef void (*FP_UTL_TRACTL_TIMEOUT)(void* data);

int utl_tractl_init(utl_tractl_t** ctl, 
                    int ts_len, int ts_num, 
                    int ts_off_caps, int ts_on_caps);

utl_tractl_t* utl_tractl_create(int duration, int ts_num, int off_caps, int on_caps);
void utl_tractl_free(utl_tractl_t* ctl);

/* if this routine is invoked, no need to invoke utl_tractl_run() */
void utl_tractl_set_timers(utl_tractl_t* ctl, utl_timers_t* pTimers);

/* when the traffic control became available, the hook fpAvail will be invoked */
void utl_tractl_set_hook_avail(utl_tractl_t* ctl, FP_UTL_TRACTL_AVAIL fpAvail, void* data);

/* return 0 if test OK */
int utl_tractl_test(utl_tractl_t* ctl);

/* return the offtime in ms of the slide window */
int utl_tractl_get_offtime(utl_tractl_t* ctl);

bool utl_tractl_inc(utl_tractl_t* ctl);
void utl_tractl_clr(utl_tractl_t* ctl);

/* return the task id if successful, -1 otherwise */
int utl_tractl_add(utl_tractl_t* ctl, int wait, void* data, 
                   FP_UTL_TRACTL_READY fpReady, 
                   FP_UTL_TRACTL_TIMEOUT fpTmo);

/* delete the task added by utl_tractl_add() before */
int utl_tractl_del(utl_tractl_t* ctl, int tid);

void utl_tractl_run(utl_tractl_t* ctl);

const char* utl_tractl_status(utl_tractl_t* ctl);

#ifdef __cplusplus
}
#endif

#endif


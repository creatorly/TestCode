/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_timer_ex.h
* PACKAGE:      libeut
*   Header of:          utl_timer_ex.c
*   Date:               2016.10.26
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_TIMER_EX_H__
#define  __UTL_TIMER_EX_H__

#include <utl_timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/* struct definition of timer and group timer; */
typedef struct tagstUtlTimersEx utl_timers_ex_t;

/**
 * CREATE a timer group with specified min and max time in milli-seconds
 * @param min_time: The min time in milli-seconds of timer
 * @param max_time: The max time in milli-seconds of timer
 *
 * @return:         the created timer group
 */
utl_timers_ex_t* utl_timers_ex_create(int min_time, int max_time);

/**
 * DELETE a specified timer group
 * @param tmrGrp:   The timer group to be deleted
 */
void utl_timers_ex_delete(utl_timers_ex_t* tmrGrp);

/**
 * GET the number of timers in specified timer group
 * @param tmrGrp:   The timer group to be worked on
 *
 * return:          the timer number
 */
int utl_timers_ex_size(utl_timers_ex_t* tmrGrp);

/**
 * START a timer in specified timer group
 * @param tmrGrp:   The timer group to be worked on
 * @param timer:    The timer to be started
 */
void utl_timer_ex_start(utl_timers_ex_t* tmrGrp, utl_timer_t* timer);

/**
 * STOP a timer in specified timer group
 * @param tmrGrp:   The timer group to be worked on
 * @param timer:    The timer to be stoped
 */
void utl_timer_ex_stop(utl_timers_ex_t* tmrGrp, utl_timer_t* timer);

/**
 * RESET a timer with new length in specified timer group
 * @param tmrGrp:   The timer group to be worked on
 * @param timer:    The timer to be reset
 * @param period:   The new length of the timer
 *
 * @NOTE:   If the timer started before, the function will stop it first
 */
void utl_timer_ex_reset(utl_timers_ex_t* tmrGrp, utl_timer_t* timer, int period);

/**
 * GET the remain time of next timer to be fired in a specified timer group
 * @param tmrGrp:   The timer group to be worked on
 *
 * @return:         the max timeout in milli-seconds
 */
int utl_timer_ex_nexttmo(utl_timers_ex_t* tmrGrp);

/**
 * RUN a specified timer group
 * @param tmrGrp:       The timer group to be run
 * @param max_fired:    Max number of timers to be run, zero means all
 *
 * @return:             The number of expired timers
 *
 * @NOTE:   All timer functions will be invoked in this function
 */
int utl_timer_ex_run(utl_timers_ex_t* tmrGrp, int max_fired);

/**
 * SHOW the infomations of the timers in the given timer group
 * @param tmrGrp:   The timer group to be worked on
 * @param fpPrint:  The print function user specified
 * @param data:     The data used by function fpPrint
 *
 * @return:     The length of timer informations
 */
int utl_timer_ex_show(utl_timers_ex_t* tmrGrp, utl_timer_print_f fpPrint, void* data);

#ifdef __cplusplus
}
#endif

#endif


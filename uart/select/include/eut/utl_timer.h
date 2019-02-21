/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_timer.h
* PACKAGE:      libeut
*   Header of:          utl_timer.c
*   Date:               09/04/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_TIMER_H__
#define  __UTL_TIMER_H__

#include <stdbool.h>
#ifdef __VXWORKS_OS__
#include <time.h>
#include <sys/times.h>
#else
#include <sys/time.h>
#endif

#include "utl_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef UTL_TIMER_DEBUG

typedef enum
{
    UTL_TIMERMODE_ONESHOT,
    UTL_TIMERMODE_PERIODIC
}utl_timer_mode_t; 

/* struct definition of timer and group timer; */
typedef struct tagstUtlTimers utl_timers_t;
typedef struct tagstUtlTimer utl_timer_t;

/* timer callback function, DO NOT operate the timer itself in it */
typedef void (*utl_timer_func)(void*, utl_timers_t*, utl_timer_t*);
typedef int (*utl_timer_print_f)(void*, const char*, ...);

typedef struct tagstUtlTimersData utl_timers_data_t;
typedef struct tagstUtlTimersSlice utl_timers_slice_t;

struct tagstUtlTimers
{
    utl_timers_data_t* ts_data;
};

/* DO NOT change the members of the struct */
struct tagstUtlTimer
{
    utl_node_t node;

    int period;                 /* in milli-seconds */
    utl_timer_mode_t mode;
    
    utl_timer_func timer_f;
    void* data;

    int running;
    unsigned run_tick;          /* the running ticks */
    unsigned end_tick;          /* the final ending ticks */
    unsigned tmo_tick;          /* the ticks for next timeout */

#ifdef UTL_TIMER_DEBUG
    const char* name;
    const char* file;
    int init_line;
    int start_line;
#endif
    
    utl_timers_slice_t* ts_slice;
};

/* create and destroy a group timer */
int utl_timers_init(utl_timers_t* gtimer);
void utl_timers_free(utl_timers_t* gtimer);

int utl_timers_size(utl_timers_t* gtimer);

/* initialize, start(period in ms), stop and reset a timer */
void utl_timer_init_ex(utl_timer_t* timer, const char* file, int line,
                       const char* name, int period, utl_timer_mode_t mode, 
                       utl_timer_func func, void* data);

#define utl_timer_init(t, l, m, f, d) \
            utl_timer_init_ex(t, __FILE__, __LINE__, #f, l, m, f, d)

void utl_timer_set_name(utl_timer_t* timer, const char* name);
void utl_timer_set_time(utl_timer_t* timer, int period);
void utl_timer_set_mode(utl_timer_t* timer, utl_timer_mode_t mode);
void utl_timer_set_func(utl_timer_t* timer, utl_timer_func func, void* data);

bool utl_timer_isrun(utl_timer_t* timer);
int  utl_timer_left(utl_timer_t* timer);

void utl_timer_start_ex(utl_timers_t* gtimer, utl_timer_t* timer, int line);
#define utl_timer_start(gt, t)  utl_timer_start_ex(gt, t, __LINE__)

void utl_timer_stop(utl_timers_t* gtimer, utl_timer_t* timer);
void utl_timer_reset(utl_timers_t* gtimer, utl_timer_t* timer, int period);

#define utl_timer_init_start(gt, tmo, m, f, d, pt) \
do{ \
    utl_timer_init(pt, tmo, m, (utl_timer_func)f, d); \
    utl_timer_start(gt, pt); \
}while(0)

/**
 * GET the max timeout of the timer group
 * @param gtimer:   The timer group to be worked on
 *
 * @return:         the max timeout in milli-seconds
 */
int utl_timer_get_timeout(utl_timers_t* gtimer);

/**
 * EXECUTE specified number of fired timers
 * @param gtimer:   The timer group to be worked on
 * @param num:      The max number of timers fired, zero means all timers
 *
 * @return:         The number of timers fired
 */
int utl_timer_run(utl_timers_t* gtimer, int num);

/**
 * SHOW the active timers
 * @param gtimer:   The timer group to be worked on
 * @param fpPrint:  The function used to be print
 * @param data:     The data used by function fpPrint
 *
 * @return:         false if all fired timers are executed or true otherwise
 */
int utl_timer_show(utl_timers_t* tmrs, utl_timer_print_f fpPrint, void* data);

#ifdef __cplusplus
}
#endif

#endif


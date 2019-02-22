/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_tpool.h
* PACKAGE:      libeut
*   Header of:          utl_tpool.c
*   Date:               2015.06.19
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_TPOOL_H__
#define  __UTL_TPOOL_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlThreadPool utl_tpool_t;

typedef int (*utl_tpool_msgproc_f)(void* msg, void* data);
typedef int (*utl_tpool_msgfree_f)(void* msg);
typedef int (*utl_tpool_printf_f)(void* fd, const char* fmt, ...);

utl_tpool_t* utl_tpool_create(int min_thread_num, int max_thread_num);
void utl_tpool_destroy(utl_tpool_t* tp);

void utl_tpool_set_name(utl_tpool_t* tp, const char* name);
void utl_tpool_set_priority(utl_tpool_t* tp, int priority);
void utl_tpool_set_stacksize(utl_tpool_t* tp, int stack_size);

/* the total threads maybe increased or decremented */
int utl_tpool_set_threads(utl_tpool_t* tp, int max_threads);

int utl_tpool_start(utl_tpool_t* tp);
int utl_tpool_stop(utl_tpool_t* tp);

int utl_tpool_add(utl_tpool_t* tp, void* msg, void* data, 
        utl_tpool_msgproc_f fpProc, utl_tpool_msgfree_f fpFree);
int utl_tpool_show(utl_tpool_t* tp, utl_tpool_printf_f fpPrintf, void* fd);

#ifdef __cplusplus
}
#endif

#endif


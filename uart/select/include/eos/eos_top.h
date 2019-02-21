/********************************************************
* 
*
* Ejoin EOS platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         eos_top.h
* PACKAGE:      libeos
*   Header of:          eos_top.c
*   Date:               2016.10.31
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __EOS_TOP_H__
#define  __EOS_TOP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EOS_TOP_MAX_PROCS   10

typedef struct tagstEosTopKeys
{
    const char* k_cpu;      /* key for CPU line,        default is "Cpu(s)" */
    const char* k_mem;      /* key for Mem line,        default is "Mem" */
    
    const char* kc_idle;    /* key for CPU idle,        default is "id" */
    const char* km_used;    /* key for memory used,     default is "used" */
    const char* km_free;    /* key for momory free,     default is "free" */

    const char* kp_pid;     /* key for process PID,     default is "PID" */
    const char* kp_cpu;     /* key for process %CPU,    default is "%CPU" */
    const char* kp_mem;     /* key for process %MEM,    default is "%MEM" */
    const char* kp_tim;     /* key for process TIME+,   default is "TIME+" */
    const char* kp_cmd;     /* key for process COMMAND, default is "COMMAND" */
}eos_top_keys_t;

typedef struct tagstEosTopProcessInfo
{
    int p_pid;
    int p_cpu;
    int p_mem;
    int p_tim;              /* TIME+ in seconds */
    char p_cmd[64];
}eos_top_pinfo_t;

typedef struct tagstEosTopInfo
{
    int cpu_usage;          /* in percent */
    int mem_used;           /* in KBs */
    int mem_free;           /* in KBs */

    int proc_num;
    eos_top_pinfo_t proc_infos[EOS_TOP_MAX_PROCS];
}eos_top_info_t;

typedef void (*eos_top_handler_t)(eos_top_info_t* top_info, void* usrDat);

/**
 * INITIALIZE the global top instanse
 * @param keys:     The keys for top, if NULL is passed, regard all keys as default
 *
 * @return:         0 if successful, otherwise non-zero
 */
int eos_top_init(eos_top_keys_t* keys);

/**
 * FREE the global top instanse
 *
 * @return:         0 if successful, otherwise non-zero
 */
void eos_top_free();

/**
 * ADD a handler for the output string of top running one time
 * @param fpHandler:    The handler to be added
 * @param usrDat:       The user data used by function fpHandler
 *
 * @NOTE:   the function fpHandler will be invoked when top each running
 */
void eos_top_add_handler(eos_top_handler_t fpHandler, void* usrDat);

/**
 * START a memory monitor in a specified value(KB) list in DESC oreder
 * @param fpHandler:    The handler to be added
 * @param usrDat:       The user data used by function fpHandler
 *
 * @NOTE:   The memory value list must be order by DESC
 */
void eos_top_mem_mon(int* vals, int val_num, eos_top_handler_t fpHandler, void* usrDat);

/**
 * START a cpu usage monitor in a specified value(%) list in ASC oreder
 * @param fpHandler:    The handler to be added
 * @param usrDat:       The user data used by function fpHandler
 *
 * @NOTE:   The cpu usage list must be order by ASC
 */
void eos_top_cpu_mon(int* vals, int val_num, eos_top_handler_t fpHandler, void* usrDat);

/**
 * START the top instanse
 * @param count:    The count of top command running
 * @param delay:    The delay time in seconds of top command
 *
 * @return:         0 if successful, otherwise non-zero
 */
int eos_top_start(int count, int delay);

/**
 * STOP the top instanse
 */
void eos_top_stop();

#ifdef __cplusplus
}
#endif

#endif


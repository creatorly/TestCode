/********************************************************
* 
*
* Ejoin EOS platform
* Copyright (C) 2017-2017, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         eos_stat.h
* PACKAGE:      libeos
*   Header of:          eos_stat.c
*   Date:               2017.06.21
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __EOS_STAT_H__
#define  __EOS_STAT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* the default CPU usage scale is 100 */
typedef struct tagstEosCpuStat
{
    int cs_usr;         /* cpu usage of user */
    int cs_nic;         /* cpu usage of nice */
    int cs_sys;         /* cpu usage of system */
    int cs_idle;        
    int cs_iow;         /* cpu usage of iowait */
    int cs_irq;
    int cs_sirq;        /* cpu usage of softirq */
    int cs_st2l;        /* cpu usage of stealtolen */
    int cs_guest;
}eos_cpu_stat_t;

/* the unit of memory size is kB */
typedef struct tagstEosMemStat
{
    int ms_tot;         /* total memory size in kB */
    int ms_used;        /* used memory size in kB */
    int ms_free;        /* free memory size in kB */
    int ms_buf;         /* buffers memory size in kB */
    int ms_cach;        /* cached memory size in kB */
}eos_mem_stat_t;

/**
 * GET a cpu statistics
 * @param pStat:        The pointer to the buffer used to store the cpu statistics
 * @param percent:      The percent of cpu usage, default is 100.
 *                      if you want the usage like xx.x%, you should set percent as 1000
 *
 * return:              zero if successful, -1 otherwise
 */
int eos_get_cpustat(eos_cpu_stat_t* pStat, int percent);

/**
 * GET a memory statistics
 * @param pStat:        The pointer to the buffer used to store the memory statistics
 *
 * return:              zero if successful, -1 otherwise
 */
int eos_get_memstat(eos_mem_stat_t* pStat);

#ifdef __cplusplus
}
#endif

#endif


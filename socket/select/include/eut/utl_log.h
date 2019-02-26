/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2011-2011, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_log_.h
* PACKAGE:      libeut
*   Header of:          utl_log.c
*   Date:               12/09/11
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_LOG_H__
#define  __UTL_LOG_H__

#include <stdarg.h>
#include <stdbool.h>
#include <utl_typdef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_LOG_TIME    1
#define UTL_LOG_SRC     2       /* log source identification used for UDP log server */
#define UTL_LOG_NAME    4
#define UTL_LOG_SEQ     8
#define UTL_LOG_LEVEL   0x10
#define UTL_LOG_CODE    0x20
#define UTL_LOG_TRACE   0x40
#define UTL_LOG_INDEX   0x80
#define UTL_LOG_LOGID   0x100
#define UTL_LOG_TICK    0x200   /* print ticks if UTL_LOG_TIME and UTL_LOG_TICK are both enabled */

#define UTL_LOG_NOPATH      0x10000
#define UTL_LOG_NOCRLF      0x20000
#define UTL_LOG_NOBUF       0x40000

#define UTL_LOG_FILE        0x80000000
#define UTL_LOG_STDOUT      0x40000000
#define UTL_LOG_UDP         0x20000000
#define UTL_LOG_TCP         0x10000000      /* reserved */
#define UTL_LOG_SNMP        0x08000000      /* reserved */
#define UTL_LOG_CALLBACK    0x04000000

#define UTL_LOG_ALL_HDRS    0xff

typedef struct tagstUtlLog utl_log_t;

typedef int (*utl_log_out_fp)(void* fd, int lev, const char* buf, int len);
typedef void (*utl_log_newfile_fp)(utl_log_t* pLog, const char* newfile, const char* oldfile, void* data);

/* set the log handle for the utils module itself */
void utl_log_set_self(utl_log_t* pLog, int level);

utl_log_t* utl_log_create(int level, const char* basename, int size, int count);
int utl_log_destroy(utl_log_t* hLog);

int utl_log_name2level(const char* lname);
const char* utl_log_level2name(int level);

int utl_log_set_level(utl_log_t* hLog, int level);
int utl_log_get_level(utl_log_t* hLog);

int utl_log_set_syslog_level(utl_log_t* pstLog, int level);

/* set the log source identification for UDP log server */
void utl_log_set_src(utl_log_t* hLog, const char* src);

/* set the name of log module */
void utl_log_set_name(utl_log_t* hLog, const char* name);

/* set the name of log level */
void utl_log_set_lname(utl_log_t* hLog, int level, const char* name);

int utl_log_get_flags(utl_log_t* hLog);
void utl_log_set_flags(utl_log_t* hLog, int flags);
void utl_log_set_flag(utl_log_t* hLog, int flag, bool enabled);
void utl_log_enable_flags(utl_log_t* hLog, int flags);
void utl_log_disable_flags(utl_log_t* hLog, int flags);

void utl_log_set_filesize(utl_log_t* hLog, int filesize);
void utl_log_set_filecount(utl_log_t* hLog, int filecount);

/**
 * SET the max size in bytes of cached logs
 * @param hLog:     the log handle to be worked on
 * @param max_buf:  the max buffer size in bytes to be set
 *
 * @Note:   non positive value means no limitation
 */
void utl_log_set_maxbuf(utl_log_t* hLog, int max_buf);

/**
 * SET the min size of log memory nodes
 * @param hLog:         the log handle to be worked on
 * @param min_nodes:    the min memory nodes to be set
 *
 * @Note:   if the passed min_nodes is too small, the default value(128) will be used
 */
void utl_log_set_minnodes(utl_log_t* hLog, int min_nodes);

/**
 * GET the filename of the existed log files
 * @param hLog:     the log handle to be worked on
 * @param count:    the count of logfile to be got, a zero value means all
 * @param buf:      the buffer used to store the filename
 * @param size:     the size of the given buffer
 * 
 * @return:         the length of value stored in the given buffer
 */
int utl_log_get_file(utl_log_t* hLog, int count, char* buf, int size);

int utl_log_set_server(utl_log_t* hLog, const char* host, int port);
void utl_log_set_callback(utl_log_t* hLog, utl_log_out_fp fpOut, void* fd);

void utl_log_set_hook_newfile(utl_log_t* hLog, utl_log_newfile_fp fpNewFile, void* data);

int UTL_CHK_FMTARGS(5) 
utl_log_add(utl_log_t* hLog, const char* fi, int li, int level,
            const char* fmt, ...);

int UTL_CHK_FMTARGS(9) 
utl_log_add_ex(utl_log_t* hLog, const char* fi, int li, 
               int lv, int code, int idx, const char* logid,
               const char* msg, const char* fmt, ...);

int UTL_CHK_FMTARGS(10)
utl_log_nadd(utl_log_t* pstLog, int size, const char* fi, int li, 
             int lv, int code, int idx, const char* logid,
             const char* msg, const char* fmt, ...);

int utl_log_vadd(utl_log_t* hLog, const char* fi, int li, 
                 int lv, const char* fmt, va_list va);

int utl_log_vadd_ex(utl_log_t* pstLog, const char* fi, int li, 
                    int lv, int code, int idx, const char* logid, 
                    const char* msg, const char* fmt, va_list va);

int utl_log_vnadd(utl_log_t* pstLog, int size, const char* fi, int li, 
                  int lv, int code, int idx, const char* logid, 
                  const char* msg, const char* fmt, va_list va);

/* add log without any header */
int UTL_CHK_FMTARGS(2) 
utl_log_printf(utl_log_t* pstLog, const char* fmt, ...);

int utl_log_vprintf(utl_log_t* pstLog, const char* fmt, va_list va);

int utl_log_cache_out(utl_log_t* pstLog);

#ifdef __cplusplus
}
#endif

#endif


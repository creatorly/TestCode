/********************************************************
*
* Ejoin Utils Library
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         utl_dba.h (DB Adapt for mysql)
* PACKAGE:      
*   Header of:          utl_dba.c
*   Date:               2016.07.09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_DBA_H__
#define __UTL_DBA_H__

#include <stdbool.h>
#include <mysql/mysql.h>
#include <utl_log.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagsUtlDBAdapt utl_dba_t;

utl_dba_t* utl_dba_create(int conn_num,
                          const char* dbname,
                          const char* username,
                          const char* password,
                          const char* host, int port);

void utl_dba_destroy(utl_dba_t* dba);

void utl_dba_set_maxsqls(utl_dba_t* dba, int max_sqls);
void utl_dba_set_maxwait(utl_dba_t* dba, int max_wait);

int utl_dba_start(utl_dba_t* dba);
int utl_dba_stop(utl_dba_t* dba);

void utl_dba_pause(utl_dba_t* dba);
void utl_dba_resume(utl_dba_t* dba);

/* all DB connections are paused */
bool utl_dba_paused(utl_dba_t* dba);

int utl_dba_query(utl_dba_t* dba, int index, const char* sql, int len);
int UTL_CHK_FMTARGS(3) utl_dba_vquery(utl_dba_t* dba, int index, const char* fmt_sql, ...);

int utl_dba_query_now(utl_dba_t* dba, const char* sql, int len);
int UTL_CHK_FMTARGS(2) utl_dba_vquery_now(utl_dba_t* dba, const char* fmt_sql, ...);

/* if a MYSQL_RES* returned, you MUST invoke mysql_free_result() to release it */
MYSQL_RES* utl_dba_query_res(utl_dba_t* dba, const char* sql, int len);
MYSQL_RES* UTL_CHK_FMTARGS(2) utl_dba_vquery_res(utl_dba_t* dba, const char* fmt_sql, ...);

#if defined(__cplusplus)
}
#endif

#endif


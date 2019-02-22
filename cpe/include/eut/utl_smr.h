/********************************************************
* 
*
* Ejoin SIP Utils Lib
* Copyright (C) 2008-2008, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         ut_smr.h
* PACKAGE:      libeut
*   Header of:          ut_smr.c
*   Date:               03/06/2008
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef UT_SMR_H
#define UT_SMR_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlSmr utl_smr_t;
typedef int utl_smr_rid_t;         /* Route ID */

typedef enum
{
    UTL_SMR_FLD_DSTDM = 1,
    UTL_SMR_FLD_DSTNUM = 2,
    UTL_SMR_FLD_SRCDM = 4,
    UTL_SMR_FLD_SRCNUM = 8,

    UTL_SMR_FLD_DST = (UTL_SMR_FLD_DSTDM | UTL_SMR_FLD_DSTNUM),
    UTL_SMR_FLD_SRC = (UTL_SMR_FLD_SRCDM | UTL_SMR_FLD_SRCNUM),
    UTL_SMR_FLD_ALL = (UTL_SMR_FLD_DST | UTL_SMR_FLD_SRC)
}EN_UTL_SMR_FLD;

/* the route key compare function */
typedef int (*utl_smr_cmp_f)(void* key1, void* key2);
/* the route key match function */
typedef bool (*utl_smr_match_f)(void* key, void* match_key);
/* the route statistic printf function */
typedef int (*utl_smr_printf_f)(void* fd, const char* fmt, ...);

/** 
 * Construct a route table.
 *
 * @param phRtbl    to store the route table handle.
 * @param enFlds    identify the valid route fields.
 *                  if no field specifed, using all fields
 * @param fpCmp     the route key compare function
 * @param fpMatch   the route key match function
 */
utl_smr_t* utl_smr_create(EN_UTL_SMR_FLD enFlds,
                          utl_smr_cmp_f fpCmp,
                          utl_smr_match_f fpMatch);

int utl_smr_destroy(utl_smr_t* hRt);

/**
 * Add a route to route table.
 * 
 * @inout
 * @param prid      to pass and store the route id allocted
 *                  if the input *prid > 0, try to use it.
 */
int utl_smr_add(utl_smr_t* hRt, 
              const char* dstDomain,
              const char* srcDomain,
              const char* dstNum,
              const char* srcNum,
              void* key,
              void* data,
              utl_smr_rid_t* prid);

/* Delete a route specified by rid from route table */
int utl_smr_del(utl_smr_t* hRt, utl_smr_rid_t rid);

/* Modify a route specifed by rid in route table. */
int utl_smr_modify(utl_smr_t* hRt, 
              utl_smr_rid_t rid,
              const char* dstDomain,
              const char* srcDomain,
              const char* dstNum,
              const char* srcNum,
              void* key,
              void* data);

/* Find a route specified by rid in route table */
int utl_smr_find(utl_smr_t* hRt, utl_smr_rid_t rid, void** ppUsrDat);

/* Get the user data of the route matched with the input */
int utl_smr_match(utl_smr_t* hRt, 
                const char* dstDm,
                const char* srcDm,
                const char* dstNum,
                const char* srcNum,
                void* key,
                void** ppUsrDat);

/* Get the user data of the route matched with the input */
int utl_smr_show(utl_smr_t* hRt, utl_smr_printf_f fpPrintf, void* fd);

#ifdef __cplusplus
}
#endif

#endif


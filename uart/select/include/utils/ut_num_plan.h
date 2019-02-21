/********************************************************
* 
*
* Cosmos-II SIP Proxy Lib
* Copyright (C) 2013-2013, All rights reserved
* Designed by Cosmos-II Development Team.
* 
* 
* File:         spx_numplan.h
* PACKAGE:      cosSpxLib
*   Header of:          spx_numplan.c
*   Date:               2013.03.05
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UT_NUMPLAN_H__
#define __UT_NUMPLAN_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_UT_NUM_PLAN

typedef struct tagstSpxNumPlan* PST_UT_NUMPLAN;

/* if argument translate_caller is false, the called number will translated */
PST_UT_NUMPLAN UT_NumPlanCreate(bool translate_caller);

void UT_NumPlanDestroy(PST_UT_NUMPLAN pnp);

int UT_NumPlanAdd(PST_UT_NUMPLAN pnp, 
                  const char* srcnum,
                  const char* srcdom,
                  const char* dstnum,
                  const char* dstdom,
                  int nNumStripped, 
                  const char* numAdded);

void UT_NumPlanClear(PST_UT_NUMPLAN pnp);
int UT_NumPlanSize(PST_UT_NUMPLAN pnp);

int UT_NumPlanDump(PST_UT_NUMPLAN pnp, char* buf, int size);

int UT_NumPlanMatch(PST_UT_NUMPLAN pnp, 
                    const char* srcnum,
                    const char* srcdom,
                    const char* dstnum,
                    const char* dstdom,
                    char* newnum, int size);

#endif

#ifdef __cplusplus
}
#endif

#endif


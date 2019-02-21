/********************************************************
* 
*
* Ejoin Goip Platform
* Copyright (C) 2013-2013, All rights reserved
* Designed by Ejoin Goip Development Team.
* 
* 
* File:         ccm_mbase.h
* PACKAGE:      cosSpxLib
*   Header of:          ccm_mbase.c
*   Date:               2013.04.12
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __CCM_MBASE_H__
#define __CCM_MBASE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstCcmMobileBaseSet* PST_CCM_MBS;
typedef struct tagstCcmMobileBaseMgr* PST_CCM_MBM;

PST_CCM_MBS CCM_MBSCreate(int iMaxBases, int iValidSignal);
void CCM_MBSDestroy(PST_CCM_MBS pMbs);

void CCM_MBSSetMaxBases(PST_CCM_MBS pMbs, int iMaxBases);
void CCM_MBSSetValidSignal(PST_CCM_MBS pMbs, int iValidSignal);

void CCM_MBMSetMaxBases(PST_CCM_MBS pMbs, int iMaxBases);
void CCM_MBMSetValidSignal(PST_CCM_MBS pMbs, int iValidSignal);

PST_CCM_MBM CCM_MBMCreate(PST_CCM_MBS pMbs, 
                          const char* blackList, 
                          const char* whiteList);
void CCM_MBMDestroy(PST_CCM_MBM pMbm);

void CCM_MBMSetBlackList(PST_CCM_MBM pMbm, const char* blackList);
void CCM_MBMSetWhiteList(PST_CCM_MBM pMbm, const char* whiteList);

int CCM_MBMUpdate(PST_CCM_MBM pMbm, const char* bases);
int CCM_MBMGetBase(PST_CCM_MBM pMbm);

#ifdef __cplusplus
}
#endif

#endif


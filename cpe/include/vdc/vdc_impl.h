/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vdc_impl.h
* PACKAGE:      
*   Header of:          vdc_impl.c
*   Date:               2015.10.14
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VDC_IPL_H__
#define  __VDC_IPL_H__

#include <utl_conn.h>
#include <utl_httpc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstVdcIpl* PST_VDC_IPL;

const char* VDC_IplVersion();
int VDC_IplRevision();

PST_VDC_IPL VDC_IplCreate(PST_VDC_CFG cfg);
void VDC_IplDelete(PST_VDC_IPL impl);

void VDC_IplRun(PST_VDC_IPL impl, int ms);
void VDC_IplStop(PST_VDC_IPL impl);

utl_httpc_t* VDC_IplGetHttpc(PST_VDC_IPL impl);

EN_UTL_CONN_TYPE vdcIplMsgGetConnType(const char* msg, const char* pstr);
int VDC_TLV_Run_RecvMsg(PST_VDC_IPL ipl, int tmo);
int VDC_TLV_VRS_SendData(PST_VDC_IPL ipl);
void VDC_TLV_RemDialFail(void* pvImpl);
void VDC_TLV_SvrErrProce(void* pvImpl, int cause);
void vdcIplRemoteSimOnlineRilProce(void* pImpl);
void vdcIplSvrCmdSleepProce(void* pImpl, int timeout);
void vdcIplUserChangeRemoteSim(void* data, int firstmno);
#ifdef VAD_UI_ENABLED
int vdcIplUiSockGetDevInfo(PST_VDC_IPL impl, char* buf, int buf_len, char* seq_str, const char* msg_type, int key_type);
void vdcIplUISocketTestCmd(PST_VDC_IPL ipl, int testcmd);
#endif

#ifdef __cplusplus
}
#endif

#endif


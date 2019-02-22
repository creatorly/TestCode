/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vfd_httpd.h
* PACKAGE:      
*   Header of:          vfd_httpd.c
*   Date:               2015.02.02
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VFD_HTTPD_H__
#define  __VFD_HTTPD_H__

#include <utl_typdef.h>
#include <vad_config.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VFD_HTTPD_VAD_TYPE          "vad"

#ifdef ANDROID
#define VFD_HTTPD_BIN_TYPE          "vad_apk"
#elif ((ACOM_BOARD_TYPE == MDM9X07) && (ACOM_BOARD_SUBTYPE == MDM_01))
#define VFD_HTTPD_BIN_TYPE          "vad_mdm01"
#elif ((ACOM_BOARD_TYPE == MDM9X07) && (ACOM_BOARD_SUBTYPE == MDM_02))
#define VFD_HTTPD_BIN_TYPE          "vad_mdm02"
#elif ((ACOM_BOARD_TYPE == MDM9X07) && (ACOM_BOARD_SUBTYPE == MDM_03))
#define VFD_HTTPD_BIN_TYPE          "vad_mdm03"
#else
#define VFD_HTTPD_BIN_TYPE          VFD_HTTPD_VAD_TYPE
#endif

#define VFD_HTTPD_UPG_REASON_LOWBAT "Battery is lower than "UTL_STR(VAD_UPGRADE_MIN_BATTERY)"!"

typedef struct tagstVfdHttpDaemon* PST_VFD_HTTPD;

/* the default action is reboot the system */
typedef int (*FP_VFD_UPDATE_DONE)(const char* update_file, char* errstr, int size, void* usr_data);

/* the function to execute a system command */
typedef int (*FP_VFD_EXEC_CMD)(const char* cmds, char* outbuf, int osize, char* errbuf, int esize);

/* used to unlock and lock the flash when saving updated files */
typedef int (*FP_VFD_FLASH_LOCK)();
typedef int (*FP_VFD_FLASH_UNLOCK)();

PST_VFD_HTTPD VFD_HttpdCreate(int port, const char* mmap_file, const char* mmap_ffile);
void VFD_HttpdDelete(PST_VFD_HTTPD htpd);

void VFD_HttpdSetHomeDir(PST_VFD_HTTPD htpd, const char* home_dir);
void VFD_HttpdSetHtmlDir(PST_VFD_HTTPD htpd, const char* html_dir);
void VFD_HttpdSetListUrl(PST_VFD_HTTPD htpd, const char* list_url);
void VFD_HttpdSetDefaultPage(PST_VFD_HTTPD htpd, const char* dflt_page);
void VFD_HttpdSetProgressUrl(PST_VFD_HTTPD htpd, const char* pro_url);
void VFD_HttpdSetUplogUrl(PST_VFD_HTTPD htpd, const char* uplog_url);

void VFD_HttpdSetLocFile(PST_VFD_HTTPD htpd, const char* type, const char* loc_file);

void VFD_HttpdSetHookUpdateDone(PST_VFD_HTTPD htpd, FP_VFD_UPDATE_DONE fpDone, void* usrData);
void VFD_HttpdSetHookFlash(PST_VFD_HTTPD htpd, FP_VFD_FLASH_LOCK fpLock, FP_VFD_FLASH_UNLOCK fpUnlock);
void VFD_HttpdSetHookExecCmd(PST_VFD_HTTPD htpd, FP_VFD_EXEC_CMD fpExecCmd);

int VFD_HttpdStart(PST_VFD_HTTPD htpd);

int VFD_HttpdVerifyFile(const char* type, const char* data, int len, const char** reason);

/*******************************************************************************************************/
int VFD_System(const char* cmd, char* obuf, int size);

#ifdef __cplusplus
}
#endif

#endif


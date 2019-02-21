
/********************************************************
* 
*
* Ejoin SIM Cloud platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:                 ut_fselect.h
* PACKAGE:      
*   Header of:          ut_fselect.c
*   Date:               2018.08.08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UT_FSELECT_H__
#define __UT_FSELECT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef _INT32 (*FP_UT_FSELECT_READ)(_INT32 fd, const _CHAR8* file, _VOID* usrData);

_INT32 UT_FSelectInit(_INT32 priority, _INT32 stackSize);
_VOID  UT_FSelectFree();

_INT32 UT_FSelectAddFd(_INT32 fd, FP_UT_FSELECT_READ fpRead, _VOID* usrData);
_INT32 UT_FSelectDelFd(_INT32 fd);

_INT32 UT_FSelectAddFile(const _CHAR8* file, FP_UT_FSELECT_READ fpRead, _VOID* usrData);
_INT32 UT_FSelectDelFile(const _CHAR8* file);

_INT32 UT_FSelectStart(const _CHAR8* name);

#ifdef __cplusplus
}
#endif

#endif


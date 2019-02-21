/********************************************************
* 
*
* xp-workroom CCXML platform
* Copyright (C) 2013-2013, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         stdbool.h
* PACKAGE:      libutl
*   Header of:          
*   Date:               06/01/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_STDBOOL_H__
#define  __UTL_STDBOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef bool
typedef int bool;
#endif

#ifndef true
#define true    1
#endif

#ifndef false
#define false   0
#endif

#ifdef __cplusplus
}
#endif

#endif


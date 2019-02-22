/********************************************************
* Ejoin utils platform
* Copyright (C) 2017-2017, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         vwa_ver.h
* PACKAGE:      
*   Date:               2017.03.28
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VWA_VER_H__
#define  __VWA_VER_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * 1.0.0    the first version of vwa
 * 2.1.1    fixed the issue of crash if getting html package timeout
 *     2    fixed the issue of crash if the authenticating timeout
 *     3    fixed the issue of always print log to stdout not in daemon mode
 * 2.2.4    add the feature of AD pushing
 **************************************************************************/

#define VWA_VERSION     "2.2"
#define VWA_BLD_NUM     4
#define VWA_SVN_VER     5482

/* the version information will be saved to the file */
#define VWA_VER_FILE    "/opt/ejoin/var/vwa.ver"

#if defined(__cplusplus)
}
#endif

#endif


/********************************************************
* 
*
* Ejoin utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         vds_ver.h
* PACKAGE:      
*   Date:               2016.03.09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VDS_VER_H__
#define  __VDS_VER_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * 1.0.1    the first version of vds
       2    change the log directory to "/opt/ejoin/log/vds/"
       3    fixed the issue failure to close the opened session
   1.1.4    add the feature of log uploading
 **************************************************************************/

#define VDS_VERSION     "1.1"
#define VDS_BLD_NUM     6
#define VDS_SVN_VER     2131

/* the version information will be saved to the file */
#define VDS_VER_FILE    "/opt/ejoin/var/vds.ver"

#if defined(__cplusplus)
}
#endif

#endif



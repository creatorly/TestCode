/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_idmgr.h
* PACKAGE:      libeut
*   Header of:          utl_idmgr.c
*   Date:               09/04/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_IDMGR_H__
#define  __UTL_IDMGR_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* utl_idmgr_h;

#define UTL_INVALID_ID  (-1)

/**
 * INITIALIZE a id manager
 * @param pIdMgr:   the id manager should be initialized
 * @param base_id:  the base id managed
 * @param n:        the numbers of id managed
 *
 * @return:         0 if successful, negative otherwise
 */
int utl_idmgr_init(utl_idmgr_h* pIdMgr, int base_id, int n);
/**
 * FREE a id manager
 * @param pIdMgr:   the id manager should be free
 */
void utl_idmgr_free(utl_idmgr_h idmgr);
/**
 * GET the size of a id manager
 * @param pIdMgr:   the id manager to be worked on
 * 
 * @return:         the size of the id manager
 */
int utl_idmgr_size(utl_idmgr_h idmgr);
/**
 * SHOW a id manager
 * @param pIdMgr:   the id manager should be show
 * @param fpShow:   the function to show the statistics
 * @param fd:       the file description or buffer to be print
 */
int utl_idmgr_show(utl_idmgr_h idmgr, 
    int (*fpShow)(void*, const char*, ...), void* fd);

/**
 * ALLOCATE/SPECIFY a id from a id manager
 * @param idmgr:    the id manager to be worked on
 * @param id:       the id specified, -1 means not specified
 *
 * @return:         the id allocated if successful, -1 otherwise
 */
int utl_id_alloc(utl_idmgr_h idmgr, int id);

/**
 * FREE a id to a id manager
 * @param idmgr:    the id manager worked on
 * @param id:       the id should be free
 */
void utl_id_free(utl_idmgr_h idmgr, int id);

/**
 * CHECK if a id is valid
 * @param idmgr:    the id manager worked on
 * @param id:       the id should be checked
 *
 * @return:         true if valid, or false
 */
bool utl_id_isvalid(utl_idmgr_h idmgr, int id);

#ifdef __cplusplus
}
#endif

#endif


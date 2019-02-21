/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_ptr.h
* PACKAGE:      libeut
*   Header of:          utl_ptr.c
*   Date:               2016.12.22
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_PTR_H__
#define  __UTL_PTR_H__

#include <string.h>
#include "utl_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlPointer utl_ptr_t;

utl_ptr_t* utl_ptr_create(void* data, void (*fpFree)(void* data));
void utl_ptr_delete(utl_ptr_t* ptr);

utl_ptr_t* utl_ptr_get(utl_ptr_t* ptr);

/* return the reference count after the PUT operation */
int utl_ptr_put(utl_ptr_t* ptr);

int utl_ptr_get_cnt(utl_ptr_t* ptr);

/********************************************************************
 * The data of the referenced pointer MAY be deleted, 
 * you SHOULD tell if it is NULL before using the data got by utl_ptr_data() 
 */
utl_ptr_t* utl_ptr_ref(utl_ptr_t* ptr);

/* return the reference count after the REF operation */
int utl_ptr_unref(utl_ptr_t** pptr);

int utl_ptr_ref_cnt(utl_ptr_t* ptr);

void* utl_ptr_data(utl_ptr_t* ptr);

#ifdef __cplusplus
}
#endif

#endif


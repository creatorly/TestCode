/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2007-2007, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_objs.h
* PACKAGE:      libeut
*   Header of:          utl_objs.c
*   Date:               09/17/07
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_OBJS_H__
#define  __UTL_OBJS_H__

#include <string.h>
#include "utl_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlObjectSet utl_objs_t;

utl_objs_t* utl_objs_create(int maxObjTypes, int initObjNum);
void utl_objs_destroy(utl_objs_t* objs);

/* free all the objects added and then destroy the objs */
void utl_objs_free(utl_objs_t* objs, void (*fpObjFree)(void* pObj));

int utl_objs_size(utl_objs_t* objs);

/* return the object id if successful, or -1 if failed */
int utl_objs_add(utl_objs_t* objs, int type, void* obj);

/* return the object deleted from the objs table */
void* utl_objs_del(utl_objs_t* objs, int obj_id);

/* get a object id by specified type */
int utl_objs_get_id(utl_objs_t* objs, int type);

/* return the object id if successful, or -1 if failed */
int utl_objs_add_id(utl_objs_t* objs, int obj_id, void* obj);

void* utl_objs_find(utl_objs_t* objs, int obj_id);

int utl_objs_get_type(utl_objs_t* objs, int obj_id);

void utl_objs_for_each(utl_objs_t* objs, 
        void (*doeach)(int obj_id, void* obj, void* data), void* each_data);

void utl_objs_for_each_if(utl_objs_t* objs, 
        bool (*cond)(int obj_id, void* obj, void* data), void* cond_data,
        void (*doeach)(int obj_id, void* obj, void* data), void* each_data);

#ifdef __cplusplus
}
#endif

#endif


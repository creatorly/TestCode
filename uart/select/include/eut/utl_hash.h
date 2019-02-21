/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2007-2007, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_hash.h
* PACKAGE:      libeut
*   Header of:          utl_hash.c
*   Date:               09/17/07
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_HASH_H__
#define  __UTL_HASH_H__

#include <string.h>
#include "utl_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlHash utl_hash_t;

typedef struct tagstUtlHashIterator
{
    utl_hash_t* table;
    utl_node_t* node;
    int index;
    int count;
}utl_hash_it_t;

typedef int      (*utl_hash_cmp_func)(const void*, const void*);
typedef unsigned (*utl_hash_hsh_func)(const void*);
typedef void     (*utl_hash_del_func)(void*);

typedef void      (*utl_hash_do_func)(const void* key, const void* val, const void* do_data);
typedef bool      (*utl_hash_cond_func)(const void* key, const void* val, const void* cond_data);

typedef int      (*utl_hash_print_func)(void*, const char*, ...);
    
utl_hash_t* utl_hash_create(utl_hash_cmp_func key_cmp, 
                            utl_hash_hsh_func key_hsh, 
                            utl_hash_del_func key_del, 
                            utl_hash_del_func dat_del, 
                            unsigned size);
void utl_hash_destroy(utl_hash_t* table, int del);

int utl_hash_size(utl_hash_t* table);
int utl_hash_insert(utl_hash_t* table, const void* key, const void* val, int overwrite);
/* return the value deleted from the hash table */
void* utl_hash_remove(utl_hash_t* table, const void* key, int del);
void* utl_hash_search(utl_hash_t* table, const void* key);

void utl_hash_for_each(utl_hash_t* table, utl_hash_do_func do_func, void* do_data);

/**
 * TRAVERSE(loop) the hash table with a given condition matched
 * @param table:    The hash table to be worked on
 * @param do_func:  The function to do something when hash node matched the given condition
 * @param do_data:  The data used by function do_func
 * @param cond_func: The function to tell something
 * @param cond_data: The data used by function cond_func
 *
 * @return:         the matched hash nodes
 */
int utl_hash_for_each_if(utl_hash_t* table, 
                         utl_hash_do_func do_func, void* do_data, 
                         utl_hash_cond_func cond_func, void* cond_data);

void utl_hash_remove_val(utl_hash_t* table, const void* val, int del);

void utl_hash_show(utl_hash_t* table, utl_hash_print_func print_func, void* fd);

#define utl_hash_int_create(size) \
            utl_hash_create(utl_hash_intcmp, utl_hash_inthsh, NULL, NULL, size)
#define utl_hash_str_create(size) \
            utl_hash_create(utl_hash_strcmp, utl_hash_strhsh, NULL, NULL, size)

int utl_hash_intcmp(const void* pi1, const void* pi2);
unsigned utl_hash_inthsh(const void* pi);

#define utl_hash_strcmp ((utl_hash_cmp_func)strcmp)
unsigned utl_hash_strhsh(const void* str);

void* utl_hash_get_first(utl_hash_t* table, utl_hash_it_t* it);
void* utl_hash_get_next(utl_hash_it_t* it);

bool utl_hash_it_eoh(utl_hash_it_t* it);
void* utl_hash_it_key(utl_hash_it_t* it);
void* utl_hash_it_val(utl_hash_it_t* it);
void utl_hash_it_remove(utl_hash_it_t* it, int del);

#ifdef __cplusplus
}
#endif

#endif


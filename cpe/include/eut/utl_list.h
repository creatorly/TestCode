/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_list.h
* PACKAGE:      libeut
*   Header of:          utl_list.c
*   Date:               09/04/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_LIST_H__
#define  __UTL_LIST_H__

#include <stdbool.h>
#include <stdarg.h>

#include "utl_typdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef UTL_LIST_DEBUG

typedef struct tagstUtlNode        
{
    struct tagstUtlNode *next;
    struct tagstUtlNode *prev;
}utl_node_t;

typedef struct tagstUtlList
{
    utl_node_t node;
    size_t size;
}utl_list_t;

typedef bool (*utl_list_cond_f)(utl_node_t*, void*);
typedef void (*utl_list_do_f)(utl_node_t*, void*);
typedef void (*utl_list_free_f)(utl_node_t*);
typedef int (*utl_list_cmp_f)(utl_node_t* pn1, utl_node_t* pn2);

typedef int (*utl_list_tidself_f)();
typedef char* (*utl_list_taskname_f)(int tid);
typedef int (*utl_list_dump_f)(const void* fd, const char* fmt, ...);

#define utl_list_init(lst) \
DO{ \
    (lst)->node.next = &(lst)->node; \
    (lst)->node.prev = &(lst)->node; \
    (lst)->size = 0; \
}WHILE(0)

void utl_list_free(utl_list_t* ls, utl_list_free_f fpFree);

#define UTL_LIST_FREE(ls, fp)  \
            utl_list_free(ls, (utl_list_free_f)fp)

#define utl_list_size(lst)  ((lst)->size)
#define utl_list_begin(lst) ((lst)->node.next)
#define utl_list_end(lst)   (&(lst)->node)     /* iter of list end, not a real node */
#define utl_list_next(node) ((node)->next)
#define utl_list_prev(node) ((node)->prev)

#define utl_list_first(lst)  ((lst)->node.next)
#define utl_list_last(lst)  ((lst)->node.prev)

#define utl_list_eol(ls, node) \
            (((utl_node_t*)(node)) == utl_list_end(ls))

#define utl_list_first_t(ls, T) ((T*)utl_list_begin(ls))
#define utl_list_last_t(ls, T) ((T*)utl_list_last(ls))

#define utl_list_next_t(node, T) \
            ((T*)utl_list_next((utl_node_t*)(node)))
#define utl_list_prev_t(node, T) \
            ((T*)utl_list_prev((utl_node_t*)(node)))

/* for debug */
void utl_list_set_task(const char* tname, int (*fpTidSelf)());
int utl_list_dump(const void* fd, utl_list_dump_f fpDump);

#ifdef UTL_LIST_DEBUG
void utl_list_add_front(utl_list_t* ls, utl_node_t* node);
void utl_list_add_ex(utl_list_t* ls, utl_node_t* node, const char* file, int line);
void utl_list_ins(utl_list_t* ls, utl_node_t* prev, utl_node_t* node);
void utl_list_del_ex(utl_list_t* ls, utl_node_t* node, const char* file, int line);
void utl_list_swap(utl_list_t* ls1, utl_list_t* ls2);
void utl_list_concat(utl_list_t* ls_dst, utl_list_t* ls_src);

#define utl_list_add(l, n)  utl_list_add_ex(l, n, __FILE__, __LINE__)
#define utl_list_del(l, n)  utl_list_del_ex(l, n, __FILE__, __LINE__)
#else
void utl_list_add_front(utl_list_t* ls, utl_node_t* node);
void utl_list_add(utl_list_t* ls, utl_node_t* node);
void utl_list_ins(utl_list_t* ls, utl_node_t* prev, utl_node_t* node);
void utl_list_del(utl_list_t* ls, utl_node_t* node);
void utl_list_swap(utl_list_t* ls1, utl_list_t* ls2);
void utl_list_concat(utl_list_t* ls_dst, utl_list_t* ls_src);
#endif

/* sort by ASC defaultly */
void utl_list_sort(utl_list_t* ls, utl_list_cmp_f fpCmp);

utl_node_t* utl_list_get(utl_list_t* ls);
utl_node_t* utl_list_get_bypos(utl_list_t* ls, int n);

utl_node_t* utl_list_findif(utl_list_t* ls,
                            utl_list_cond_f fpCond, void* key);

void utl_list_foreach(utl_list_t* ls, 
                      utl_list_do_f fpDo, void* data);

int utl_list_foreach_if(utl_list_t* ls, 
                        utl_list_cond_f fpCond, void* key,
                        utl_list_do_f fpDo, void* data);

#ifdef __cplusplus
}
#endif

#endif


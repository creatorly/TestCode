/********************************************************
* 
*
* Ejoin SIP Load Balancor
* Copyright (C) 2007-2007, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_cfg.h
* PACKAGE:      libeut
*   Header of:          utl_cfg.cpp
*   Date:               09/07/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_CFG_H__
#define __UTL_CFG_H__

#include <unistd.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UTL_CFGTYPE_NULL = 0,
        
    UTL_CFGTYPE_INT,
    UTL_CFGTYPE_STR,
    UTL_CFGTYPE_REC,

    UTL_CFGTYPE_COUNT
}utl_cfg_type_t;

typedef struct tagstUtlCfgDesc
{
    int id;
    utl_cfg_type_t type;
    const char* name;
    const char* val;
}utl_cfg_desc_t;

typedef void* utl_cfg_h;

typedef int (*utl_cfg_set_f)
    (int id, int index, int iVal, char* pcVal, void* data);
typedef utl_cfg_set_f utl_cfg_add_f;
typedef utl_cfg_set_f utl_cfg_del_f;

/* if the path of the filename not exists, make it recursively */
int utl_cfg_init(utl_cfg_h* p_cfg, 
                 const char* filename, 
                 utl_cfg_desc_t* cfgtbl, 
                 size_t size);

int utl_cfg_free(utl_cfg_h cfg);

int utl_cfg_save_ifnone(utl_cfg_h hCfg);
int utl_cfg_save(utl_cfg_h cfg);
int utl_cfg_load(utl_cfg_h cfg);

int utl_cfg_get(utl_cfg_h cfg, int id, int index, int* piVal, char* pcVal);
int utl_cfg_set(utl_cfg_h cfg, int id, int index, int iVal, const char* pcVal);

/* for the RECORD configuration */
int utl_cfg_add(utl_cfg_h cfg, int id, int* pIdx, int iVal, const char* pcVal);
int utl_cfg_del(utl_cfg_h cfg, int id, int index);
int utl_cfg_clr(utl_cfg_h cfg, int id);

#define utl_cfg_get_int(cfg, id, pval) \
            utl_cfg_get(cfg, id, 0, pval, NULL)
#define utl_cfg_get_str(cfg, id, buf) \
            utl_cfg_get_str_r(cfg, id, 0, buf, sizeof(buf))

int utl_cfg_get_str_r(utl_cfg_h cfg, int id, int index, char* buf, int size);

utl_cfg_type_t utl_cfg_type(utl_cfg_h cfg, int id);
int utl_cfg_size(utl_cfg_h cfg, int id);
int utl_cfg_count(utl_cfg_h cfg, int id);
bool utl_cfg_valid(utl_cfg_h cfg, int id, int index);
int utl_cfg_next_n(utl_cfg_h cfg, int id, int index, int n);

int utl_cfg_set_cbset(utl_cfg_h cfg, int id, utl_cfg_set_f fpSet, void* data);
int utl_cfg_set_cbadd(utl_cfg_h cfg, int id, utl_cfg_add_f fpAdd, void* data);
int utl_cfg_set_cbdel(utl_cfg_h cfg, int id, utl_cfg_del_f fpDel, void* data);

utl_cfg_desc_t* utl_cfg_get_desc(utl_cfg_h cfg, int pos);
int utl_cfg_get_filename(utl_cfg_h cfg, char* filename, int size);

int utl_cfg_mkdir(const char* filename, char* validname, size_t size);

#ifdef __cplusplus
}
#endif

#endif


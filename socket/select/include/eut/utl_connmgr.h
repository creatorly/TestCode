/********************************************************
* 
*
* xp-workroom utils
* Copyright (C) 2011-2011, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         utl_connmgr.h
* PACKAGE:      
*   Header of:          utl_connmgr.cpp
*   Date:               11/18/2011
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_CONNMGR_H__
#define  __UTL_CONNMGR_H__

#include <utl_select.h>
#include <utl_timer.h>
#include <utl_conn.h>
#include <utl_transaction.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlConnectionManager utl_connmgr_t;
typedef struct tagstUtlConnMultiplexGroup utl_connmux_grp_t;

utl_connmgr_t* utl_connmgr_create(utl_select se, utl_timers_t* timers);
void utl_connmgr_destroy(utl_connmgr_t* pcm);

/**
 * SET the external select for a connection manager
 * @param pcm:      the connection manager to be worked
 * @param sel:      the external select to be set
 */
void utl_connmgr_set_select(utl_connmgr_t* pcm, utl_select sel);

/**
 * SET the external timer group for a connection manager
 * @param pcm:      the connection manager to be worked
 * @param timers:   the external timer group to be set
 */
void utl_connmgr_set_timers(utl_connmgr_t* pcm, utl_timers_t* timers);

void utl_connmgr_set_ids(utl_connmgr_t* pcm, int cid, int mask);
void utl_connmgr_set_keepalive(utl_connmgr_t* pcm, int idle, int intvl, int cnt);

int utl_connmgr_add_ports(utl_connmgr_t* pcm, int start_port, int end_port);
int utl_connmgr_del_ports(utl_connmgr_t* pcm, int start_port, int end_port);

/****************************************************************************************************
 * SET the max number of multiplex connections on a base connection
 * @param pcm:      to which the connection is released
 * @param max_mux:  the max number to be set
 */
void utl_connmgr_set_maxmux(utl_connmgr_t* pcm, int max_mux);

/**
 * SET the keepalive interval from multiplex connection
 * @param pcm:      to which the connection is released
 * @param keepintvl:  the keepalive interval(in seconds) to be set
 */
void utl_connmgr_set_mux_keepintvl(utl_connmgr_t* pcm, int keepintvl);

/**
 * ALLOCATE a multiplex connection group from a manager
 * @param pcm:      from which the connection is allocated
 * @param max_mux:  the maximum of multiplex connections contained in this group
 * @param host:     the local host, NULL means any local host like "0.0.0.0"
 * @param port:     the local port, 0 means random and -1 means auto-allocate
 *
 * @return:         the multiplex connection group allocated or NULL if failed
 */
utl_connmux_grp_t* utl_conn_muxgrp_alloc(utl_connmgr_t* pcm, int max_mux, const char* host, int port);

/**
 * FREE a multiplex connection group
 * @param muxgrp:   the multiplex connection group to be released
 *                  
 */
void utl_conn_muxgrp_free(utl_connmux_grp_t* muxgrp);

/**
 * ALLOCATE a multiplex connection from a group
 * @param muxgrp:   the multiplex connection group to be worked
 * @param channel:  the channel specified(begin with zero)
 *
 * @return:         the connection allocated or NULL if failed
 */
utl_conn_t* utl_connmux_alloc(utl_connmux_grp_t* muxgrp, int channel);

/**************************************************************************************************
 * DEFINE the connotr manager functions
 */
typedef struct tagstUtlConnOtrManger utl_connotr_mgr_t;

/**
 * CREATE a connotr mananger based on a conn manager and a transaction manager
 * @param pcm:      from which the connection will be allocated
 * @param trm:      the transaction manager on which the connotr manager will be built
 * @param bgn_port: the begin port used by connotr manger
 * @param end_port: the end port used by connotr manger
 *
 * @return:         the connotr manager created or NULL if failed
 */
utl_connotr_mgr_t* utl_connotr_mgr_create(utl_connmgr_t* pcm, 
                                          utl_trm_t* trm, 
                                          unsigned short bgn_port, 
                                          unsigned short end_port);
/**
 * DESTROY a connotr mananger
 * @param pOtrMgr:  the connotr manager should be destroyed
 *
 */
void utl_connotr_mgr_destroy(utl_connotr_mgr_t* pOtrMgr);

/**
 * ALLOCATE a connection from a connotr manager
 * @param muxgrp:   the multiplex connection group to be worked
 * @param channel:  the channel specified(begin with zero)
 *
 * @return:         the connection allocated or NULL if failed
 */
utl_conn_t* utl_connotr_alloc(utl_connotr_mgr_t* pOtrMgr);

/**************************************************************************************************
 * ALLOCATE a utils connection from a manager
 * @param pcm:      from which the connection is allocated
 * @param type:     the connection type defined by enum EN_UTL_CONN_TYPE
 * @param host:     the local host, NULL means any local host like "0.0.0.0"
 * @param port:     the local port, 0 means random and -1 means auto-allocate
 *
 * @return:         the connection allocated or NULL if failed
 */
utl_conn_t* utl_conn_alloc(utl_connmgr_t* pcm, int type, const char* host, int port);

/**
 * FREE a utils connection to a manager
 * @param pcm:      to which the connection is released
 * @param cid:      the id of the connection to be released, got by utl_conn_get_di()
 *                  
 */
void utl_conn_free(utl_connmgr_t* pcm, int cid);

/**
 * RELEASE a utils connection, same as utl_conn_free() but NOT SAFE
 * @param pCon:     the connection to be released
 *
 * NOTE:    user MUST ENSURE the connection is not freed
 */
void utl_conn_release(utl_conn_t* pCon);

utl_conn_t* utl_conn_find(utl_connmgr_t* pcm, int cid);

#ifdef __cplusplus
}
#endif

#endif


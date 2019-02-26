/******************************************************************************
                Copyright (c) 2015-2015 Ejoin-Tech Inc.
                
Filename   :    utl_httpd.h
Description:    define the interface of ccm http server
Revision:       1.0
Author:         pengjian
Date:           2015.12.26
******************************************************************************/

#ifndef __UTL_HTTPD_H__
#define __UTL_HTTPD_H__

#include <utl_select.h>
#include <utl_timer.h>
#include <utl_connmgr.h>
#include <utl_http_msg.h>
#include <utl_tls.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_HTTPD_MAX_LISTEN_ADDRS  10

typedef struct tagstUtlHttpDaemon utl_httpd_t;
typedef struct tagstUtlHttpDaemonSession utl_httpd_sess_t;

/* a new partial HTTP request with headers is received */
typedef int (*FP_UTL_HTTPD_SESS_PRE)(utl_httpd_sess_t* sess, const utl_http_msg_t* req, const void* udat);
/* a new HTTP request is received with a opened http session */
typedef int (*FP_UTL_HTTPD_SESS_NEW)(utl_httpd_sess_t* sess, utl_http_msg_t* req, const void* udat);
/* (NOT IMPLEMENT) subsequent chunked data is received. zero length of data means no data will be received. */
typedef int (*FP_UTL_HTTPD_SESS_DAT)(utl_httpd_sess_t* sess, const char* data, int len, const void* udat);
/* a HTTP session is closed by remote, normally you SHOULD close the sessoin in this invoking */
typedef int (*FP_UTL_HTTPD_SESS_DEL)(utl_httpd_sess_t* sess, int code, const char* reason, const void* udat);

utl_httpd_t* utl_httpd_create(const char* host, int port,
                              FP_UTL_HTTPD_SESS_NEW fpNew, 
                              FP_UTL_HTTPD_SESS_DAT fpDat,      /* (NOT IMPLEMENT now) */
                              FP_UTL_HTTPD_SESS_DEL fpDel, 
                              void* usr_dat);

utl_httpd_t* utl_httpd_create_qlen(const char* host, int port, int quelen,
                                   FP_UTL_HTTPD_SESS_NEW fpNew, 
                                   FP_UTL_HTTPD_SESS_DAT fpDat,      /* (NOT IMPLEMENT now) */
                                   FP_UTL_HTTPD_SESS_DEL fpDel, 
                                   void* usr_dat);

void utl_httpd_delete(utl_httpd_t* httpd);

/**
 * SET the external select for a http daemon
 * @param httpc:    the http daemon to be worked
 * @param sel:      the external select to be set
 */
void utl_httpd_set_select(utl_httpd_t* httpd, utl_select sel);

/**
 * SET the external timer group for a http daemon
 * @param httpc:    the http daemon to be worked
 * @param timers:   the external timer group to be set
 */
void utl_httpd_set_timers(utl_httpd_t* httpd, utl_timers_t* timers);

/**
 * SET the external connection manager for a http daemon
 * @param httpc:    the http daemon to be worked
 * @param conmgr:   the external connection manager to be set
 */
void utl_httpd_set_conmgr(utl_httpd_t* httpd, utl_connmgr_t* conmgr);

/**
 * ADD a http/https listen address
 * @param httpd:    The http daemon handle to be worked on
 * @param loc_ip:   The local ip address to be listened
 * @param loc_port: The local port to be listened
 * @param tls_ctx:  The tls contex for https, NULL means http
 *
 * return:      0 if successful, a error code otherwise
 */
int utl_httpd_add_listen_addr(utl_httpd_t* httpd, const char* loc_ip, int loc_port, utl_tls_ctx_t* tls_ctx);

void utl_httpd_set_max_hdrsize(utl_httpd_t* httpd, int max_hdr_size);

/**
 * SET the maximum of body size of a received http message
 * @param httpd:    The http daemon handle to be worked on
 * @param max_body_size:    The max body size to be set
 *
 * NOTE:    the default value is set to UTL_HTTP_MAX_BODY_SIZE defined in utl_http_msg.h
 */
void utl_httpd_set_max_bodysize(utl_httpd_t* httpd, int max_body_size);

void utl_httpd_set_hook_sess_pre(utl_httpd_t* httpd, FP_UTL_HTTPD_SESS_PRE fpPre);

void utl_httpd_run(utl_httpd_t* httpd, int ms);

/* get the session progress in percent for a incoming POST */
int utl_httpd_sess_get_progress(utl_httpd_sess_t* sess);

void utl_httpd_sess_get_caddr(utl_httpd_sess_t* sess, char cip[32], int* cport);
void utl_httpd_sess_set_keepalive(utl_httpd_sess_t* sess, int timeout, int max);

/* send a HTTP response without a body */
int utl_httpd_sess_answer(utl_httpd_sess_t* sess, int code, const char* reason);

/* send a HTTP response with 200 OK and a body(type: value of header Content-Type) */
int utl_httpd_sess_answer_body(utl_httpd_sess_t* sess, const char* type, const char* body, int len);

/* send a user-defined HTTP response */
int utl_httpd_sess_send_resp(utl_httpd_sess_t* sess, utl_http_msg_t* resp);

/* Call this funcion to close the httpd session, or another request may be received for a keep-alive session */
void utl_httpd_sess_close(utl_httpd_sess_t* sess);

#ifdef __cplusplus
}
#endif

#endif


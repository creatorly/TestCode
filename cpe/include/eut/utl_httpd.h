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

#include <utl_http_msg.h>

#ifdef __cplusplus
extern "C" {
#endif

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

void utl_httpd_set_max_hdrsize(utl_httpd_t* httpd, int max_hdr_size);
void utl_httpd_set_hook_sess_pre(utl_httpd_t* httpd, FP_UTL_HTTPD_SESS_PRE fpPre);

void utl_httpd_delete(utl_httpd_t* httpd);
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


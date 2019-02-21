/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2018-2018, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_tls.h
* PACKAGE:      libeut
*   Header of:          utl_tls.c
*   Date:               2018.06.05
*   Author:             PengJian
*   Version:            1.0
* 
*********************************************************/

#ifndef  __UTL_TLS_H__
#define  __UTL_TLS_H__

#include <stdbool.h>

#define UTL_TLS_ERROR_NONE          0
#define UTL_TLS_ERROR_UNKNOWN       -1

#define UTL_TLS_ERROR_WANT_READ     2
#define UTL_TLS_ERROR_WANT_WRITE    3
#define UTL_TLS_ERROR_ZERO_RETURN   6

#ifdef __cplusplus
extern "C" {
#endif

typedef void utl_tls_ctx_t;
typedef void utl_tls_t;

utl_tls_ctx_t* utl_tlsc_ctx_new(const char* root_ca_cert, 
                                const char* certfile,
                                const char* keyfile, 
                                const char* password, 
                                bool dtls);

utl_tls_ctx_t* utl_tlss_ctx_new(const char* root_ca_cert, 
                                const char* certfile,
                                const char* keyfile, 
                                const char* password, 
                                const char* dh_pars_file,
                                bool dtls);

void utl_tls_ctx_free(utl_tls_ctx_t* ctx);

utl_tls_t* utl_tls_new(utl_tls_ctx_t* ctx, int sock);
void utl_tls_free(utl_tls_t* tls);

int utl_tls_set_host(utl_tls_t* tls, const char* hostname);

int utl_tls_connect(utl_tls_t* tls);
int utl_tls_accept(utl_tls_t* tls);
int utl_tls_send(utl_tls_t* tls, const char* buf, int len, int* pRes);
int utl_tls_recv(utl_tls_t* tls, char* buf, int size, int* pRes);

#ifdef __cplusplus
}
#endif

#endif


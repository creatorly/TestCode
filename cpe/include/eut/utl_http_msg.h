/******************************************************************************
                Copyright (c) 2014-2014 Ejoin-Tech Inc.
                
Filename   :    utl_http_msg.h
Description:    define the interface of http message
Revision:       1.0
Author:         pengjian
Date:           2015.12.23
******************************************************************************/

#ifndef __UTL_HTTP_MSG_H__
#define __UTL_HTTP_MSG_H__

#include "utl_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_HTTP_USER_AGENT     "UHTTPC/1.0"
#define UTL_HTTP_SERVER         "UHTTPS/1.0"

#define UTL_HTTP_PARSE_INIT         -1
#define UTL_HTTP_PARSE_DONE         0
#define UTL_HTTP_PARSE_HDRS         1
#define UTL_HTTP_PARSE_BODY         2
#define UTL_HTTP_PARSE_FAIL         3

typedef struct tatstUtlHttpUrl
{
    char* path;
    utl_list_t pars;

    size_t len;     /* total length of parameters, used to convert to string */
}utl_http_url_t;

typedef struct tagstUtlHttpMessage
{
    char version[16];
    
    /* req */
    utl_http_url_t url;
    char method[16];
    
    /* resp */
    int status_code;
    char reason_phrase[36];

    /* headers */
    char conn[64];
    char host[64];
    char* user_agent;
    char* server;
    
    char* content_type;
    size_t content_len;

    /* other headers */
    utl_list_t headers;

    /* body */
    char* body;
    size_t blen;

    /* the whole http message */
    char* message;
    size_t mlen;
    
    long mstatus;
}utl_http_msg_t;

int utl_http_msg_init(utl_http_msg_t** pHttp);
void utl_http_msg_free(utl_http_msg_t* http);

int utl_http_msg_parse(utl_http_msg_t* http, const char* msg, int len);

/* you can access the string message via http->message and http->mlen if successful */
int utl_http_msg_to_str(utl_http_msg_t* http);

/* used to get parameter or header from a http message */
const char* utl_http_msg_get_uparam(utl_http_msg_t* http, const char* name);
const char* utl_http_msg_get_header(utl_http_msg_t* http, const char* name);

/* used to get the headers with same name */
const char* utl_http_msg_get_header_n(utl_http_msg_t* http, const char* name, int* pos);

/* used to construct a http message */
int utl_http_msg_set_method(utl_http_msg_t* http, const char* method);
int utl_http_msg_set_status(utl_http_msg_t* http, int code, const char* phrase);
int utl_http_msg_set_url(utl_http_msg_t* http, const char* url);
int utl_http_msg_set_uparam(utl_http_msg_t* http, const char* name, const char* value);

int utl_http_msg_set_host(utl_http_msg_t* http, const char* host);
int utl_http_msg_set_conn(utl_http_msg_t* http, const char* connection);
int utl_http_msg_set_user_agent(utl_http_msg_t* http, const char* ua);
int utl_http_msg_set_server(utl_http_msg_t* http, const char* server);
int utl_http_msg_set_header(utl_http_msg_t* http, const char* name, const char* value);

int utl_http_msg_set_body(utl_http_msg_t* http, const char* type, const char* body, int len);

#ifdef __cplusplus
}
#endif

#endif


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

#include <unistd.h>
#include <utl_list.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_HTTP_USER_AGENT     "UHTTPC/1.0"
#define UTL_HTTP_SERVER         "UHTTPS/1.0"

#define UTL_HTTP_PARSE_INIT         -1
#define UTL_HTTP_PARSE_HDRS         -2
#define UTL_HTTP_PARSE_BODY         -3
#define UTL_HTTP_PARSE_CHUNK_LINE   -4
#define UTL_HTTP_PARSE_CHUNK_DATA   -5

#define UTL_HTTP_PARSE_DONE         0
#define UTL_HTTP_PARSE_FAIL         1

#define UTL_HTTP_MAX_BODY_SIZE      5*1024*1024

typedef struct tatstUtlHttpUrl
{
    char proto[16];
    char* path;
    utl_list_t pars;

    size_t len;     /* total length of parameters, used to convert to string */
}utl_http_url_t;

typedef struct tagstUtlHttpMessagePrivate utl_http_msg_private_t;

typedef struct tagstUtlHttpMessage
{
    char version[16];
    
    /* req */
    char method[16];
    utl_http_url_t url;
    
    /* resp */
    int status_code;
    char reason_phrase[36];

    /* headers */
    char conn[64];
    char host[64];
    char* user_agent;
    char* server;
    
    char* content_type;
    ssize_t content_len;

    /* other headers */
    utl_list_t headers;

    /* body */
    char* body;
    size_t blen;

    /* the whole http message */
    char* message;
    size_t mlen;

    long mstatus;

    /* NOTE: http message private data, DO NOT change it */
    utl_http_msg_private_t* pdata;
}utl_http_msg_t;

int utl_http_msg_init(utl_http_msg_t** pHttp);
void utl_http_msg_free(utl_http_msg_t* http);

int utl_http_msg_parse(utl_http_msg_t* http, const char* msg, int len);

/**
 * PARSE a http message by one or more invoking of this function
 * @param http:     The http message to be worked on
 * @param pData:    The data passed and also store the remain data
 * @param pLen:     The length of passed data and also store the length of remain
 *
 * @return:         The parsing status of this http message
 */
int utl_http_msg_parse2(utl_http_msg_t* http, char** pData, int* pLen);

/* you can access the string message via http->message and http->mlen if successful */
int utl_http_msg_to_str(utl_http_msg_t* http);

const char* utl_http_msg_get_proto(utl_http_msg_t* http);

/* used to get parameter or header from a http message */
const char* utl_http_msg_get_uparam(utl_http_msg_t* http, const char* name);
const char* utl_http_msg_get_header(utl_http_msg_t* http, const char* name);

/**
 * GET a decoded url param value with specified name
 * @param http:     The http message to be worked on
 * @param name:     The specified url param name
 * @param buf:      The buffer used to store the decoded url param value
 * @param size:     The buffer size
 *
 * @return:         The length of param got. 0 means null param, -1 means failed
 */
int utl_http_msg_get_uparam_r(utl_http_msg_t* http, const char* name, char* buf, int size);

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

/**
 * SET the max body size(default is 5M)
 * @param http:         The http message to be worked on
 * @param max_bsize:    The maximum of body size to be set
 *
 * @NOTE:   zero or negative value means unlimited
 */
void utl_http_msg_set_body_size(utl_http_msg_t* http, int max_bsize);

#ifdef __cplusplus
}
#endif

#endif


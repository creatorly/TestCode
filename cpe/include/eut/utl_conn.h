/********************************************************
* 
*
* xp-workroom utils
* Copyright (C) 2011-2011, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         utl_conn.h
* PACKAGE:      
*   Header of:          utl_conn.c
*   Date:               2014.10.22
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_CONN_H__
#define  __UTL_CONN_H__

#include <utl_crypt.h>
#include <utl_addr.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UTL_CONN_TYPE_UDP = 0,
    UTL_CONN_TYPE_TCP,      /* TCP Client connection */
    UTL_CONN_TYPE_TCS,      /* TCP Server connection */
    UTL_CONN_TYPE_UDPTL,
    UTL_CONN_TYPE_HTTPC,    /* HTTP Client connection */
    UTL_CONN_TYPE_HTTPS,    /* HTTP Server connection */
#if 0
    UTL_CONN_TYPE_TLS,      /* TLS Client connection */
#endif

    UTL_CONN_TYPE_MUX,
    UTL_CONN_TYPE_OTR,      /* connection on tcp transaction */

    UTL_CONN_TYPE_MAX
}EN_UTL_CONN_TYPE;

typedef enum
{
    UTL_CONN_PAR_NULL = 0,

    UTL_CONN_PAR_FLAG,          /* interger, the connection flags */
    
    UTL_CONN_PAR_LOCIP,         /* string, char[32] */
    UTL_CONN_PAR_LOCPORT,       /* interger, int */
    UTL_CONN_PAR_REMIP,         /* string, char[32] */
    UTL_CONN_PAR_REMPORT,       /* interger, int */
    UTL_CONN_PAR_REMOTE,        /* utl_addr_t* */
    UTL_CONN_PAR_PUBIP,         /* string, char[32] */
    UTL_CONN_PAR_PUBPORT,       /* interger, int */

    UTL_CONN_PAR_KEEPINTVL,        /* for udp/udptl, is the nat keepalive expires */
    UTL_CONN_PAR_KEEPDATA,      /* keepalive data, only for UDP */
    UTL_CONN_PAR_SOCK,          /* interger, the socket of connection */

    UTL_CONN_PAR_SID,           /* string, session id used in htc connection */

    UTL_CONN_PAR_SNDBUF,        /* interger, the send buffer of socket */
    UTL_CONN_PAR_RCVBUF,        /* interger, the recv buffer of socket */

    UTL_CONN_PAR_TYPE,          /* interger, connection or data type */
    UTL_CONN_PAR_CRYPT_ID,      /* interger, the crypte algorithm id */
    UTL_CONN_PAR_PUBKEY,        /* string, the public key corresponding to the crypte id */

    UTL_CONN_PAR_MSG_CACHES,    /* interger, the cached message count */
    UTL_CONN_PAR_CHN_MODE,      /* interger, the channel mode */

    UTL_CONN_PAR_LINGER,        /* interger, linger timeout in ms */
    UTL_CONN_PAR_NOT_SENT,      /* interger, the bytes of data to be sent */

    UTL_CONN_PAR_PACKETS,       /* the max packets for udptl */
    UTL_CONN_PAR_ACK_TIME,      /* the ack time in ms, ensure small than retrans time */
    UTL_CONN_PAR_RETRANS_TIME,  /* the first retrans time in ms */
    UTL_CONN_PAR_RETRANS_INTVL, /* the retrans interval time in ms */
    UTL_CONN_PAR_RETRANS_COUNT, /* the max retrans count */

    UTL_CONN_PAR_LOC_MPORT,     /* local mport for multiplex connection */
    UTL_CONN_PAR_REM_MPORT,     /* remote mport for multiplex connection */
    UTL_CONN_PAR_KPA_MPORT,     /* keepalive mport for multiplex connection */
    UTL_CONN_PAR_MUXSIZE,       /* multiplex connection size for muxgrp connection */
    
    UTL_CONN_PAR_MAX
}EN_UTL_CONN_PAR;

typedef enum
{
    UTL_CONN_FLAG_PORT_AUTO = 0x1,  /* the local port of connection is auto allocated */
    UTL_CONN_FLAG_PKT = 0x02,       /* packet mode for TCP or TCS */
}EN_UTL_CONN_FLAG;

/* defined the enum for htc/hts connection, no need now */
#if 0
typedef enum
{
    UTL_CONN_CTT_NONE = 0,
    UTL_CONN_CTT_SIGNAL,
    UTL_CONN_CTT_MEDIA,
    UTL_CONN_CTT_LOG,
    UTL_CONN_CTT_DATA,
    UTL_CONN_CTT_TCP,       /* tcp connection to the third-party server */
}EN_UTL_CONN_CTT;

typedef enum
{
    UTL_CONN_ACM_UDP = 1,
    UTL_CONN_ACM_HTTP = 2,
    UTL_CONN_ACM_HTTPS = 4,
    
    UTL_CONN_ACM_AUTO = 7
}EN_UTL_CONN_ACM;           /* the mask of auto connection */

typedef enum
{
    UTL_CONN_QOS_NONE = 0,  /* no network */
    UTL_CONN_QOS_WORST,
    UTL_CONN_QOS_BAD,
    UTL_CONN_QOS_NORMAL,
    UTL_CONN_QOS_GOOD,
    UTL_CONN_QOS_BEST,
}EN_UTL_CONN_QOS;

#endif

/* the connection must be allocated by utl_conn_alloc() and released by utl_conn_free() */
typedef struct tagstUtlConnection utl_conn_t;

typedef void (*FP_UTL_CONN_OPENED)(utl_conn_t* pcon);
typedef void (*FP_UTL_CONN_OPENFAIL)(utl_conn_t* pcon, int status);
typedef void (*FP_UTL_CONN_CLOSED)(utl_conn_t* pcon);

typedef int  (*FP_UTL_CONN_ONRECV)(utl_conn_t* pcon, const char* data, int len, void* usrDat);
typedef void (*FP_UTL_CONN_ONWRITE)(utl_conn_t* pcon, void* usrDat);

int utl_conn_get_id(utl_conn_t* pcon);

void utl_conn_set_data(utl_conn_t* pcon, void* data);
void* utl_conn_get_data(utl_conn_t* pcon);

void utl_conn_set_hook_enc(utl_conn_t* conn, FP_UTL_ENCRYPT fpEnc, 
                           const char* key, int iKeyLen);
void utl_conn_set_hook_dec(utl_conn_t* conn, FP_UTL_DECRYPT fpDec, 
                           const char* key, int iKeyLen);

/* when the connection received data, the function fpRecv will be invoked */
void utl_conn_set_hook_onrcv(utl_conn_t* conn, FP_UTL_CONN_ONRECV fpRecv, void* data);

/* when the connection can be wrote, the function fpWrite will be invoked */
void utl_conn_set_hook_onwrt(utl_conn_t* conn, FP_UTL_CONN_ONWRITE fpWrite, void* data);

int utl_conn_set_remote(utl_conn_t* pcon, const char* host, int port);

void utl_conn_set_remaddr(utl_conn_t* conn, utl_addr_t* remaddr);
utl_addr_t* utl_conn_get_remaddr(utl_conn_t* conn);

/* OPEN a http-tunnel connection by send HTTP GET request */
int utl_conn_open(utl_conn_t* pcon, 
                  FP_UTL_CONN_OPENED fpOpened, 
                  FP_UTL_CONN_OPENFAIL fpOpenFail,
                  FP_UTL_CONN_CLOSED fpClosed);

int utl_conn_close(utl_conn_t* pcon);

/* note: please reserve 4 bytes before data to add length */
int utl_conn_send(utl_conn_t* pcon, const char* data, int len);

int utl_conn_bridge(utl_conn_t* pcon1, utl_conn_t* pcon2);
int utl_conn_unbridge(utl_conn_t* pcon);

int utl_conn_get_par(utl_conn_t* pcon, EN_UTL_CONN_PAR par, void* buf, int size);
int utl_conn_set_par(utl_conn_t* pcon, EN_UTL_CONN_PAR par, const void* val, int len);

#ifdef __cplusplus
}
#endif

#endif


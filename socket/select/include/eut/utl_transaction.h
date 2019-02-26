/********************************************************
* 
*
* Ejoin Utils Library
* Copyright (C) 2014-2014, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         utl_transaction.h
* PACKAGE:      
*   Header of:          utl_transaction.c
*   Date:               2014.11.11
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_TRANSACTION_H__
#define  __UTL_TRANSACTION_H__

#include <utl_addr.h>
#include <utl_crypt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* the request need a ACK message */
#define UTL_TR_FLAG_ACK     0x01
/* the request need to AutoAnswer */
#define UTL_TR_FLAG_AA      0x02
/* the message need to segment */
#define UTL_TR_FLAG_FRAG    0x04

/* if the TCP connection not found, create a new */
#define UTL_TR_FLAG_TCPNEW  0x80000000

/* if UTL_TR_FLAG_TCPNEW is set, a TCP block conn will be created */
#define UTL_TR_FLAG_TCPBLK  0x40000000

typedef enum
{
    UTL_TRM_EVT_NONE = 0,
    UTL_TRM_EVT_CONNECTED,
    UTL_TRM_EVT_DISCONNECTED,
}EN_UTL_TRM_EVT;

typedef enum
{
    UTL_TRC_PTYPE_UDP = 0,      /* the client port is fixed */
    UTL_TRC_PTYPE_UDP_VAR,      /* the client port can be changed when reconnecting */
    UTL_TRC_PTYPE_TCP,          /* TCP, duplex non-block mode */
    UTL_TRC_PTYPE_TCP_BLK,      /* TCP, simplex(one TX socket and one RX socket) block mode */
}EN_UTL_TRC_PTYPE;

#define UTL_TR_TIMEOUT      0

#define UTL_TR_TRING        100
#define UTL_TR_PROCEEDING   180
#define UTL_TR_OK           200
#define UTL_TR_REQ_TIMEOUT  408
#define UTL_TR_SVR_ERROR    500
#define UTL_TR_SVC_UNVAIL   503
#define UTL_TR_SVR_TIMEOUT  508

typedef struct tagstUtlTransactionManager utl_trm_t;

typedef int (*FP_UTL_TR_HOOK_EVT)(int evt, const char* host, int port, void* data);

typedef int (*FP_UTL_TR_HOOK_REQ)(int tid, const char* req, int len, utl_addr_t* from, utl_addr_t* to, void* data);
typedef int (*FP_UTL_TR_HOOK_RSP)(int tid, int status, const char* resp, int len, utl_addr_t* from, void* tr_data, void* data);
typedef int (*FP_UTL_TR_HOOK_TMO)(int tid, utl_addr_t* to, void* tr_data, void* data);

typedef struct tagstUtlTrPipeAddr
{
    int pid;        /* the pipe id, zero means no pipe specified */
    int cid;        /* the connection id, zero means no connection specified */
    
    utl_addr_t raddr;    /* remote address which message is received from or sent to */
}utl_tr_ppaddr_t;

typedef struct tagstUtlTransactionMessage
{
    int status;                 /* response status, >0: response, 0: request, -1: timeout */
    int len;                    /* the length of message data */
    char* msg;                  /* the message data */

    int pid;                    /* the pipe id from which the message is received */
    int cid;                    /* the connection id from which the message is received */
    utl_addr_t* from;           /* the address from which the message is received */
    utl_addr_t* to;             /* the address to which the message is sent */
    
    void* data;                 /* the user data of the transaction */
}utl_tr_msg_t;

/* if HOOK_MSG is set, the HOOK_REQ, HOOK_RSP and HOOK_TMO will be replaced by it */
typedef int (*FP_UTL_TR_HOOK_MSG)(int tid, utl_tr_msg_t* msg, void* data);

typedef enum
{
    UTL_TRX_MTYPE_TMO = 0,      /* Request Timeout */
    UTL_TRX_MTYPE_REQ,          /* Request */
    UTL_TRX_MTYPE_RSP,          /* Response */
}utl_trx_mtype_e;

typedef struct tagstUtlTrxMsg
{
    utl_trx_mtype_e type;       /* transaction message type, defined by enum utl_trx_mtype_e */
    int flags;                  /* message flags */
    
    utl_addr_t addr;            /* the address from which the message is received for REQ/RSP
                                   the address to which the message is sent for TMO */
                                    
    /* the user data for the transaction, may be set by function 
       utl_trm_set_trx_data(), utl_trm_send_req() or utl_trm_send_req_to().
       if not set, the value of data is NULL */
    void* usr_dat;
    
    int tid;                    /* transaction id */
    unsigned short status;      /* response status, only valid for response message */
    unsigned short len;         /* the length of message data */
    char data[0];               /* the message data */
}__attribute__((__packed__)) utl_trx_msg_t;

typedef struct tagstUtlTrConn utl_tr_conn_t;

/**
 * CREATE a transaction manager
 * @param host:     the local ip address to be binded, if null 0.0.0.0 will be used
 * @param port:     the local port to be binded, if 0 a random port will be used
 * 
 * @return:         the created transaction manager
 */
utl_trm_t* utl_trm_create(const char* host, int port);
utl_trm_t* utl_trm_create_retry(const char* host, int* pPort, int retrys);
void utl_trm_destroy(utl_trm_t* pTrm);

void utl_trm_set_ssrc(utl_trm_t* pTrm, int ssrc);
void utl_trm_set_maxspeed(utl_trm_t* pTrm, int max_speed);

int utl_trm_add_svr(utl_trm_t* pTrm, const char* host, int port, int expires, int timeout);
int utl_trm_del_svr(utl_trm_t* pTrm, const char* host, int port, int linger_time);

int utl_trm_add_tcp_svr(utl_trm_t* pTrm, const char* host, int port, int expires);
int utl_trm_listen_addr(utl_trm_t* pTrm, const char* host, int port);

/**
 * GET the first connected server
 * @param pTrm:     the transaction manager to be used
 * @param svr_addr: the server address got
 * 
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int utl_trm_get_svr(utl_trm_t* pTrm, utl_addr_t* svr_addr);


/*************************************************************************************
 * CREATE a connection used to sendrecv transaction messages
 * @param host:     the local ipaddr to be used(NULL and "0.0.0.0" means all local host)
 * @param port:     the local port to be used(0 means random)
 * @param ptype:    the specified protocol type of the connection
 * 
 * @return:         the pointer to the connection if sucessfully, otherwize NULL returned
 */
utl_tr_conn_t* utl_trm_conn_create(const char* host, int port, EN_UTL_TRC_PTYPE ptype);
utl_tr_conn_t* utl_trm_conn_create2(EN_UTL_TRC_PTYPE ptype, const char* host, int port, int expires);
void utl_trm_conn_delete(utl_tr_conn_t* conn);

void utl_trm_conn_set_rem(utl_tr_conn_t* conn, const char* host, int port, int idle_time);
void utl_trm_conn_set_enc(utl_tr_conn_t* conn, FP_UTL_ENCRYPT enc, const void* key, int len);
void utl_trm_conn_set_dec(utl_tr_conn_t* conn, FP_UTL_DECRYPT dec, const void* key, int len);

/*************************************************************************************
 * SET the NAT expires in seconds
 * @param expires:  the nat expires in seconds to be set
 */
void utl_trm_conn_set_natexp(utl_tr_conn_t* conn, int expires);

/*************************************************************************************
 * SET the pipe expires in seconds
 * @param expires:  the pipe expires in seconds to be set
 */
void utl_trm_conn_set_ppexp(utl_tr_conn_t* conn, int expires);

int utl_trm_conn_set_mqsize(utl_tr_conn_t* conn, int mqsize);
void utl_trm_conn_set_maxspeed(utl_tr_conn_t* conn, int max_speed);
void utl_trm_conn_set_kpalive(utl_tr_conn_t* conn, int idle, int intvl, int cnt);

int utl_trm_conn_add(utl_trm_t* pTrm, utl_tr_conn_t* conn);

bool utl_trm_conn_is_connected(utl_tr_conn_t* conn);
utl_addr_t* utl_trm_conn_get_rem(utl_tr_conn_t* conn);

/* the connection MUST has been added to trm */
int utl_trm_conn_send_req(utl_tr_conn_t* conn, const char* req, int len, int flags, void* data);
int utl_trm_conn_send_req_to(utl_tr_conn_t* conn, const char* req, int len, int flags, utl_addr_t* to, void* data);

/*************************************************************************************/

/**
 * TELL if any server is connected
 * @param pTrm:     the transaction manager to be used
 *
 * @return:         true if one server is connected at least, false if none
 */
bool utl_trm_is_connected(utl_trm_t* pTrm);

int utl_trm_set_hook_enc(utl_trm_t* pTrm, FP_UTL_ENCRYPT fpEnc, const void* key, int klen);
int utl_trm_set_hook_dec(utl_trm_t* pTrm, FP_UTL_DECRYPT fpDec, const void* key, int klen);

void utl_trm_set_hook_evt(utl_trm_t* pTrm, FP_UTL_TR_HOOK_EVT fpEvt, void* data);

/**
 * SET the hook when a request received
 * @param pTrm:     the transaction manager to be used
 * @param fpReq:    the hook invoked when a request is received
 * @param data:     the user data will be passed to fpReq
 */
void utl_trm_set_hook_req(utl_trm_t* pTrm, FP_UTL_TR_HOOK_REQ fpReq, void* data);
/**
 * SET the hook when a response received
 * @param pTrm:     the transaction manager to be used
 * @param fpRsp:    the hook invoked when a response is received
 * @param data:     the user data will be passed to fpRsp
 */
void utl_trm_set_hook_rsp(utl_trm_t* pTrm, FP_UTL_TR_HOOK_RSP fpRsp, void* data);
/**
 * SET the hook when a request timeout
 * @param pTrm:     the transaction manager to be used
 * @param fpTmo:    the hook invoked when timeout
 * @param data:     the user data will be passed to fpRsp
 */
void utl_trm_set_hook_tmo(utl_trm_t* pTrm, FP_UTL_TR_HOOK_TMO fpTmo, void* data);

/**
 * SET the hook when a request or response received or request timeout
 * @param pTrm:     the transaction manager to be used
 * @param ptype:    the specified user proto type
 * @param fpMsg:    the hook invoked when req/rsp received or req timeout
 * @param data:     the user data will be passed to fpMsg
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int utl_trm_set_hook_msg(utl_trm_t* pTrm, int ptype, FP_UTL_TR_HOOK_MSG fpMsg, void* data);

/**
 * SET the hooks for a specified user proto
 * @param pTrm:     the transaction manager to be used
 * @param ptype:    the specified user proto type
 * @param data:     the user data will be passed to fpReq, fpRsp or fpTmo
 * @param fpReq:    the hook invoked when a request is received
 * @param fpRsp:    the hook invoked when a response is received
 * @param fpTmo:    the hook invoked when timeout
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int utl_trm_set_hooks(utl_trm_t* pTrm, 
                      int ptype, void* data,
                      FP_UTL_TR_HOOK_REQ fpReq, 
                      FP_UTL_TR_HOOK_RSP fpRsp,
                      FP_UTL_TR_HOOK_TMO fpTmo);

/**
 * SEND a request to the server added before
 * @param pTrm:     the transaction manager to be used
 * @param req:      the request message to be sent
 * @param len:      the length of the message to be sent
 * @param flags:    the transaction flags(b0~b7: user flags, b8~b15: user proto type)
 * @param data:     the user data corresponding to the transaction
 *
 * @return:         the transaction id if sucessfully, otherwize -1 returned
 */
int utl_trm_send_req(utl_trm_t* pTrm, const char* req, int len, int flags, void* data);

/**
 * SEND a request to the specified address
 * @param pTrm:     the transaction manager to be used
 * @param req:      the request message to be sent
 * @param len:      the length of the message to be sent
 * @param flags:    the transaction flags(b0~b7: user flags, b8~b15: user proto type)
 * @param to:       the remote address to which the request will be sent
 * @param data:     the user data corresponding to the transaction
 *
 * @return:         the transaction id if sucessfully, otherwize -1 returned
 */
int utl_trm_send_req_to(utl_trm_t* pTrm, const char* req, int len, int flags, utl_addr_t* to, void* data);

/**
 * INITIALIZE a pipe address from utl_tr_msg_t
 * @param ppaddr:   the pipe address need to be initialized
 * @param msg:      the message received via hook message function set by utl_trm_set_hook_msg()
 */
void utl_trm_ppaddr_init(utl_tr_ppaddr_t* ppaddr, utl_tr_msg_t* msg);

/**
 * SEND a request to the specified pipe address
 * @param pTrm:     the transaction manager to be used
 * @param req:      the request message to be sent
 * @param len:      the length of the message to be sent
 * @param flags:    the transaction flags(b0~b7: user flags, b8~b15: user proto type)
 * @param to:       the remote pipe address to which the request will be sent
 * @param data:     the user data corresponding to the transaction
 *
 * @return:         the transaction id if sucessfully, otherwize -1 returned
 */
int utl_trm_send_req_pipe(utl_trm_t* pTrm, const char* req, int len, int flags, utl_tr_ppaddr_t* to, void* data);

/**
 * SEND a response to a incoming transaction
 * @param pTrm:     the transaction manager to be used
 * @param tid:      the incoming transaction id
 * @param status:   the status of response
 * @param rsp:      the response message to be sent
 * @param len:      the length of the message to be sent
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int utl_trm_send_rsp(utl_trm_t* pTrm, int tid, int status, const char* rsp, int len);

/**
 * SEND a response to a incoming transaction with specified flags
 * @param pTrm:     the transaction manager to be used
 * @param tid:      the incoming transaction id
 * @param status:   the status of response
 * @param rsp:      the response message to be sent
 * @param len:      the length of the message to be sent
 * @param flags:    the specified flags
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int utl_trm_send_rsp_flg(utl_trm_t* pTrm, int tid, int status, const char* rsp, int len, int flags);

/******************************************************************************************************
 * SET the transaction data
 * @param pTrm:     the transaction manager to be used
 * @param tid:      the transaction id
 * @param data:     the pointer to transaction data
 *
 * @return:         0 if sucessfully, otherwize -1 returned
 */
int utl_trm_set_trx_data(utl_trm_t* pTrm, int tid, void* data);

/**
 * RUN(blocking) a transaction manager within specifed period
 * @param pTrm:     the transaction manager to be worked on
 * @param ms:       the specified period time in milliseconds,
 *                  ms > 0: blocking within this period 
 *                  ms = 0: non blocking
 *                  ms < 0: blocking always
 *                  exit if some message received
 */
void utl_trm_run(utl_trm_t* pTrm, int ms);

/**
 * RECEIVE(blocking) a message from the message queue
 * @param pTrm:     the transaction manager to be worked on
 * @param ms:       the specified period time in milliseconds,
 *                  ms > 0: blocking within this period till a message received
 *                  ms = 0: non blocking
 *                  ms < 0: blocking always till a message received
 *
 * @return:         the pointer to the received transaction message
 *
 * @note:           if any message hook function is set, no message will not be received
 */
utl_trx_msg_t* utl_trm_recv(utl_trm_t* pTrm, int ms);

/**
 * FREE a message got by function utl_trm_recv()
 * @param pTrm:     the transaction manager to be worked on
 * @param msg:      the message to be free
 */
void utl_trm_msg_free(utl_trm_t* pTrm, utl_trx_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif


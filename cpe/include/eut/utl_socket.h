/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_socket.h
* PACKAGE:      libeut
*   Implement of:       utl_socket.c
*   Date:               07/02/08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_SOCKET_H__
#define __UTL_SOCKET_H__

#include "utl_addr.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef int utl_sock;

typedef enum
{
    UTL_SOCKPROTO_UDP = 0,
    UTL_SOCKPROTO_TCP,

    UTL_SOCKPROTO_SIZE
}utl_sock_proto_e;

typedef enum
{
    UTL_SOSTATE_CLOSED = 0,
    UTL_SOSTATE_LISTEN,
    UTL_SOSTATE_CONNECT,
    UTL_SOSTATE_ESTABLISHED
}utl_sock_state_e;

int utl_sock_init(utl_sock* s, utl_addr_type_e atype, utl_sock_proto_e ptype);
int utl_sock_free(utl_sock s, bool clean);

int utl_sock_init_fd(utl_sock* s, int fd);
int eos_sock_free_fd(utl_sock s);

/* only shutdow the connection, not freed */
void utl_sock_shutdown(utl_sock s);

/* used to protect the socket not being freed */
void utl_sock_get(utl_sock s);
void utl_sock_put(utl_sock s);

void utl_sock_set_linger(utl_sock s, int linger);
void utl_sock_reuse(utl_sock s, bool reuse);
void utl_sock_non_block(utl_sock s);
void utl_sock_set_rcvbuf(utl_sock s, int size);
void utl_sock_set_sndbuf(utl_sock s, int size);
void utl_sock_no_delay(utl_sock s, bool nodelay);
int utl_sock_set_keepalive(utl_sock s, int idle, int intv, int cnt);

/* close the socket when create a sub-process by exec() */
int utl_sock_cloexec(utl_sock s);

bool utl_sock_is_non_block(utl_sock s);

int utl_sock_bind(utl_sock s, utl_addr_t* loc);
int utl_sock_connect(utl_sock s, utl_addr_t* rem);
int utl_sock_listen(utl_sock s, int queuelen);
int utl_sock_accept(utl_sock s, utl_sock* new_s, utl_addr_t* rem);

/* return the length of data sent or received */
int utl_sock_recv(utl_sock s, char* buf, int size, utl_addr_t* rem);
int utl_sock_send(utl_sock s, const void* dat, int len, utl_addr_t* rem);

/* return the bytes not read in the receiving buffer */
int utl_sock_bytes(utl_sock s);
/* return the bytes not sent in the sending queue */
int utl_sock_nwrite(utl_sock s);

int utl_sock_get_rcvbuf(utl_sock s);
int utl_sock_get_sndbuf(utl_sock s);

int utl_sock_errno(utl_sock s);
int utl_sock_accept_errno(utl_sock s);
int utl_sock_read_errno(utl_sock s);
int utl_sock_write_errno(utl_sock s);

int utl_sock_read_bytes(utl_sock s);
int utl_sock_write_bytes(utl_sock s);

/* return the socke file descriptor */
int utl_sock_fd(utl_sock s);

/* get the socket attributes */
utl_addr_type_e utl_sock_addr_type(utl_sock s);
utl_sock_proto_e utl_sock_proto(utl_sock s);
utl_addr_t* utl_sock_loc_addr(utl_sock s);
utl_addr_t* utl_sock_rem_addr(utl_sock s);

utl_sock_state_e utl_sock_get_state(utl_sock s);
void utl_sock_set_state(utl_sock s, utl_sock_state_e state);

#define utl_sock_is_ipv4(s)  (utl_sock_addr_type(s) == UTL_ADDRTYPE_IPV4)
#define utl_sock_is_ipv6(s)  (utl_sock_addr_type(s) == UTL_ADDRTYPE_IPV6)
#define utl_sock_is_udp(s)   (utl_sock_proto(s) == UTL_SOCKPROTO_UDP)
#define utl_sock_is_tcp(s)   (utl_sock_proto(s) == UTL_SOCKPROTO_TCP)

#if defined(__cplusplus)
}
#endif 

#endif


/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_pipe.h
* PACKAGE:      libeut
*   Implement of:       utl_pipe.c
*   Date:               07/03/08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_PIPE_H__
#define __UTL_PIPE_H__

#include "utl_socket.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef void* utl_pipe;

int utl_pipe_init(utl_pipe* p_pp);
int utl_pipe_free(utl_pipe pp);

int utl_pipe_write(utl_pipe pp, const char* buf, int len);
int utl_pipe_read(utl_pipe pp, char* buf, int size);

utl_sock utl_pipe_get_rdr(utl_pipe pp);
utl_sock utl_pipe_get_wtr(utl_pipe pp);

/* return the original socket */
utl_sock utl_pipe_pop_rdr(utl_pipe pp);
utl_sock utl_pipe_pop_wtr(utl_pipe pp);
utl_sock utl_pipe_pop_svr(utl_pipe pp);

#if defined(__cplusplus)
}
#endif

#endif


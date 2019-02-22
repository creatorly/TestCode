/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_token.h
* PACKAGE:      libeut
*   Header of:          utl_token.c
*   Date:               09/04/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef  __UTL_TOKEN_H__
#define  __UTL_TOKEN_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlToken
{
    const char* dlmts;
    const char* begin;
    char* end;

    /* add to restore the delimiter */
    char restore;
    char dlmt;
    char pad[6];
}utl_token_t;

/**
 * INITIALIZE a string token parser
 * @param t:        The token paser to be initialized
 * @param dlmts:    The delimiters of token
 * @param s:        The string to be parsed
 * @param res:      The flag to identify if restore the string or not
 */
void utl_token_init(utl_token_t* t, const char* dlmts, char* s, bool res);
void utl_token_free(utl_token_t* t);

char* utl_token_get(utl_token_t* t);
char* utl_token_left(utl_token_t* t);

#ifdef __cplusplus
}
#endif

#endif


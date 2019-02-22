/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_string.h
* PACKAGE:      libeut
*   Header of:          utl_string.c
*   Date:               09/07/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_STRING_H__
#define  __UTL_STRING_H__

#include <eos_config.h>
#include <eos_string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_EOS     '\0'
#define UTL_SPACE   " \t\r\n"
#define UTL_CRLF    EOS_CRLF

const char* utl_strof(const char* str, const char* token);
const char* utl_strnotof(const char* str, const char* token);
const char* utl_endofquote(const char* begin_qt, char end_qt);

char* utl_strnotail(char* str, const char* tail);
char* utl_strnospace(char* str);
char* utl_strdup(const char* str);
char* utl_strndup(const char* str, int n);
const char* utl_strnstr(const char* in_str, const char* sub_str, int count);

#define utl_strcasecmp(s1, s2)      eos_strcasecmp(s1, s2)
#define utl_strncasecmp(s1, s2, n)  eos_strncasecmp(s1, s2, n)

char* utl_touppers(char* lowers);
char* utl_tolowers(char* uppers);

/* if all the characters of the string are digits, return the strlen */
int utl_isdigits(const char * str);

char* utl_memdup(const void* src, int len);
void utl_memdump(const void* data, int len);

void utl_free(void* ptr);

unsigned int utl_rand();

#ifdef __cplusplus
}
#endif

#endif


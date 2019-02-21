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
#include <utl_typdef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_EOS     '\0'
#define UTL_SPACE   " \t\r\n"
#define UTL_CRLF    EOS_CRLF

int utl_itoa(int x, char* buf, int size);

const char* utl_strof(const char* str, const char* token);
const char* utl_strnotof(const char* str, const char* token);
const char* utl_endofquote(const char* begin_qt, char end_qt);

char* utl_strnotail(char* str, const char* tail);
char* utl_strnospace(char* str);
char* utl_strdup(const char* str);
char* utl_strndup(const char* str, int n);
const char* utl_strnstr(const char* in_str, const char* sub_str, int count);
const char* utl_strwrd(const char* str, const char* word);

#define utl_strcasecmp(s1, s2)      eos_strcasecmp(s1, s2)
#define utl_strncasecmp(s1, s2, n)  eos_strncasecmp(s1, s2, n)

char* utl_touppers(char* lowers);
char* utl_tolowers(char* uppers);

/***************************************************************************************
 * TEST if all the characters of the string are digits, alphas, uppers or lowers
 *
 * return: the strlen if test OK, 0 if failed
 */
int utl_isdigits(const char * str);
int utl_isalphas(const char * str);
int utl_isuppers(const char * str);
int utl_islowers(const char * str);

char* utl_memdup(const void* src, int len);
void utl_memdump(const void* data, int len);

void utl_free(void* ptr);

unsigned int utl_rand();

/**************************************************************************************/
typedef struct tagstUtlString* utl_string;

utl_string utl_str_init(int init_size);
void utl_str_free(utl_string ustr);

int utl_str_add(utl_string ustr, const char* str);
int utl_str_nadd(utl_string ustr, const char* str, int len);
int utl_str_add_int(utl_string ustr, int iVal);
int utl_str_add_line(utl_string ustr, const char* str);

/* delete a suffix string with specified length, -1 means del all  */
/* return the length of remain string */
int utl_str_del(utl_string ustr, int len);

int UTL_CHK_FMTARGS(2) utl_str_printf(utl_string ustr, const char* fmt, ...);

const char* utl_str_str(utl_string ustr);
int utl_str_len(utl_string ustr);

/* you MUST free the returned string using utl_free() */
char* utl_str_pop(utl_string ustr, int* pLen);

#ifdef __cplusplus
}
#endif

#endif


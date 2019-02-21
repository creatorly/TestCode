/********************************************************
* 
*
* xp-workroom Utils platform
* Copyright (C) 2009-2009, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         utl_typdef.h
* PACKAGE:      libeut
*   Header of:          utl_util.c
*   Date:               06/01/09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __UTL_TYPDEF_H__
#define  __UTL_TYPDEF_H__

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTL_OK  0
#define UTL_EOS '\0'

#define UTL_INT_MAX     0x7fffffff
#define UTL_INT_MIN     (-UTL_INT_MAX - 1)

#define UTL_HEX_UPPER_CHARS     "0123456789ABCDEF"
#define UTL_HEX_LOWER_CHARS     "0123456789abcdef"

#ifdef UTL_DOWHILE
#define DO  do
#define WHILE(cond) while(cond)
#else
#define DO
#define WHILE(cond)
#endif

#define _UTL_STR(p)     #p
#define UTL_STR(p)      _UTL_STR(p)

#define UTL_PTOI(p)        ((int)(long)(p))

#ifdef __GNUC__
#define UTL_CHK_FMTARGS(fmt_pos) \
            __attribute__((format(printf, fmt_pos, fmt_pos+1)))
#else
#define UTL_CHK_FMTARGS(fmt_pos)
#endif

#define utl_min(x, y)   (((x) < (y)) ? (x) : (y))
#define utl_max(x, y)   (((x) > (y)) ? (x) : (y))

#define utl_range(v, min, max)  (((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)))

#ifdef __ACOM_MACOS
#define utl_mod(x, y)   utl_get_mod(x, y)
#else
#define utl_mod(x, y)   ((x) % (y))
#endif

#define utl_sizeofa(a)  ((sizeof(a)/sizeof(a[0])))
#define utl_sizeofm(T, m) sizeof(((T*)0)->m)
#define utl_offsetof(T, m) ((size_t)(&(((T*)0)->m)))
#define utl_structof(T, m, p) ((T*)((char*)(p) - utl_offsetof(T, m)))

#define utl_round_down(x, n)    ((x)/(n)*(n))
#define utl_round_up(x, n)      utl_round_down((x) + (n) - 1, (n))

#define utl_align_down(p, n)    (typeof(p))((char*)(p) - (UTL_PTOI(p) & ((n) - 1)))
#define utl_align_up(p, n)      utl_align_down((char*)(p) + ((n) - 1), (n))

#define utl_byte_is_set(B, b)   ((B) & (1 << (b)))
#define utl_byte_set(B, b)      ((B) |= (1 << (b)))
#define utl_byte_clr(B, b)      ((B) &= ~(1 << (b)))

#define utl_bytes_is_set(s, n)  utl_byte_is_set((s)[(n) >> 3], (n) & 0x07)
#define utl_bytes_set(s, n)     utl_byte_set((s)[(n) >> 3], (n) & 0x07)
#define utl_bytes_clr(s, n)     utl_byte_clr((s)[(n) >> 3], (n) & 0x07)

#define utl_byte_set(B, b)      ((B) |= (1 << (b)))
#define utl_byte_clr(B, b)      ((B) &= ~(1 << (b)))

#define utl_swap(x, y) \
DO{ \
    (x) ^= (y); \
    (y) ^= (x); \
    (x) ^= (y); \
}WHILE(0)

#define utl_chacpy(buf, str) utl_buf_strcpy(buf, str)

#define utl_buf_strcpy(buf, str) utl_buf_strncpy(buf, str, sizeof(buf))
#define utl_buf_strncpy(buf, str, size) \
DO{ \
    eos_strncpy(buf, str, size); \
    (buf)[size - 1] = UTL_EOS; \
}WHILE(0)

#define utl_buf_memcpy(buf, mem, len) utl_buf_memncpy(buf, sizeof(buf), mem, len)
#define utl_buf_memncpy(buf, size, mem, len) \
DO{ \
    if (len < 1) { \
        buf[0] = UTL_EOS; \
    } else if (len < size) { \
        eos_memcpy(buf, mem, len); \
        buf[len] = UTL_EOS; \
    } else { \
        eos_memcpy(buf, mem, size - 1); \
        buf[size - 1] = UTL_EOS; \
    } \
}WHILE(0)

#define utl_tv_less(tv1, tv2) \
            (((tv1)->tv_sec < (tv2)->tv_sec) \
            || (((tv1)->tv_sec == (tv2)->tv_sec) && ((tv1)->tv_usec < (tv2)->tv_usec)))

#define utl_tv_more(tv1, tv2) utl_tv_less(tv2, tv1)

int utl_get_mod(int x, int y);

/* return the number of bits whose value is 1 */
int utl_numofbit1(int i);

#ifdef __cplusplus
}
#endif

#endif


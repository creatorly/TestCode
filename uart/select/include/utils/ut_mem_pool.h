
#ifndef __UT_MEM_POOL_H__
#define __UT_MEM_POOL_H__

#include <stdarg.h>
#include <ut_mem.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagstUtMemPool* PST_UT_MP;

PST_UT_MP ut_mp_create(int bUseLock);
void ut_mp_destroy(PST_UT_MP pmp);

int ut_mp_add(PST_UT_MP pmp, ut_mem_size_t size, int cnt);

void* ut_mp_alloc(PST_UT_MP pmp, int size, const char* file, int line);
void* ut_mp_realloc(PST_UT_MP pmp, void* ptr, int size, const char* file, int line);
void ut_mp_free(PST_UT_MP pmp, void* ptr, const char* file, int line);

void ut_mp_debug(PST_UT_MP pmp, int enable);

/* if null print passed the information will be printed to stdout */
int ut_mp_show(PST_UT_MP pmp, ut_mem_print_fp print, void* out);

#if defined(__cplusplus)
}
#endif

#endif


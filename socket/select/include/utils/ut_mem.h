
#ifndef __UT_MEM_H__
#define __UT_MEM_H__

#include <stdarg.h>
#include <eos_mem.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_MEM_DEBUG

typedef enum
{
    UT_MEM_SIZE_32 = 32,
    UT_MEM_SIZE_64 = 64,
    UT_MEM_SIZE_128 = 128,
    UT_MEM_SIZE_256 = 256,
    UT_MEM_SIZE_512 = 512,
    UT_MEM_SIZE_1024 = 1024,
    UT_MEM_SIZE_2048 = 2048,
    UT_MEM_SIZE_4096 = 4096
}ut_mem_size_t;

typedef int (*ut_mem_print_fp)(void* fd, const char* info);

int ut_mem_config(ut_mem_size_t size, int n);
int ut_mem_init(int nCfgSize);

void* ut_mem_alloc(int size, const char* file, int line);
void* ut_mem_realloc(void* ptr, int size, const char* file, int line);
void ut_mem_free(void* ptr, const char* file, int line);

void ut_mem_debug(int enable);

/* if null print passed the information will be printed to stdout */
int ut_mem_show(ut_mem_print_fp print, void* out);

/***************************************************************************/

#define UT_MALLOC(s)        eos_malloc(s)
#define UT_REALLOC(p, s)    eos_realloc(p,s)
#define UT_FREE(p)          eos_free(p)

#define UT_TALLOC(T)        (T*)UT_MALLOC(sizeof(T))
#define UT_TCALLOC(T, n)    (T*)UT_MALLOC(sizeof(T)*(n))

#define UT_MemAlloc(s, f, l)        __eos_malloc(s, f, l)
#define UT_MemReAlloc(p, s, f, l)   __eos_realloc(p, s, f, l)
#define UT_MemDebug(en)     eos_mdebug(en)
#define UT_MemShow(fp, fd)  eos_mem_show(fp, fd)

#if defined(__cplusplus)
}
#endif

#endif


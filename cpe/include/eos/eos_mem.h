#ifndef __EOS_MEM_H
#define __EOS_MEM_H

#include <eos_config.h>
#include <eos_typedef.h>
#include <eos_statistics.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef EOS_MEM_DEBUG

typedef struct memcfgunit{
        _UINT32        memunitsize;
        _UINT32        memunitnum;
}MEMCFGUNIT,*PMEMCFGUNIT;

enum{
    EOS_MEM_SIZE_32 = 32,
    EOS_MEM_SIZE_64 = 64,
    EOS_MEM_SIZE_128 = 128,
    EOS_MEM_SIZE_256 = 256, 
    EOS_MEM_SIZE_512 = 512, 
    EOS_MEM_SIZE_1024 = 1024,
    EOS_MEM_SIZE_2048 = 2048,
    EOS_MEM_SIZE_4096 = 4096,
    EOS_MEM_SIZE_END = 0
};    

typedef struct tagstEosMem* EOS_MEM_ID;

_INT32 eos_mem_init();
_INT32 eos_mem_show(EOS_INFO_PRINTF print, _VOID* out);

_INT32 eos_mem_create(const _CHAR8 *pcName, PMEMCFGUNIT pmcu, _UINT32 num, EOS_MEM_ID *pstMemID);

_VOID* eos_mem_malloc(EOS_MEM_ID mid, _INT32 size, const _CHAR8* fi, _INT32 li);
_VOID* eos_mem_realloc(EOS_MEM_ID mid, void* ptr, int size, const char* fi, int li);
_INT32 eos_mem_free(EOS_MEM_ID mid, _VOID* ptr, const _CHAR8* fi, _INT32 li);
_INT32 eos_mem_stat(EOS_MEM_ID mid, EOS_INFO_PRINTF print, void* out);

#define    EOS_MALLOC(mid, s)       eos_mem_malloc(mid, s, __FILE__, __LINE__)
#define    EOS_REALLOC(mid, p, s)   eos_mem_realloc(mid, p, s, __FILE__, __LINE__)
#define    EOS_FREE(mid, p)         eos_mem_free(mid, p, __FILE__, __LINE__)

#ifdef EOS_MEM_DEBUG
void* __eos_malloc(size_t size, const char* file, int line);
void* __eos_calloc(size_t num, size_t size, const char* file, int line);
void* __eos_realloc(void* ptr, size_t size, const char* file, int line);
void __eos_free(void* ptr, const char* file, int line);
void __eos_mem_debug(int bEnable);

#define eos_malloc(s)       __eos_malloc(s, __FILE__, __LINE__)
#define eos_calloc(n, s)    __eos_calloc(n, s, __FILE__, __LINE__)
#define eos_realloc(p, s)   __eos_realloc(p, s, __FILE__, __LINE__)
#define eos_free(p)         __eos_free(p, __FILE__, __LINE__)

#define eos_mdebug(en)      __eos_mem_debug(en)

#else
#define __eos_malloc(s, f, l)       malloc(s)
#define __eos_realloc(p, s, f, l)   realloc(p, s)

#define eos_malloc(s)       malloc(s)
#define eos_calloc(n, s)    calloc(n, s)
#define eos_realloc(p, s)   realloc(p, s)
#define eos_free(p)         free(p)

#define eos_mdebug(en)
#endif

#define eos_calloc_t(n, T)  (T*)eos_calloc(n, sizeof(T))
#define eos_malloc_t(T)     eos_calloc_t(1, T)

/* to compatible with previous version */
#define eos_talloc(T)       eos_calloc_t(1, T)

#ifdef __cplusplus
}
#endif
#endif

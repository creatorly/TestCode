
#include <eos.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utils.h>
#include <ut_mem.h>

#if defined(__cplusplus)
extern "C" {
#endif

static MEMCFGUNIT g_utMemUnits[8];
static int g_utMemUnitNum = 0;
static EOS_MEM_ID g_utMemId = NULL;

int ut_mem_config(ut_mem_size_t size, int n)
{
    if (g_utMemUnitNum >= EOS_NELEMENTS(g_utMemUnits))
    {
        printf("too many mem units! max_num = %zd\r\n", EOS_NELEMENTS(g_utMemUnits));
        return EOS_ERROR;
    }

    PMEMCFGUNIT pMemCfg = &g_utMemUnits[g_utMemUnitNum++];

    pMemCfg->memunitsize = size;
    pMemCfg->memunitnum = n;

    return EOS_OK;
}

int ut_mem_init(int chn_num)
{
    int i;

    for (i = 0; i < g_utMemUnitNum; i++)
    {
        g_utMemUnits[i].memunitnum *= chn_num;
    }
    
    return eos_mem_create("utMemPool", g_utMemUnits, g_utMemUnitNum, &g_utMemId);
}

void* ut_mem_alloc(int size, const char* file, int line)
{
    return eos_mem_malloc(g_utMemId, size, file, line);
}

void* ut_mem_realloc(void* ptr, int size, const char* file, int line)
{
    return eos_mem_realloc(g_utMemId, ptr, size, file, line);
}

void ut_mem_free(void* ptr, const char* file, int line)
{
    eos_mem_free(g_utMemId, ptr, file, line);
}

void ut_mem_debug(int enable)
{
    eos_mdebug(enable);
}

int ut_mem_show(ut_mem_print_fp print, void* out)
{
    return 0;
}

#if defined(__cplusplus)
}
#endif



#ifndef __UT_EXC__H__
#define __UT_EXC__H__

#ifdef __VXWORKS_OS__

#if (EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN)
#include <arch/arm/esfArm.h>
#elif (EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN)
#include <arch/ppc/esfPpc.h>
#define ESF ESFPPC
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_EXC_MAX_RESUME     128

typedef struct tagstUtilsExcFunc
{
    _INT32 task;
    FP_UT_EXC_HANDLER func;
    void* data;
}ST_UT_EXC_HANDLER;

typedef struct tagstUtilsExcWdgTask
{
    _INT32 task;
    _INT32 wdgTask;
}ST_UT_EXC_WDGTASK;

typedef struct tagstUtilsException
{
    SEM_ID mutex;
    SEM_ID sem;

    _INT32 task;
    _INT32 vecNum;

    ESF* esf;

    EOS_INFO_PRINTF dump;
    _VOID* data;

    ST_UT_EXC_HANDLER handlers[UT_EXC_MAX_RESUME];
    ST_UT_EXC_WDGTASK wdgTasks[UT_EXC_MAX_RESUME];
}ST_UT_EXC;

#if (EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN)

typedef struct tagstUtilsStackFrame
{
    _UINT32* fp;    /* the last Frame Pointer */
    _UINT32* ip;    /* the last Stack Pointer */
    _UINT32* lr;    /* the last Link Register(returned address) */
    _UINT32* pc;    /* the Program Counter */
    _UINT32 sp[0];  /* the last Stack Pointer, the ip must equal to sp */
}ST_UT_SF;

#elif (EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN)

typedef struct tagstUtilsStackFrame
{
    _UINT32* sp;        /* the last Stack Pointer */
    _UINT32* lr;        /* the last Link Register(returned address) */
    _UINT32* args[0];   /* the arguments of the invoking */
}ST_UT_SF;

#endif

static ST_UT_EXC g_stUtExc;

#define UT_GET_REGISTER(reg, name) \
            __asm__("mov %0, " name "\n\t" :"=r"(reg))

_VOID utExcHandler(_INT32 task, _INT32 vecNum, ESF* pEsf);
_VOID utExcWdgMain(ST_UT_EXC* exc);

_INT32 utExcInfoGet(_VOID* sp, _CHAR8* buf, _INT32 size);
_INT32 utExcInfoDump(ST_UT_EXC* exc, _CHAR8* buf);
#ifdef ACOM_DEBUG
_INT32 utExcAnalyzeStack(_VOID* sp, _VOID* pc, _CHAR8* buf, _INT32 size);
#endif

_INT32 utExcDefHandler(_INT32 task, _VOID* data);
_VOID excClrWdgMain(_INT32 interval, _INT32 n);

#if defined(__cplusplus)
}
#endif

#endif

#endif


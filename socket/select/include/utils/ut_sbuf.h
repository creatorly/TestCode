
#ifndef __UT_SBUF_H__
#define __UT_SBUF_H__

#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagstUtilsStreamBuffer
{
    _CHAR8* usrBuf;
    _CHAR8* buf;
    _INT32 size;
    _INT32 len;

    /* end of line, default is "\r\n", efficient for UT_SbufOutLine */
    _CHAR8 endl[4];
    _INT32 endlen;
}ST_UT_SBUF;

_INT32 UT_SbufInit(ST_UT_SBUF* sbuf, _CHAR8* buf, _INT32 size);
_VOID UT_SbufFree(ST_UT_SBUF* sbuf);

_INT32 UT_SbufOut(ST_UT_SBUF* sbuf, const _CHAR8* fmt, ...);
_INT32 UT_SbufAdd(ST_UT_SBUF* sbuf, const _CHAR8* str);
_INT32 UT_SbufCpy(ST_UT_SBUF* sbuf, const _CHAR8* mem, _INT32 len);

_INT32 UT_SbufOutLine(ST_UT_SBUF* sbuf, const _CHAR8* fmt, ...);

/* set the end string of a line */
_INT32 UT_SbufSetEndl(ST_UT_SBUF* sbuf, _CHAR8* endl);

#if defined(__cplusplus)
}
#endif

#endif



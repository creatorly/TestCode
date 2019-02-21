
#include <eos.h>

#include "ut_mem.h"
#include "ut_sbuf.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define UtSbufEndlInit(pbuf) \
do{ \
    (pbuf)->endl[0] = '\r'; \
    (pbuf)->endl[1] = '\n'; \
    (pbuf)->endl[2] = '\0'; \
    (pbuf)->endlen = 2; \
}while (0)

_INT32 UT_SbufInit(ST_UT_SBUF* sbuf, _CHAR8* buf, _INT32 size)
{
    sbuf->usrBuf = buf;
    sbuf->buf = buf;
    sbuf->size = size;
    sbuf->len = 0;

    if ((NULL == buf) || (size <= 0))
    {
        sbuf->size = (size > 0) ? size : 1024;

        /* pre-allocate a buffer */
        sbuf->buf = (_CHAR8*)UT_MALLOC(sbuf->size);
        if (NULL == sbuf->buf)
        {
            printf("UT_SbufInit: failed to malloc for %d!\r\n", sbuf->size);
            return EOS_ERROR;
        }
    }

    UtSbufEndlInit(sbuf);

    return EOS_OK;
}

_VOID UT_SbufFree(ST_UT_SBUF* sbuf)
{
    if (sbuf != NULL)
    {
        if (sbuf->buf && (sbuf->buf != sbuf->usrBuf))
        {
            UT_FREE(sbuf->buf);
        }
    }
}

_INT32 UT_SbufOut(ST_UT_SBUF* sbuf, const _CHAR8* fmt, ...)
{
    _CHAR8 buf[4096];
    _INT32 len;
    va_list ap;

    va_start(ap, fmt);
    len = vsprintf(buf, fmt, ap);
    va_end(ap);

    return UT_SbufCpy(sbuf, buf, len);
}

_INT32 UT_SbufAdd(ST_UT_SBUF* sbuf, const const _CHAR8* str)
{
    return UT_SbufCpy(sbuf, str, strlen(str));
}

_INT32 UT_SbufCpy(ST_UT_SBUF* sbuf, const _CHAR8* mem, _INT32 len)
{
    if (sbuf->len + len >= sbuf->size)
    {
        if (sbuf->buf != sbuf->usrBuf)
        {
            _CHAR8* pTmp = UT_REALLOC(sbuf->buf, sbuf->size*2 + len);
            if (NULL == pTmp)
            {
                printf("UT_SbufCpy: failed to realloc %d\r\n", sbuf->size*2 + len);
                return 0;
            }

            sbuf->buf = pTmp;
            sbuf->size += (sbuf->size + len);
        }
        else
        {
            len = sbuf->size - sbuf->len - 1;
        }
    }

    if (len > 0)
    {
        memcpy(&sbuf->buf[sbuf->len], mem, len);
        sbuf->len += len;

        sbuf->buf[sbuf->len] = '\0';
    }
    
    return len;
}

_INT32 UT_SbufOutLine(ST_UT_SBUF* sbuf, const _CHAR8* fmt, ...)
{
    _CHAR8 buf[4096];
    _INT32 len;
    va_list ap;

    va_start(ap, fmt);
    len = vsprintf(buf, fmt, ap);
    va_end(ap);

    len = UT_SbufCpy(sbuf, buf, len);

    /* add the line end string */
    len += UT_SbufCpy(sbuf, sbuf->endl, sbuf->endlen);

    return len;
}

_INT32 UT_SbufSetEndl(ST_UT_SBUF* sbuf, _CHAR8* endl)
{
    if (NULL == endl)
    {
        UtSbufEndlInit(sbuf);
    }
    else
    {
        _INT32 len = strlen(endl);

        if (len >= EOS_NELEMENTS(sbuf->endl))
        {
            sbuf->endlen = sizeof(sbuf->endl) - 1;

            memcpy(sbuf->endl, endl, sbuf->endlen);
            sbuf->endl[sbuf->endlen] = '\0';
        }
        else
        {
            strcpy(sbuf->endl, endl);
            sbuf->endlen = len;
        }
    }

    return sbuf->endlen;
}

#if defined(__cplusplus)
}
#endif



#include "eos.h"
#include "ut_msgq.h"

#if defined(__cplusplus)
extern "C" {
#endif

ST_UT_MSGQ* UT_MsgqInit(_INT32 maxMsgs, _INT32 maxMsgLen)
{
    return utl_msgq_buf_create(maxMsgs, maxMsgLen);
}

_VOID UT_MsgqFree(ST_UT_MSGQ* msgq)
{
    utl_msgq_free(msgq);
}

_INT32 UT_MsgqSize(ST_UT_MSGQ* msgq)
{
    return utl_msgq_size(msgq);
}

_INT32 UT_MsgqRecv(ST_UT_MSGQ* msgq, _CHAR8* buf, _INT32 size, _INT32 timeout)
{
    return utl_msgq_buf_recv(msgq, buf, size, timeout);
}

_INT32 UT_MsgqGrab(ST_UT_MSGQ* msgq, _VOID* buf, _INT32 size)
{
    return utl_msgq_buf_recv(msgq, buf, size, 0);
}

_INT32 UT_MsgqSend(ST_UT_MSGQ* msgq, const _VOID* msg, _INT32 len)
{
    return utl_msgq_buf_send(msgq, msg, len);
}

_INT32 UT_MsgqStat(ST_UT_MSGQ* msgq, _CHAR8* buf, _INT32 size)
{
    _INT32 len = 0;

    len = snprintf(buf, size,
        "msgq stat: size(%d), sent(%d), rcvd(%d), lost(%d)",
        utl_msgq_size(msgq), 0, 0, 0);

    return len;
}

#if defined(__cplusplus)
}
#endif



#include "svrcom.h"
#include "line.h"
#include "proxycom.h"

_UINT32 SvrSend(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size)
{
    return ProxyRecv(ChannelID, buf, size);
}

_UINT32 SvrRecv(_UINT32 ChannelID, const _CHAR8* buf, _INT32 size)
{
    return Line_ProccessRecv(ChannelID, buf, size);
}



/******************************************************************************
 * Description:
 * Author: pengjian
 * Histroy: 1.00, 2006.07.26 created
 * CopyRight: Ejoin Tech. Ltd. Co, 2006.7~2006
******************************************************************************/

#include "eos.h"
#include "ut_debug.h"
#include "ut_udp.h"

#if defined(__cplusplus)
extern "C" {
#endif

_USHORT16 UT_UdpChkSum(const _CHAR8* data, _USHORT16 len)
{
    _UCHAR8* uc = (_UCHAR8*)data;
    _USHORT16 sum = 0;
    _USHORT16 i = 0;

    UT_ASSERT(data);

    for (; i<(len>>1); i++)
    {
        sum += ~((uc[i<<1] << 8) | uc[(i<<1)+1]);
    }

    if ((i<<1) < len)
    {
        sum += ~(uc[i<<1] << 8);
    }

    return sum;
}

_USHORT16 UT_ChkSum(_VOID* data, _USHORT16 len)
{
    _UINT32 sum = 0;
    _UCHAR8* pucData = (_UCHAR8*)data;
    _USHORT16 i = 0;

    UT_ASSERT(data);

    for (; i<(len&~1); i+=2)
    {
        sum += (pucData[i] << 8) | pucData[i+1];
    }

    if (i < len)
    {
        sum += (pucData[i] << 8);
    }

    sum = (sum >> 16) | (sum & 0xffff);
    sum = (sum >> 16) | (sum & 0xffff);

    return (_USHORT16)(~sum);
}

#if defined(__cplusplus)
}
#endif


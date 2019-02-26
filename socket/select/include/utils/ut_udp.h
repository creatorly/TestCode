
#ifndef UT_UDP_H
#define UT_UDP_H

#if defined(__cplusplus)
extern "C" {
#endif

/* the implemtion of this routine is not correct, but it is used before, so reserve it */
_USHORT16 UT_UdpChkSum(const _CHAR8* data, _USHORT16 len);

_USHORT16 UT_ChkSum(_VOID* data, _USHORT16 len);

#if defined(__cplusplus)
}
#endif

#endif



#ifndef UT_PEER_ADDR_H
#define UT_PEER_ADDR_H

#if defined(__cplusplus)
extern "C" {
#endif

EOS_DECLARE_HANDLE(HPAL);

_INT32 utPeerAppConstruct(LPHPAL lpHApp);
_INT32 utPeerAppDestruct(HPAL pHApp);
_INT32 utPeerAppGetAddr(HPAL pHApp,_CHAR8** ppcCallNum, _CHAR8* pcIp, _USHORT16* pusPort);
_INT32 utPeerAppDelAddr(HPAL pHApp,_CHAR8* pcId);
_INT32 utPeerAppAddAddr(HPAL pHApp,_CHAR8* pcId, _CHAR8* pcIp, _USHORT16 usPort,_BOOL bDrop);
_INT32 utPeerAppShow(HPAL pHApp,EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

#if defined(__cplusplus)
}
#endif

#endif


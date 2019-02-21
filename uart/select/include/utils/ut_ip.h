
EOS_DECLARE_HANDLE(HIPLST);

/* 2006.09.15 pengjian added */
typedef _VOID (*FP_UTIP_UPDATE)(_CHAR8* pcIpOrDns,
                                _CHAR8* pcIp,
                                _UINT32 uiIp,
                                _VOID* data);
typedef _VOID (*FP_UTIP_RESOLVFAIL)(_CHAR8* pcIpOrDns,
                                _INT32 code,
                                _VOID* data);

_INT32 utIpContruct(LPHIPLST lphIplst);
_INT32 utIpDestruct(HIPLST hIplst);
_INT32 utIpGetAddr(HIPLST hIplst, const _CHAR8* pcIp,_CHAR8* pcNewIp,_UINT32* puiIp);

_INT32 utIpAddAddr(HIPLST hIplst,
                   const _CHAR8* pcIp, 
                   FP_UTIP_UPDATE fpUpdate, 
                   FP_UTIP_RESOLVFAIL fpResolvFail,
                   _VOID* data);

_INT32 utIpDelAddr(HIPLST hIplst, _CHAR8* pcIp, FP_UTIP_UPDATE fpUpdate);

_INT32 utIpUpdatesAddr(HIPLST hIplst);
_INT32 utIpShow(HIPLST hIplst,EOS_INFO_PRINTF pPrintFunc,_VOID *pData);


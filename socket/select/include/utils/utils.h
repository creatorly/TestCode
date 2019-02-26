
#ifndef  UTILS_H
#define  UTILS_H

#include <eos.h>

#include <time.h>
#include <utils/ut_mem.h>
#include <utils/ut_task.h>
#include <utils/ut_timer.h>
#include <utils/ut_rtstat.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef ENABLE_UT_RTS

/***********************************************************************/
typedef _VOID (*FP_UT_IPUPDATE)(_CHAR8* pcIpOrDns,
                                _CHAR8* pcIp,
                                _UINT32 uiIp,
                                _VOID* data);

typedef _VOID (*FP_UT_IPRESOLVFAIL)(_CHAR8* pcIpOrDns, _INT32 code, _VOID* data);

_UINT32 UT_Init(_VOID);
_UINT32 UT_Init2(_BOOL bAclEnabled);
_UINT32 UT_Uninit(_VOID);
_UINT32 UT_Start(EOS_TIMER_ID TimerGID);

_UINT32 UT_Show(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

int UT_DnsAdd(const char* resolv_conf, char* dns_list);

/***********************************************************************
 * UT DNS ip interface
 */

#ifdef ENABLE_UT_DNSIP

_UINT32 UT_DnsIpAdd(const _CHAR8* pcIp);
_UINT32 UT_DnsIpAddEx(const _CHAR8* pcIp, FP_UT_IPUPDATE fpUpdate, _VOID* data);
_UINT32 UT_DnsIpAddEx2(const _CHAR8* pcIp, FP_UT_IPUPDATE fpUpdate, FP_UT_IPRESOLVFAIL fpResolvFail, _VOID* data);
_UINT32 UT_DnsIpDel(_CHAR8* pcIp, FP_UT_IPUPDATE fpUpdate);
_UINT32 UT_DnsIpGet(const _CHAR8* pcIp, _CHAR8* pcNewIp, _UINT32* puiNewIp);
_UINT32 UT_DnsShow(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

#endif

/***********************************************************************
 * flux control interface
 */

#ifdef ENABLE_UT_FLUXCTL

typedef enum
{
    EN_UTFC_IPPROTO_UDP = 0,
    EN_UTFC_IPPROTO_TCP,
    EN_UTFC_IPPROTO_UDPTCP
}EN_UTFC_IPPROTO;

typedef _BOOL (*FP_UT_FCISVOICEPKG)(_UINT32 uiChanId, _UINT32 uiIpAddr);
typedef _UINT32 (*FP_UT_FCGETTALKCNT)(_UINT32 uiChanId);

_BOOL UT_FcRecvHook(_UINT32 uiChanId, _VOID* pMblk);
_BOOL UT_FcSendHook(_UINT32 uiChanId, _VOID* pMblk);
_UINT32 UT_FcIsVoicePkgHookAdd(FP_UT_FCISVOICEPKG fpIsVoice);
_UINT32 UT_FcGetTalkCntHookAdd(FP_UT_FCGETTALKCNT fpGetTalkCnt);
_UINT32 UT_FcAclUpdate(_UINT32 uiIfNum, const _CHAR8* pcLocIp);
_UINT32 UT_FcAclAdd(_UINT32 uiIfNum, 
                     EN_UTFC_IPPROTO enProto,
                    _CHAR8* pcLocIp, 
                    _USHORT16 usLocPort, 
                    _CHAR8* pcRemIp,
                    _USHORT16 usRemPort);

_UINT32 UT_FcShow(EOS_INFO_PRINTF pPrintFunc,_VOID *pData);

#endif

/***********************************************************************
 * parameter types of fpMalloc, fpFree and fpPrint are separately 
 * FP_UTRTS_MALLOC, FP_UTRTS_FREE and EOS_INFO_PRINTF
 */

#ifdef ENABLE_UT_RTS

_UINT32 UT_RtsConstruct(_VOID** phRts, 
                        const _CHAR8* pcFile,
                        _UINT32 uiLine,
                        _VOID* fpPrint,
                        _VOID* data);

_UINT32 UT_RtsDestruct(_VOID* hRts);

_UINT32 UT_RtsBegin(_VOID* hRts, const _CHAR8* file, _INT32 line);
_UINT32 UT_RtsEnd(_VOID* hRts, const _CHAR8* file, _INT32 line);
_VOID UT_RtsShow(_VOID* hRts, FP_UT_RTS_PRINTF fpPrint, _VOID* data);

_VOID UT_RtsShowAll(FP_UT_RTS_PRINTF fpPrint, _VOID* data);

#endif

#define UT_SPACE " \t\r\n"
#define UT_ISSPACE(c) (strchr(UT_SPACE, c) != NULL)

_VOID UT_MemDump(_VOID* data, _UINT32 uiLen);

_INT32 UT_StrCaseCmp(const _CHAR8* s1, const _CHAR8* s2);
_INT32 UT_StrNCaseCmp(const _CHAR8* s1, const _CHAR8* s2, _UINT32 n);

_BOOL UT_StrIsDigit(const _CHAR8* str);
_CHAR8* UT_StrSkpSpc(_CHAR8* str);
_UINT32 UT_UdpSendRecv(eosSocket* locSock,
            _CHAR8* msg, _UINT32 len, eosAddress* remAddr, 
            _CHAR8* reply, _UINT32* replyLen, _UINT32 msec);

#ifdef ENABLE_UT_RTS
#define UtRtsDefine(rts) \
            static _VOID* rts = NULL
    
#define UtRtsBegin(rts) \
            do{ \
                if (NULL == rts) \
                { \
                    UT_RtsConstruct(&(rts), __FILE__, __LINE__, NULL, NULL); \
                } \
                UT_RtsBegin(rts, __FILE__, __LINE__); \
            }while(0)
        
#define UtRtsEnd(rts) UT_RtsEnd(rts, __FILE__, __LINE__)
#else
#define UtRtsDefine(rts)
#define UtRtsBegin(rts)
#define UtRtsEnd(rts)
#endif

#ifdef ENABLE_UT_RTS
#define UT_RTS_BEGIN(args...)   UT_RtsBegin(NULL, __FILE__, __LINE__)
#define UT_RTS_END(args...)     UT_RtsEnd(NULL, __FILE__, __LINE__)
#else
#define UT_RTS_BEGIN(args...)
#define UT_RTS_END(args...)
#endif

#define UT_BITSOF(type)  (sizeof(type) << 3)

#define UT_PORTS_ISSET(pps, pn)  \
    ((pps)[(pn)/UT_BITSOF(_UINT32)] & (1 << ((pn) % UT_BITSOF(_UINT32))))
#define UT_PORTS_SET(pps, pn) \
    ((pps)[(pn)/UT_BITSOF(_UINT32)] |= (1 << ((pn) % UT_BITSOF(_UINT32))))
#define UT_PORTS_CLR(pps, pn) \
    ((pps)[(pn)/UT_BITSOF(_UINT32)] &= ~(1 << ((pn) % UT_BITSOF(_UINT32))))

#define UT_PORTS_INT_NUM \
    ((ACOM_PORT_NUM + UT_BITSOF(_UINT32) - 1)/UT_BITSOF(_UINT32))

_UINT32 UT_PortsParse(const _CHAR8* ports, _UCHAR8* pucPorts, _UCHAR8 ucMaxPort);
_UINT32 UT_PortMaskParse(const _CHAR8* ports, _UINT32* puiPorts, _UINT32 uiMaxPort);
_INT32 UT_IdsParse(const _CHAR8** ppStrIds, _INT32* piIds, _INT32 maxIdNum);

const _CHAR8* UT_FmtTime(time_t tv_sec, _CHAR8* buf, const _CHAR8* fmtYmdHms);

#define UT_BUF_STRCPY(buf, s) \
do{ \
    strncpy(buf, s, sizeof(buf)); \
    (buf)[sizeof(buf) - 1] = '\0'; \
}while(0)

/***********************************************************************/

const _CHAR8* UT_Mem2Str(const _VOID* pMem, _INT32 len, _CHAR8* buf, _INT32 size);

int UT_FtpGetFile(const char* host, const char* file, const char* usr, const char* pwd, char** pptr);
int UT_FtpDelFile(const char* host, const char* file, const char* usr, const char* pwd);

_VOID UT_Md5Digest(const _VOID* data, _INT32 len, const _CHAR8* pkey, _CHAR8 digest[32]);
_INT32 UT_Md5Verify(const _VOID* data, _INT32 len, const _CHAR8* pkey);

_INT32 UT_FloatStr2Int(const _CHAR8* pcFloatStr, _INT32 bits);

#ifdef __cplusplus
}
#endif

#endif


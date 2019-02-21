
#ifndef UT_FLUXCTL_H
#define UT_FLUXCTL_H

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef ENABLE_UT_FLUXCTL

/* function: tell if a package is voice, 
 * uiProto:  TCP or UDP, 
 * uiIpAddr: network order ip address 
 */
typedef _BOOL (*FP_UTFC_ISVOICEPKG)(_UINT32 uiChanId, _UINT32 uiIpAddr);

/* get the count of the current talking with the value returned */
typedef _UINT32 (*FP_UTFC_GETTALKCNT)(_UINT32 uiChanId);

#define UTFC_IPPROTO_UDP 1
#define UTFC_IPPROTO_TCP 2
#define UTFC_IPPROTO_UDPTCP (UTFC_IPPROTO_UDP | UTFC_IPPROTO_TCP)

typedef struct tagstUtfcConfig
{
    _UINT32 bAclEnable;                 /* enable or disable the flux control */
    _UINT32 uiTimePerPiece;             /* time of a piece, million second */
    _UINT32 uiMaxCpuUsage;              /* max percent usage of CPU */
    _UINT32 uiMinVoicePassRate;         /* min percent pass rate of voice data */
}ST_UTFC_CONFIG;

typedef _VOID* H_UTFC;

_UINT32 UTFC_Construct(H_UTFC* phUtfc, ST_UTFC_CONFIG* pstCfg);
_UINT32 UTFC_Destruct(H_UTFC hUtfc);

_BOOL UTFC_RecvHook(H_UTFC hUtfc, _UINT32 uiChanId, _VOID* pMblk);
_BOOL UTFC_SendHook(H_UTFC hUtfc, _UINT32 uiChanId, _VOID* pMblk);

_UINT32 UTFC_IsVoicePkgHookAdd(H_UTFC hUtfc, FP_UTFC_ISVOICEPKG fpIsVoice);
_UINT32 UTFC_GetTalkCntHookAdd(H_UTFC hUtfc, FP_UTFC_GETTALKCNT fpGetTalkCnt);

_UINT32 UTFC_AclUpdate(H_UTFC hUtfc,_UINT32 uiChanId, const _CHAR8* pcLocIp);
_UINT32 UTFC_AclAdd(H_UTFC hUtfc,
                   _UINT32 uiChanId,
                   _UINT32 uiProto,
                   _CHAR8* pcLocIp, 
                   _USHORT16 usLocPort, 
                   _CHAR8* pcRemIp,
                   _USHORT16 usRemPort);

_UINT32 UTFC_Show(H_UTFC hUtfc, EOS_INFO_PRINTF fpPrint,_VOID *pData);

#endif

#if defined(__cplusplus)
}
#endif

#endif



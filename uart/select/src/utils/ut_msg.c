/******************************************************************************
                Copyright (c) 2005-2005 Ejoin Inc.
                
Filename   :    ccm_msgcomm.c
Description:    implement the interfaces of the ccm message
Revision:        1.0
Author:         pengjian
Date:            2005.10.29
******************************************************************************/

#include <ctype.h>

#include "eos.h"

#include "ut_mem.h"
#include "ut_msg.h"
#include "ut_debug.h"

typedef struct tagstCcmProtoMap
{
    const _CHAR8* pcProto;
    _UINT32 uiProto;
}ST_CCM_PROTO_MAP;    /* SIMAP: StringIntMap */

static ST_CCM_PROTO_MAP g_astCcmMsgProtoMap[] =
{
    {ASYS_MODULE_CCM,   EN_CCM_PROTO_CCM}, 
    {ASYS_MODULE_POTS,  EN_CCM_PROTO_POTS}, 
    {ASYS_MODULE_MGCP,  EN_CCM_PROTO_MGCP}, 
    {ASYS_MODULE_SIP,   EN_CCM_PROTO_SIP},
    {ASYS_MODULE_ECCP,  EN_CCM_PROTO_ECCP},
    {ASYS_MODULE_H323,  EN_CCM_PROTO_H323},
    {ASYS_MODULE_H248,  EN_CCM_PROTO_H248},
    {ASYS_MODULE_NO1,   EN_CCM_PROTO_NO1},
    {ASYS_MODULE_SLIC,   EN_CCM_PROTO_SLIC}
};

_UINT32 ccmGetIeInt(ST_CCM_MSG* msg, _UINT32 uiIeName, _UINT32 uiDefault)
{
    ST_CCM_IE* pstIe = CCM_GetIe(msg, uiIeName);
    _UINT32 uiVal = uiDefault;

    if (pstIe != NULL)
    {
        eos_memcpy(&uiVal, pstIe->data, sizeof(uiVal));
    }
    
    return uiVal;
}

ST_CCM_IE* CCM_GetIe(ST_CCM_MSG * pstCcmMsg, _UINT32 enIeType)
{
    ST_CCM_IE* pstIeEnd = NULL;
    ST_CCM_IE* pstIe = NULL;

    if (NULL == pstCcmMsg)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!");
        return NULL;
    }
    
    pstIeEnd = CCM_MSG_IEEND(pstCcmMsg);
    pstIe = CCM_MSG_IEBEGIN(pstCcmMsg);    /* get the first infomation element */

    for (; pstIe < pstIeEnd; pstIe = CCM_IE_NEXT(pstIe))
    {
        if (enIeType == pstIe->ulType)
        {
            return pstIe;
        }
    }

    return NULL;
}

ST_CCM_IE* CCM_GetIeByIdx(ST_CCM_MSG * pstCcmMsg, _UINT32 ulIndex)
{
    _UINT32 ulCount = 0;
    ST_CCM_IE* pstIeEnd = NULL;
    ST_CCM_IE* pstIe = NULL;

    if (NULL == pstCcmMsg)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!");
        return NULL;
    }
    
    pstIeEnd = CCM_MSG_IEEND(pstCcmMsg);
    pstIe = CCM_MSG_IEBEGIN(pstCcmMsg);    /* get the first infomation element */

    for (; pstIe < pstIeEnd; pstIe = CCM_IE_NEXT(pstIe))
    {
        if (ulCount++ == ulIndex)
        {
            return pstIe;
        }
    }

    UT_ERROR(EOS_ERROR, "unmatched ie type!");
    return NULL;
}

_UINT32  CCM_AddIeCopy(ST_CCM_MSG* pstCcmMsg, ST_CCM_IE* pstCcmIe)
{
    ST_CCM_IE* pstIe = NULL;

    if ((NULL == pstCcmMsg) || (NULL == pstCcmIe))
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!");
        return EOS_ERROR;
    }

    pstIe = CCM_MSG_IEEND(pstCcmMsg);
    OS_MemCpy(pstIe, pstCcmIe, (sizeof(ST_CCM_IE)+pstCcmIe->ulLen));
    pstCcmMsg->ulMsgLen += CCM_IE_GETSIZE(pstCcmIe);

    return EOS_OK;
}

_UINT32  CCM_AddIe(ST_CCM_MSG* pstCcmMsg, _UINT32 enIeType, 
                   _UINT32 ulIeLen, const _VOID* pvData)
{
    ST_CCM_IE* pstIe = NULL;

    if (NULL == pstCcmMsg)
    {
        UT_ERROR(EOS_ERROR, "null pointer passed!");
        return EOS_ERROR;
    }

    pstIe = CCM_MSG_IEEND(pstCcmMsg);
    pstIe->ulType = enIeType;
    pstIe->ulLen = ulIeLen;
    pstCcmMsg->ulMsgLen += sizeof(ST_CCM_IE);

    if (pvData && (CCM_IE_REQUEST != ulIeLen))
    {
        OS_MemCpy(pstIe->data, pvData, ulIeLen);
        pstCcmMsg->ulMsgLen += CCM_LEN_ALIGN(ulIeLen);
    }
    
    return EOS_OK;
}

_UINT32  CCM_AddIntIe(ST_CCM_MSG* pstMsg, _UINT32 enIeType, _UINT32 uiIeVal)
{
    return CCM_AddIe(pstMsg, enIeType, sizeof(uiIeVal), &uiIeVal);
}

ST_CCM_MSG* CCM_GetMsgCopy(ST_CCM_MSG* msg, FP_CCM_MSG_MALLOC fpMalloc)
{
    ST_CCM_MSG* pstNewMsg = NULL;
    FP_CCM_MSG_MALLOC fpMemAlloc = fpMalloc ? fpMalloc : (FP_CCM_MSG_MALLOC)malloc;

    pstNewMsg = (ST_CCM_MSG*)fpMemAlloc(CCM_MSG_GETLEN(msg));
    if (NULL == pstNewMsg)
    {
        UT_ERROR(EOS_ERROR, "allocation of memory failed!\r\n");
        return NULL;
    }

    memcpy(pstNewMsg, msg, CCM_MSG_GETLEN(msg));
    return pstNewMsg;
}

_UINT32 CCM_GetIeInt(ST_CCM_MSG* msg, _UINT32 uiIeName)
{
    return ccmGetIeInt(msg, uiIeName, CCM_INVALID_VALUE);
}

_CHAR8* CCM_GetIeStr(ST_CCM_MSG* msg, _UINT32 uiIeName)
{
    ST_CCM_IE* pstIe = CCM_GetIe(msg, uiIeName);
    return pstIe ? (pstIe->data) : NULL;
}

_UINT32 CCM_GetErrcode(ST_CCM_MSG* msg)
{
    return ccmGetIeInt(msg, EN_CCM_IENAME_ERRCODE, EOS_OK);
}

const _CHAR8* CCM_GetMsgtypeName(EN_CCM_MSGTYPE msgtype)
{
    const _CHAR8* apcMsgtypeName[] =
        {
            "SETUP",
            "SETUP-ACK",
            "INFOMATION",
            "PROCEEDING",
            "ALERTING",
            "CONNECT",
            "CONNECT-ACK",
            "RELEASE",
            "RELEASE-ACK",
            "REGISTER",
            "REGISTER-ACK",
            "UNREGISTER",
            "UNREGISTER-ACK",
            "HB",
            "HB-ACK",
            "OAM",
            
            "UNKNOWN"
        };

    if ((msgtype >= 0) && (msgtype < EOS_NELEMENTS(apcMsgtypeName)))
    {
        return apcMsgtypeName[msgtype];
    }
    else
    {
        return "UNKNOWN";
    }
}

const _CHAR8* CCM_GetIetypeName(EN_CCM_MSGTYPE msgtype)
{
    const _CHAR8* apcIetypeName[] =
        {
            "ERRCODE",
            "ONHOOK",
            "OFFHOOK",
            "DTMF_VALUE",
            "MEDIA_CODEC",
            "MEDIA_IP",
            "MEDIA_PORT",
            "MEDIA_CHANNEL_OPEN",
            "MEDIA_CHANNEL_CLOSE",
            "MEDIA_FAX_START",
            "MEDIA_FAX_STOP",
            "CALLING_NUMBER",
            "CALLED_NUMBER",
            "DISCONNECT_CAUSE",
            "BILL_START",
            "BILL_STOP",
            "EPNAME",
            "DIALTONE",
            "RINGBACK",
            "PKGLIST",
            "CONN_MODE",
            "RATE",
            "CHANNO",
            "DTMF_END",
            "FAXSTART",
            "FAXSTOP",
            "REM_RB",
            "DTMF_DURATION",
            "LINEIN",
            "LINEOUT",
            "CALL_ONHOLD",
            "CALL_OFFHOLD",
            "DTMF_ADJUST_LEN",
            "PASSWORD",
            "TALKING",
            "T38_MAXBITRATE",
            "MEDIA_PTIME",
            "FLASH_HOOK",
            "RFC2833_PLTYPE",
            "CALLER_ID",

            "CMD",
            "CAUSE",
            "REASON",

            /* add new iename here */

            "UNKNOWN"
        };
    
    return  apcIetypeName[msgtype];
}

EN_CCM_PROTO CCM_GetProtoID(_CHAR8* pcProto)
{
    _UINT32 i = 0;
    
    for (i=0; i<EOS_NELEMENTS(g_astCcmMsgProtoMap); i++)
    {
        if (0 == eos_strcasecmp(pcProto, g_astCcmMsgProtoMap[i].pcProto))
        {
            return g_astCcmMsgProtoMap[i].uiProto;
        }
    }

    return EN_CCM_PROTO_NULL;
}

const _CHAR8* CCM_GetProtoName(EN_CCM_PROTO uiPorto)
{
    _UINT32 i = 0;

    for (i=0; i<EOS_NELEMENTS(g_astCcmMsgProtoMap); i++)
    {
        if (uiPorto == g_astCcmMsgProtoMap[i].uiProto)
        {
            return g_astCcmMsgProtoMap[i].pcProto;
        }
    }

    return "UNKNOWN";
}

/*********************************************************************************/

typedef struct tagstCcmMsgProc
{
    _UINT32 uiStateNum;
    _UINT32 uiMsgTypeNum;
    FP_CCM_MSG_PROC** ppfpProc;
    _VOID* data;
}ST_CCM_MSG_PROC;

static _UINT32 ccmMsgProcDefault(_VOID* msg, _VOID* node, _VOID* data)
{
    return EOS_ERROR;
}

_UINT32 CCM_MsgProcConstruct(H_CCM_MSGPROC* phProc,
                            _UINT32 uiStateNum, 
                            _UINT32 uiMsgTypeNum,
                             FP_CCM_MSG_PROC fpDefProc,
                            _VOID* data)
{
    _UINT32 i = 0;
    _UINT32 j = 0;
    _UINT32 uiSize = sizeof(ST_CCM_MSG_PROC);
    ST_CCM_MSG_PROC* pstProc = NULL;
    FP_CCM_MSG_PROC* pfpProc = NULL;

    *phProc = NULL;

    uiSize += (sizeof(FP_CCM_MSG_PROC*) * uiStateNum);
    uiSize += (sizeof(FP_CCM_MSG_PROC) * uiMsgTypeNum * uiStateNum);
    
    pstProc = (ST_CCM_MSG_PROC*)UT_MALLOC(uiSize);
    memset(pstProc, 0, uiSize);

    pstProc->uiStateNum = uiStateNum;
    pstProc->uiMsgTypeNum = uiMsgTypeNum;
    pstProc->ppfpProc = (FP_CCM_MSG_PROC**)(pstProc + 1);
    pstProc->data = data;

    pfpProc = (FP_CCM_MSG_PROC*)&pstProc->ppfpProc[uiStateNum];
    fpDefProc = fpDefProc ? fpDefProc : ccmMsgProcDefault;
    for (i=0; i<uiStateNum; i++)
    {
        pstProc->ppfpProc[i] = &pfpProc[uiMsgTypeNum * i];
        for (j=0; j<uiMsgTypeNum; j++)
        {
            pstProc->ppfpProc[i][j] = fpDefProc;
        }
    }

    *phProc = pstProc;

    return EOS_OK;
}

_UINT32 CCM_MsgProcDestruct(H_CCM_MSGPROC hProc)
{
    memset(hProc, 0, sizeof(ST_CCM_MSG_PROC));
    UT_FREE(hProc);

    return EOS_OK;
}

_UINT32 CCM_MsgProcRegister(H_CCM_MSGPROC hProc,
                           _UINT32 uiState, 
                           _UINT32 uiMsgType,
                            FP_CCM_MSG_PROC fpProc)
{
    ST_CCM_MSG_PROC* pstProc = (ST_CCM_MSG_PROC*)hProc;

    pstProc->ppfpProc[uiState][uiMsgType] = fpProc;

    return EOS_OK;
}

_UINT32 CCM_MsgProcTrig(H_CCM_MSGPROC hProc,
                       _UINT32 uiState, 
                       _UINT32 uiMsgType, 
                       _VOID* msg,
                       _VOID* node)
{
    ST_CCM_MSG_PROC* pstProc = (ST_CCM_MSG_PROC*)hProc;

    return pstProc->ppfpProc[uiState][uiMsgType](msg, node, pstProc->data);
}


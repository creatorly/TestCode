/******************************************************************************
                Copyright (c) 2005-2005 Ejoin Inc.
                
Filename   :    ccm_msgcomm.c
Description:    implement the interfaces of the ccm message
Revision:        1.0
Author:         pengjian
Date:            2005.10.29
******************************************************************************/

#include <stddef.h>
#include "eos.h"
#include "utils.h"

#include "egcm.h"

#include "egcm_debug.h"

#define EGCM_DEBUG_TEST 0x00000001

#define EGCM_MsgSetLen(msg, len)    ((msg)->usLen = eosConvertHostToNetwork16(len))

typedef struct tagstEgcmMsgEx
{
    ST_EGCM_ALLOC stAlloc;
    ST_EGCM_MSG   stMsg;
}ST_EGCM_MSGEX;

static _VOID* egcmAlloc(_VOID* pool, EOSSize_t s) 
{ 
    return UT_MALLOC(s); 
}

static _VOID* egcmReAlloc(_VOID* pool, _VOID* p, EOSSize_t s) 
{ 
    return UT_REALLOC(p, s); 
}    

static _VOID egcmDeAlloc(_VOID *pool, _VOID* p) 
{ 
    UT_FREE(p); 
}

ST_EGCM_ALLOC m_stEgcmDefAlloc = {NULL, 256, ~0, egcmAlloc, egcmReAlloc, egcmDeAlloc};

ST_EGCM_IE* EGCM_IelstFind(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd, _USHORT16 usType)
{
    ST_EGCM_IE* pstIe = NULL;

    EGCM_NULLPTR_ASSERT(pstBegin, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(pstEnd, EOS_ERROR);

    for (pstIe=pstBegin; pstIe<pstEnd; pstIe=EGCM_IeNext(pstIe))
    {
        if (usType == EGCM_IeGetType(pstIe))
        {
            return pstIe;
        }
    }

    return NULL;
}

const _VOID* EGCM_IelstFindData(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd, _USHORT16 usType)
{
    ST_EGCM_IE* pstIe = EGCM_IelstFind(pstBegin, pstEnd, usType);
    return pstIe ? EGCM_IeGetData(pstIe) : NULL;
}

_UINT32 EGCM_IelstFindInt(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd, _USHORT16 usType)
{
    ST_EGCM_IE* pstIe = EGCM_IelstFind(pstBegin, pstEnd, usType);
    _UINT32 uiVal = pstIe ? (*(_UINT32*)EGCM_IeGetData(pstIe)) : (~0);
    return eosConvertNetworkToHost32(uiVal);
}

ST_EGCM_IE* EGCM_IelstFindIf(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd,
                             FP_EGCM_IECMP fpCmp, _VOID* key)
{
    ST_EGCM_IE* pstIe = NULL;
    
    EGCM_NULLPTR_ASSERT(pstBegin, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(pstEnd, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(fpCmp, EOS_ERROR);

    for (pstIe=pstBegin; pstIe<pstEnd; pstIe=EGCM_IeNext(pstIe))
    {
        if (fpCmp(pstIe, key))
        {
            return pstIe;
        }
    }

    return NULL;
}

_UINT32 EGCM_IelstForEach(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd,
                          FP_EGCM_IEOP fpOp, _VOID* data)
{
    _UINT32 uiRet = EOS_ERROR;
    ST_EGCM_IE* pstIe = NULL;
    
    EGCM_NULLPTR_ASSERT(pstBegin, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(pstEnd, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(fpOp, EOS_ERROR);

    for (pstIe=pstBegin; pstIe<pstEnd; pstIe=EGCM_IeNext(pstIe))
    {
        uiRet = fpOp(pstIe, data);
        if (EOS_OK != uiRet)
        {
            break;
        }
    }

    return uiRet;
}

_UINT32 EGCM_IelstForEachIf(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd,
                            FP_EGCM_IEOP fpOp, _VOID* data,
                            FP_EGCM_IECMP fpCmp, _VOID* key)
{
    _UINT32 uiRet = EOS_ERROR;
    ST_EGCM_IE* pstIe = NULL;
    
    EGCM_NULLPTR_ASSERT(pstBegin, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(pstEnd, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(fpOp, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(fpCmp, EOS_ERROR);

    for (pstIe=pstBegin; pstIe<pstEnd; pstIe=EGCM_IeNext(pstIe))
    {
        if (fpCmp(pstIe, key))
        {
            uiRet = fpOp(pstIe, data);
            if (EOS_OK != uiRet)
            {
                break;
            }
        }
    }

    return uiRet;
}

ST_EGCM_IE* EGCM_MsgGetIeByIndex(ST_EGCM_MSG* pstMsg, _USHORT16 usIndex)
{
    ST_EGCM_IE* pstEnd = NULL;
    ST_EGCM_IE* pstIe = NULL;
    _USHORT16 uiCount = 0;

    EGCM_NULLPTR_ASSERT(pstMsg, EOS_ERROR);
    
    pstEnd = EGCM_MsgIeEnd(pstMsg);
    pstIe = EGCM_MsgIeBegin(pstMsg);    /* get the first infomation element */

    for (; pstIe < pstEnd; pstIe = EGCM_IeNext(pstIe))
    {
        if (uiCount++ == usIndex)
        {
            return pstIe;
        }
    }

    EGCM_ERROR(EN_EGCM_ERRCODE_INVALID_IETYPE, "unmatched ie type!\r\n");
    return NULL;
}

_UINT32  EGCM_MsgAddIeCopy(ST_EGCM_MSG* pstMsg, ST_EGCM_IE* pstIeCopy)
{
    ST_EGCM_IE* pstIe = NULL;
    _USHORT16 usMsgLen = 0;

    EGCM_NULLPTR_ASSERT(pstMsg, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(pstIeCopy, EOS_ERROR);

    pstIe = EGCM_MsgIeEnd(pstMsg);
    if (pstIe != pstIeCopy)
    {
        memcpy(pstIe, pstIeCopy, EGCM_IeLen(pstIeCopy));
    }

    usMsgLen = EGCM_MsgGetLen(pstMsg) + EGCM_IeSize(pstIe);
    EGCM_MsgSetLen(pstMsg, usMsgLen);
    ++EGCM_MsgGetSize(pstMsg);

    return EOS_OK;
}

_UINT32  EGCM_MsgAddIe(ST_EGCM_MSG* pstMsg, _USHORT16 usType, 
                      _USHORT16 usLen, const _VOID* pvData)
{
    ST_EGCM_IE* pstIe = NULL;
    _USHORT16 usMsgLen = 0;

    EGCM_NULLPTR_ASSERT(pstMsg, EOS_ERROR);

    pstIe = EGCM_MsgIeEnd(pstMsg);
    EGCM_IeConstruct(pstIe, usType, usLen);
    usMsgLen = EGCM_MsgGetLen(pstMsg) + sizeof(ST_EGCM_IE);

    if (pvData && (EGCM_IE_REQUEST != usLen))
    {
        memcpy(EGCM_IeGetData(pstIe), pvData, usLen);
        usMsgLen += EGCM_RoundUp4(usLen);
    }
    
    EGCM_MsgSetLen(pstMsg, usMsgLen);    
    ++EGCM_MsgGetSize(pstMsg);
    
    return EOS_OK;
}

typedef struct tagstEgcmIelistDesc
{
    ST_EGCM_IE stIeList;
    ST_EGCM_IE stSize;
    _USHORT16 usSize;
    _USHORT16 usPad;
}ST_EGCM_IELIST_DESC;

#define EgcmIelistDescGetSize(ild)  eosConvertNetworkToHost16((ild)->usSize)
#define EgcmIelistDescSetSize(ild, size) ((ild)->usSize = eosConvertHostToNetwork16(size))

/**************** table interfaces *****************************************/

_UINT32 EGCM_IeListConstruct(ST_EGCM_IE* plsIe, _USHORT16 usType)
{
    ST_EGCM_IELIST_DESC* plsIeDesc = (ST_EGCM_IELIST_DESC*)plsIe;
    _USHORT16 usLen = 0;

    EGCM_NULLPTR_ASSERT(plsIe, EOS_ERROR);

    usLen = sizeof(ST_EGCM_IELIST_DESC) - sizeof(ST_EGCM_IE);
    EGCM_IeConstruct(plsIe, usType, usLen);
    EGCM_IeConstruct(&plsIeDesc->stSize, EGCM_IELIST_SIZE, sizeof(_USHORT16));
    EgcmIelistDescSetSize(plsIeDesc, 0);

    return EOS_OK;
}

EOSSize_t EGCM_IeListSize(ST_EGCM_IE* plsIe)
{
    EGCM_NULLPTR_ASSERT(plsIe, EOS_ERROR);

    return EgcmIelistDescGetSize((ST_EGCM_IELIST_DESC*)plsIe);
}

ST_EGCM_IE* EGCM_IeListGetByIndex(ST_EGCM_IE* plsIe, _USHORT16 usIndex)
{
    ST_EGCM_IELIST_DESC* plsIeDesc = (ST_EGCM_IELIST_DESC*)plsIe;
    ST_EGCM_IE* pstIe = NULL;
        
    EGCM_NULLPTR_ASSERT(plsIe, EOS_ERROR);

    EGCM_CONDCHK((usIndex<EgcmIelistDescGetSize(plsIeDesc)), 
        NULL, "invalid record index!\r\n");

    pstIe = (ST_EGCM_IE*)(plsIeDesc + 1);     /* get first record */
    while ((usIndex--) && (pstIe = EGCM_IeNext(pstIe)));

    return pstIe;
}

_UINT32 EGCM_IeListAddCopy(ST_EGCM_IE* plsIe, ST_EGCM_IE* pstIe)
{
    ST_EGCM_IELIST_DESC* plsIeDesc = (ST_EGCM_IELIST_DESC*)plsIe;
    ST_EGCM_IE* pstNextIe = NULL;
    _USHORT16 usLen = 0;

    EGCM_NULLPTR_ASSERT(plsIe, EOS_ERROR);
    EGCM_NULLPTR_ASSERT(pstIe, EOS_ERROR);

    pstNextIe = EGCM_IeNext(plsIe);
    if (pstNextIe != pstIe)
    {
        memcpy(pstNextIe, pstIe, EGCM_IeLen(pstIe));
    }

    usLen = EGCM_IeGetLen(plsIe) + EGCM_IeSize(pstIe);
    EGCM_IeSetLen(plsIe, usLen);
    usLen = EgcmIelistDescGetSize(plsIeDesc) + 1;
    EgcmIelistDescSetSize(plsIeDesc, usLen);

    return EOS_OK;
}

_UINT32 EGCM_IeListAdd(ST_EGCM_IE* plsIe, _USHORT16 usType, 
                      _USHORT16 usLen, _VOID* pvData)
{
    ST_EGCM_IELIST_DESC* plsIeDesc = (ST_EGCM_IELIST_DESC*)plsIe;
    ST_EGCM_IE* pstNextIe = NULL;
    _USHORT16 usTmp = 0;
        
    EGCM_NULLPTR_ASSERT(plsIe, EOS_ERROR);

    pstNextIe = EGCM_IeNext(plsIe);
    EGCM_IeConstruct(pstNextIe, usType, usLen);
    usTmp = EGCM_IeGetLen(plsIe) + sizeof(ST_EGCM_IE);
    EGCM_IeSetLen(plsIe, usTmp);
    
    if (pvData && (EGCM_IE_REQUEST != usLen))
    {
        if (EGCM_IeGetData(pstNextIe) != pvData)
        {
            memcpy(EGCM_IeGetData(pstNextIe), pvData, usLen);
        }

        usTmp = EGCM_IeGetLen(plsIe) + EGCM_RoundUp4(usLen);
        EGCM_IeSetLen(plsIe, usTmp);
    }

    usTmp = EgcmIelistDescGetSize(plsIeDesc) + 1;
    EgcmIelistDescSetSize(plsIeDesc, usTmp);

    return EOS_OK;
}

_VOID EGCM_Test(_VOID)
{   
#if (EGCM_DEBUG_MASK & EGCM_DEBUG_TEST)
    _USHORT16 i = 0;
    _USHORT16 j = 0;
    _CHAR8 acIeValBuf[128];
    ST_EGCM_MSG* pstMsg = NULL;
    ST_EGCM_IE* plsIe = NULL;
    ST_EGCM_IE* pstIe = NULL;

    pstMsg = (ST_EGCM_MSG*)UT_MALLOC(1024);
    if (NULL == pstMsg)
    {
        EGCM_ERROR(EN_EGCM_ERRCODE_NULLPTR, "null pointer passed!\r\n");
        return;
    }

    EGCM_MsgConstruct(pstMsg, EGCM_SYNC_FLAG, 0, 0, 1, 2);
    EGCM_MsgAddIe(pstMsg, 0, sizeof("hellow world!\r\n"), "hellow world!\r\n");
    i = 99;
    EGCM_MsgAddIeInt(pstMsg, 1, i);

    plsIe = EGCM_MsgIeEnd(pstMsg);
    EGCM_IeListConstruct(plsIe, 2);

    /* construct a ie of several records */
    for (i=0; i<3; i++)
    {        
        pstIe = EGCM_IeNext(plsIe);
        EGCM_IeListConstruct(pstIe, i);

        /* construct a ie of servral fields */
        for (j=0; j<3; j++)
        {
            sprintf(acIeValBuf, "%d-%d: hellow world!\r\n", i, j);
            EGCM_IeListAdd(pstIe, j, strlen(acIeValBuf)+1, acIeValBuf);
        }

        EGCM_IeListAddCopy(plsIe, pstIe);
    }

    EGCM_MsgAddIeCopy(pstMsg, plsIe);

    /* get the data of the message */
    plsIe = NULL;
    plsIe = EGCM_MsgGetIe(pstMsg, 2);   /* get the table ie */
    if (NULL == plsIe)
    {
        UT_FREE(pstMsg);
        EGCM_ERROR(EN_EGCM_ERRCODE_NULLPTR, "get table ie error!\r\n");
        return;
    }

    if (3 != EGCM_IeListSize(plsIe))
    {
        UT_FREE(pstMsg);
        EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "record number error!\r\n");
        return;
    }

    /* check table ie */
    for (i=0; i<EGCM_IeListSize(plsIe); i++)
    {
        pstIe = EGCM_IeListGet(plsIe, i);
        if (3 != EGCM_IeListSize(pstIe))
        {
            UT_FREE(pstMsg);
            EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "field number error!\r\n");
            return;
        }

        for (j=0; j<EGCM_IeListSize(pstIe);j++)
        {
            ST_EGCM_IE* pstIeItem = EGCM_IeListGetByIndex(pstIe, j);
            _USHORT16 usLen = 0;
            
            if (NULL == pstIeItem)
            {
                UT_FREE(pstMsg);
                EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "get field error!\r\n");
                return;
            }

            sprintf(acIeValBuf, "%d-%d: hellow world!\r\n", i, j);
            usLen = EGCM_IeGetLen(pstIeItem);
            if (usLen != strlen(acIeValBuf)+1)
            {
                UT_FREE(pstMsg);
                EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "table ie len error!\r\n");
                return;
            }

            if (0 != memcmp(EGCM_IeGetData(pstIeItem), acIeValBuf, usLen))
            {
                UT_FREE(pstMsg);
                EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "table ie val error!\r\n");
                return;
            }
        }
    }

    /* check the string ie */
    pstIe = EGCM_MsgGetIe(pstMsg, 0);   /* get the string ie */
    if (NULL == pstIe)
    {
        UT_FREE(pstMsg);
        EGCM_ERROR(EN_EGCM_ERRCODE_NULLPTR, "get string ie error!\r\n");
        return;
    }
    
    if (EGCM_IeGetLen(pstIe)!= sizeof("hellow world!\r\n"))
    {
        UT_FREE(pstMsg);
        EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "string ie len error!\r\n");
        return;
    }

    if (0 != strcmp((_CHAR8*)EGCM_IeGetData(pstIe), "hellow world!\r\n"))
    {
        UT_FREE(pstMsg);
        EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "string ie val error!\r\n");
        return;
    }

    /* check the int ie */
    if (99 != EGCM_MsgGetIeInt(pstMsg, 1))
    {
        UT_FREE(pstMsg);
        EGCM_ERROR(EN_EGCM_UNKNOWN_ERROR, "int ie val error!\r\n");
        return;
    }

    UT_FREE(pstMsg);
    EGCM_INFO("EGCM: all test success!\r\n");
    
    return;

#endif
}

/****************************************************************************
 * ST_EGCM_MSGEX api 
 */
 
_UINT32 EGCM_MsgExConstruct(ST_EGCM_MSG** ppstMsg, _UINT32 uiType, ST_EGCM_ALLOC* pstAlloc)
{
    ST_EGCM_MSGEX* pstMsgEx = NULL;

    pstAlloc = pstAlloc ? pstAlloc : &m_stEgcmDefAlloc;

    EGCM_NULLPTR_ASSERT(ppstMsg, EOS_ERROR);

    *ppstMsg = NULL;
    
    EGCM_ASSERT(pstAlloc->fpAlloc, EOS_ERROR, "alloc expected!\r\n");
    EGCM_ASSERT(pstAlloc->fpReAlloc, EOS_ERROR, "realloc expected!\r\n");
    EGCM_ASSERT(pstAlloc->fpDeAlloc, EOS_ERROR, "dealloc expected!\r\n");

    EGCM_ASSERT((pstAlloc->szMinSize >= sizeof(ST_EGCM_MSGEX)), 
        EOS_ERROR, "min size too little!\r\n");

    pstMsgEx = (ST_EGCM_MSGEX*)pstAlloc->fpAlloc(pstAlloc->pool, pstAlloc->szMinSize);
    EGCM_MALLOC_CHK(pstMsgEx, EOS_ERROR);

    memcpy(&pstMsgEx->stAlloc, pstAlloc, sizeof(pstMsgEx->stAlloc));
    EGCM_MsgConstruct(&pstMsgEx->stMsg, 0, 0, 0, 0, uiType);

    *ppstMsg = &pstMsgEx->stMsg;
    
    return EOS_OK;
}

_UINT32 EGCM_MsgExDestruct(ST_EGCM_MSG* pstMsg)
{
    ST_EGCM_MSGEX* pstMsgEx = NULL;
    
    EGCM_NULLPTR_ASSERT(pstMsg, EOS_ERROR);

    memset(pstMsg, 0, EGCM_MsgGetLen(pstMsg));
    pstMsgEx = EOS_GET_STRUCT(ST_EGCM_MSGEX, stMsg, pstMsg);
    pstMsgEx->stAlloc.fpDeAlloc(pstMsgEx->stAlloc.pool, pstMsgEx);

    return EOS_OK;
}

_UINT32 EGCM_MsgExAddIe(ST_EGCM_MSG** ppstMsg, _USHORT16 usType, 
                       _USHORT16 usLen, _VOID* data)
{
    ST_EGCM_MSGEX* pstMsgEx = NULL;
    ST_EGCM_ALLOC* pstAlloc = NULL;
    EOSSize_t szTotLen = sizeof(ST_EGCM_IE) + EGCM_RoundUp4(usLen);

    EGCM_NULLPTR_ASSERT(ppstMsg, EOS_ERROR);
    EGCM_ASSERT(*ppstMsg, EOS_ERROR, "null message passed!\r\n");

    pstMsgEx = EOS_GET_STRUCT(ST_EGCM_MSGEX, stMsg, *ppstMsg);
    pstAlloc = &pstMsgEx->stAlloc;

    szTotLen += sizeof(ST_EGCM_ALLOC) + EGCM_MsgGetLen(*ppstMsg);

    if (szTotLen > pstAlloc->szMinSize)
    {        
        pstAlloc->szMinSize = EOS_MAX(2*pstAlloc->szMinSize, szTotLen);
            
        pstMsgEx = (ST_EGCM_MSGEX*)pstAlloc->fpReAlloc
            (pstAlloc->pool, pstMsgEx, pstAlloc->szMinSize);
        
        EGCM_MALLOC_CHK(pstMsgEx, EOS_ERROR);

        *ppstMsg = &pstMsgEx->stMsg;
    }
    
    return EGCM_MsgAddIe(*ppstMsg, usType, usLen, data);
}

_UINT32  EGCM_MsgExAddIeInt(ST_EGCM_MSG** ppstMsg, _USHORT16 usType, _UINT32 uiVal)
{
    uiVal = eosConvertHostToNetwork32(uiVal);
    return EGCM_MsgExAddIe(ppstMsg, usType, sizeof(uiVal), &uiVal);
}

_UINT32  EGCM_MsgExAddIeStr(ST_EGCM_MSG** ppstMsg, _USHORT16 usType, _CHAR8* pcVal)
{
    return EGCM_MsgExAddIe(ppstMsg, usType, strlen(pcVal)+1, pcVal);
}


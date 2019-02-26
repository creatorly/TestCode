
/******************************************************************************
                Copyright (c) 2005-2005 Ejoin-Tech Inc.
                
Filename   :    egcm.h
Description:    define the message and the interface to access it
Revision:       1.0
Author:         pengjian
Date:           2005.10.29
******************************************************************************/

#ifndef EGCM_H
#define EGCM_H

#ifdef __cplusplus
extern "C" {
#endif

/********************* MACRO DEFINITION **************************************/

#define EGCM_SYNC_FLAG           0xaaaa5555
#define EGCM_IE_REQUEST          0xffff
#define EGCM_IELIST_SIZE         0xffff

#define EGCM_RoundUp4(x)  EOS_ROUND_UP(x, 4)

/********************* STRUCTION DEFINITION **********************************/
    
typedef enum
{
    EN_EGCM_ERRCODE_NULLPTR = 1,
    EN_EGCM_ERRCODE_INVALID_PARA,
    EN_EGCM_ERRCODE_MALLOC,
    EN_EGCM_ERRCODE_INVALID_IETYPE,
    EN_EGCM_ERRCODE_INVALID_RECIDX,
    
    EN_EGCM_ERRCODE_TIMEOUT,
    
    /* add new errcode to here */    
    EN_EGCM_UNKNOWN_ERROR,
    EN_EGCM_ERRCODE_BUTT
}EN_EGCM_ERRCODE;

typedef struct tagstEgcmMsg
{
    _UINT32 uiSync;
    _UINT32 uiRevId;
    _UINT32 uiChkSum;
    _UINT32 uiSender;
    _UINT32 uiRecver;
    _UCHAR8 ucType;
    _UCHAR8 ucSize;         /* only-read */
    _USHORT16 usLen;
}ST_EGCM_MSG;

typedef struct tagstEgcmInfoElem
{
    _USHORT16 usType;
    _USHORT16 usLen;
}ST_EGCM_IE;

typedef struct tagstEgcmInfoElemEx
{
    _USHORT16 usType;
    _USHORT16 usLen;
    _VOID*  data;
}ST_EGCM_IEEX;

typedef struct tagstEgcmAlloc
{
    _VOID* pool;
    EOSSize_t szMinSize;    /* should be 128 or larger, default is 128*/
    EOSSize_t szMaxSize;    /* no used */
    _VOID* (*fpAlloc)(_VOID* pool, EOSSize_t s);
    _VOID* (*fpReAlloc)(_VOID* pool, _VOID* p, EOSSize_t s);
    _VOID (*fpDeAlloc)(_VOID* pool, _VOID* p);   
}ST_EGCM_ALLOC;

/********************* base ie list interface **************************/
typedef _BOOL (*FP_EGCM_IECMP)(ST_EGCM_IE* pstIe, _VOID* key);
typedef _UINT32 (*FP_EGCM_IEOP)(ST_EGCM_IE* pstIe, _VOID* data);

ST_EGCM_IE* EGCM_IelstFind(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd, _USHORT16 usType);
const _VOID* EGCM_IelstFindData(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd, _USHORT16 usType);
_UINT32 EGCM_IelstFindInt(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd, _USHORT16 usType);
ST_EGCM_IE* EGCM_IelstFindIf(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd,
                             FP_EGCM_IECMP fpCmp, _VOID* key);
_UINT32 EGCM_IelstForEach(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd,
                          FP_EGCM_IEOP fpOp, _VOID* data);
_UINT32 EGCM_IelstForEachIf(ST_EGCM_IE* pstBegin, ST_EGCM_IE* pstEnd,
                            FP_EGCM_IEOP fpOp, _VOID* data,
                            FP_EGCM_IECMP fpCmp, _VOID* key);

/********************* msg interface ***********************************/

#define EGCM_MsgConstruct(msg, sync, revid, sender, recver, type) \
do{ \
    EGCM_MsgSetSync(msg, sync); \
    EGCM_MsgSetRevId(msg, revid); \
    EGCM_MsgSetChkSum(msg, 0); \
    EGCM_MsgSetSender(msg, sender); \
    EGCM_MsgSetRecver(msg, recver); \
    EGCM_MsgSetType(msg, type); \
    EGCM_MsgGetSize(msg) = 0; \
    (msg)->usLen = eosConvertHostToNetwork16(sizeof(ST_EGCM_MSG)); \
}while(0)

#define EGCM_MsgDestruct(msg)

#define EGCM_MsgGetSync(msg)    eosConvertNetworkToHost32((msg)->uiSync)
#define EGCM_MsgGetRevId(msg)   eosConvertNetworkToHost32((msg)->uiRevId)
#define EGCM_MsgGetChkSum(msg)  eosConvertNetworkToHost32((msg)->uiChkSum)
#define EGCM_MsgGetSender(msg)  eosConvertNetworkToHost32((msg)->uiSender)
#define EGCM_MsgGetRecver(msg)  eosConvertNetworkToHost32((msg)->uiRecver)
#define EGCM_MsgGetType(msg)    ((msg)->ucType)
#define EGCM_MsgGetSize(msg)    ((msg)->ucSize)
#define EGCM_MsgGetLen(msg)     eosConvertNetworkToHost16((msg)->usLen)
#define EGCM_MsgGetData(msg)    ((msg) + 1)
#define EGCM_MsgGetEnd(msg)     ((_CHAR8*)(msg) + EGCM_MsgGetLen(msg))

#define EGCM_MsgSetSync(msg, sync)      ((msg)->uiSync = eosConvertHostToNetwork32(sync))
#define EGCM_MsgSetRevId(msg, revid)    ((msg)->uiRevId = eosConvertHostToNetwork32(revid))
#define EGCM_MsgSetChkSum(msg, chksum)  ((msg)->uiChkSum = eosConvertHostToNetwork32(chksum))
#define EGCM_MsgSetSender(msg, sender)  ((msg)->uiSender = eosConvertHostToNetwork32(sender))
#define EGCM_MsgSetRecver(msg, recver)  ((msg)->uiRecver = eosConvertHostToNetwork32(recver))
#define EGCM_MsgSetType(msg, type)      ((msg)->ucType = (type))

#define EGCM_MsgIeBegin(msg)    ((ST_EGCM_IE*)EGCM_MsgGetData(msg))
#define EGCM_MsgIeEnd(msg)      ((ST_EGCM_IE*)EGCM_MsgGetEnd(msg))

#define EGCM_MsgGetIe(msg, type) \
    EGCM_IelstFind(EGCM_MsgIeBegin(msg), EGCM_MsgIeEnd(msg), type)
#define EGCM_MsgSearchIe(msg, cmp, key) \
    EGCM_IelstFindIf(EGCM_MsgIeBegin(msg), EGCM_MsgIeEnd(msg), cmp, key)
#define EGCM_MsgForEachIe(msg, op, data) \
    EGCM_IelstForEach(EGCM_MsgIeBegin(msg), EGCM_MsgIeEnd(msg), op, data)
#define EGCM_MsgForEachIeIf(msg, op, data, cmp, key) \
    EGCM_IelstForEachIf(EGCM_MsgIeBegin(msg), EGCM_MsgIeEnd(msg), \
        op, data, cmp, key)

ST_EGCM_IE* EGCM_MsgGetIeByIndex(ST_EGCM_MSG* pstMsg, _USHORT16 usIndex);

_UINT32  EGCM_MsgAddIeCopy(ST_EGCM_MSG* pstMsg, ST_EGCM_IE* pstIeCopy);
_UINT32  EGCM_MsgAddIe(ST_EGCM_MSG* pstMsg, _USHORT16 usType, 
                      _USHORT16 usIeLen, const _VOID* pvData);

#define EGCM_MsgAddIeInt(msg, type, i) \
do{ \
    _UINT32 __uiVal = (_UINT32)(i); \
    __uiVal = eosConvertHostToNetwork32(__uiVal); \
    EGCM_MsgAddIe(msg, type, sizeof(__uiVal), &__uiVal); \
}while(0)

#define EGCM_MsgAddIeStr(msg, type, s) \
    EGCM_MsgAddIe(msg, type, strlen(s)+1, s)

#define EGCM_MsgGetIeData(msg, type) \
    EGCM_IelstFindData(EGCM_MsgIeBegin(msg), EGCM_MsgIeEnd(msg), type)
#define EGCM_MsgGetIeInt(msg, type) \
    EGCM_IelstFindInt(EGCM_MsgIeBegin(msg), EGCM_MsgIeEnd(msg), type)
#define EGCM_MsgGetIeStr(msg, type) \
    (_CHAR8*)EGCM_MsgGetIeData(msg, type)
    
/********************* ie interface ***********************************/

#define EGCM_IeConstruct(ie, type, len) \
do{ \
    EGCM_IeSetType(ie, type); \
    EGCM_IeSetLen(ie, len); \
}while(0)

#define EGCM_IeDestruct(ie)

#define EGCM_IeGetType(ie)  eosConvertNetworkToHost16((ie)->usType)
#define EGCM_IeGetLen(ie)   eosConvertNetworkToHost16((ie)->usLen)
#define EGCM_IeGetData(ie)  ((ie) + 1)
#define EGCM_IeSetType(ie, type)    ((ie)->usType = eosConvertHostToNetwork16(type))
#define EGCM_IeSetLen(ie, len)      ((ie)->usLen = eosConvertHostToNetwork16(len))
#define EGCM_IeSetData(ie, val, len) memcpy(EGCM_IeGetData(ie), val, len)

#define EGCM_IeLen(ie)   (sizeof(ST_EGCM_IE) + EGCM_IeGetLen(ie))
#define EGCM_IeSize(ie)  EGCM_RoundUp4(EGCM_IeLen(ie))
#define EGCM_IeNext(ie)     \
    ((ST_EGCM_IE*)((_CHAR8*)(ie) + EGCM_IeSize(ie)))

#define EGCM_IeToD(ie, T)   ((T)EGCM_IeGetData(ie))
#define EGCM_IeToInt(ie)    eosConvertNetworkToHost32(*EGCM_IeToD(ie, _UINT32*))
#define EGCM_IeToStr(ie)    EGCM_IeToD(ie, _CHAR8*)

#define EGCM_IeConstructEx(ie, type, len, data) \
do{ \
    EGCM_IeSetType(ie, type); \
    EGCM_IeSetLen(ie, len); \
    EGCM_IeSetData(ie, data, len); \
}while(0)

#define EGCM_IeConstructInt(ie, type, i) \
do{ \
    _UINT32 __uiVal = (_UINT32)(i); \
    __uiVal = eosConvertHostToNetwork32(__uiVal); \
    EGCM_IeConstructEx(ie, type, sizeof(__uiVal), &__uiVal); \
}while(0)

#define EGCM_IeConstructStr(ie, type, s) \
    EGCM_IeConstructEx(ie, type, strlen(s)+1, s)

#define EGCM_IeexConstruct(ieex, type, len, val) \
do{ \
    (ieex)->usType = (type); \
    (ieex)->usLen = (len); \
    (ieex)->pvData = (val); \
}while(0)

#define EGCM_IeexDestruct(ieex)

/********************* ie list interface ***********************************/

_UINT32 EGCM_IeListConstruct(ST_EGCM_IE* plsIe, _USHORT16 usType);
EOSSize_t EGCM_IeListSize(ST_EGCM_IE* plsIe);

/* 2006.08.31 pengjian modifed to fix the exception when get from 0-size ie list */
#define EGCM_IeListBegin(plsie) ((plsie) + 3)
#define EGCM_IeListEnd(plsie)   EGCM_IeNext(plsie)
#define EGCM_IeListGet(plsie, type) \
    EGCM_IelstFind(EGCM_IeListBegin(plsie), EGCM_IeListEnd(plsie), type)
#define EGCM_IeListSearch(plsie, cmp, key) \
    EGCM_IelstFindIf(EGCM_IeListBegin(plsie), EGCM_IeListEnd(plsie), cmp, key)
#define EGCM_IeListForEach(plsie, op, data) \
    EGCM_IelstForEach(EGCM_IeListBegin(plsie), EGCM_IeListEnd(plsie), op, data)
#define EGCM_IeListForEachIf(plsie, op, data, cmp, key) \
    EGCM_IelstForEach(EGCM_IeListBegin(plsie), EGCM_IeListEnd(plsie), \
        op, data, cmp, key)

ST_EGCM_IE* EGCM_IeListGetByIndex(ST_EGCM_IE* plsIe, _USHORT16 usIndex);

_UINT32 EGCM_IeListAddCopy(ST_EGCM_IE* plsIe, ST_EGCM_IE* pstIe);
_UINT32 EGCM_IeListAdd(ST_EGCM_IE* plsIe, _USHORT16 usType, 
                      _USHORT16 usLen, _VOID* pvData);

#define EGCM_IeListGetData(plsie, type) \
    EGCM_IelstFindData(EGCM_IeListBegin(plsie), EGCM_IeListEnd(plsie), type)
#define EGCM_IeListGetInt(plsie, type) \
    EGCM_IelstFindInt(EGCM_IeListBegin(plsie), EGCM_IeListEnd(plsie), type)
#define EGCM_IeListGetStr(plsie, type) \
    (_CHAR8*)EGCM_MsgGetIeData(plsie, type)

#define EGCM_IeListAddInt(plsie, type, i) \
do{ \
    _UINT32 __uiVal = (_UINT32)(i); \
    __uiVal = eosConvertHostToNetwork32(__uiVal); \
    EGCM_IeListAdd(plsie, type, sizeof(__uiVal), &__uiVal); \
}while(0)

#define EGCM_IeListAddStr(plsie, type, s) \
    EGCM_IeListAdd(plsie, type, strlen(s)+1, s)

/****************************************************************************
 * ST_EGCM_MSGEX api with memory managment
 * if parameter "pstAlloc" is NULL, default memory management will be used!
 * if memory need to expand, value pointed by parameter "ppstMsg" will be changed!
 * member "szMinSize" of "pstAlloc" specify the initial memory size allocated 
 * including the size of struct ST_EGCM_ALLOC
 */

_UINT32 EGCM_MsgExConstruct(ST_EGCM_MSG** ppstMsg, 
                           _UINT32 uiType, 
                            ST_EGCM_ALLOC* pstAlloc);

_UINT32 EGCM_MsgExDestruct(ST_EGCM_MSG* pstMsg);

_UINT32 EGCM_MsgExAddIeInt(ST_EGCM_MSG** ppstMsg, _USHORT16 usType, _UINT32 uiVal);
_UINT32 EGCM_MsgExAddIeStr(ST_EGCM_MSG** ppstMsg, _USHORT16 usType, _CHAR8* pcVal);
_UINT32 EGCM_MsgExAddIe(ST_EGCM_MSG** ppstMsg, _USHORT16 usType, 
                       _USHORT16 usLen, _VOID* data);

#ifdef __cplusplus
}
#endif

#endif


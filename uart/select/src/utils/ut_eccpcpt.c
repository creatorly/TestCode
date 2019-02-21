#include "eos.h"
#include "ut_crypt.h"

/* young 20060405    */
 _UINT32 utEncEccpMsgEncrypt(_VOID** ppMsg, _UINT32* puiLen, _CHAR8* pcKey, _VOID* data)
{
    _UCHAR8* pucMsg = (_UCHAR8*)(*ppMsg);
    _USHORT16 msgLen = (_USHORT16)(*puiLen);
     _INT32 i,iMaxLen;

    if(*pcKey==0)
        return EOS_OK;
    
    if(msgLen>=12)
        iMaxLen=12;
    else
        iMaxLen=msgLen;

    for(i=0;i<iMaxLen;i++)
        pucMsg[i]=pucMsg[i]^*pcKey;

    return EOS_OK;
}

 _UINT32 utEncEccpMsgDecrypt(_VOID** ppMsg, _UINT32* puiLen, _CHAR8* pcKey, _VOID* data)
{
    _UCHAR8* pucMsg = (_UCHAR8*)(*ppMsg);
    _USHORT16 msgLen = (_USHORT16)(*puiLen);
     _INT32 i,iMaxLen;

    if(*pcKey==0)
        return EOS_OK;
    
    if(msgLen>=12)
        iMaxLen=12;
    else
        iMaxLen=msgLen;

    for(i=0;i<iMaxLen;i++)
        pucMsg[i]=pucMsg[i]^*pcKey;

    return EOS_OK;
}



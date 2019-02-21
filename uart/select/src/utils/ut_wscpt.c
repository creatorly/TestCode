
#include "eos.h"
#include "ut_crypt.h"

#if defined(__cplusplus)
extern "C" {
#endif

unsigned char krand()
{
  unsigned char key;
  key = (unsigned char)rand();
  return key&0x7F;
}

/*encrypt rtp header*/
#define WrRtphEncode(rtppack, key) \
do{ \
    unsigned char tmp = (key); \
    (rtppack)[0] ^= ((tmp <<= 1) | ((key) >> 7)); \
    (rtppack)[1] ^= ((tmp <<= 1) | ((key) >> 6)); \
    (rtppack)[2] ^= ((tmp <<= 1) | ((key) >> 5)); \
    (rtppack)[3] ^= ((tmp <<= 1) | ((key) >> 4)); \
}while(0)

_UINT32 utWrTokenCrypt(_VOID** ppMsg, _UINT32* puiLen, _CHAR8* pcKey, _VOID* data)
{
    _UCHAR8* pucMsg = (_UCHAR8*)(*ppMsg);
    _USHORT16 msgLen = (_USHORT16)(*puiLen);
    _USHORT16 keyLen = strlen(pcKey);
    _USHORT16 i = 0;
    
    if (0 == keyLen)
    {
        return EOS_OK;
    }

    for(i=0; i<msgLen; i++)
    {
        pucMsg[msgLen-1-i] ^= pcKey[keyLen-1-i%keyLen];
    }

    return EOS_OK;
}

_UINT32 UT_WsTokenEncrypt(_VOID** ppMsg, _UINT32* puiLen, _CHAR8* pcKey, _VOID* data)
{
    return utWrTokenCrypt(ppMsg, puiLen, pcKey, data);
}

_UINT32 UT_WsTokenDecrypt(_VOID** ppMsg, _UINT32* puiLen, _CHAR8* pcKey, _VOID* data)
{
    return utWrTokenCrypt(ppMsg, puiLen, pcKey, data);
}

_UINT32 UT_WsMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    if (key[0])
    {
        unsigned char* pucRtph = (unsigned char*)(*pkg);
        unsigned char ucKey = krand();

        WrRtphEncode(pucRtph, ucKey);
        *(--pucRtph) = ucKey;
        *pkg = pucRtph;
        ++(*len);
    }

    return EOS_OK;
}

_UINT32 UT_WsMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    if (key[0])
    {
        unsigned char* pucRtph = (unsigned char*)(*pkg);
        unsigned char ucKey = *pucRtph++;

        WrRtphEncode(pucRtph, ucKey);
        *pkg = pucRtph;
        --(*len);
    }

    return EOS_OK;
}

#if defined(__cplusplus)
}
#endif


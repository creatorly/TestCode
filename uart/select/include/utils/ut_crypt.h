
#ifndef UT_CRYPT_H
#define UT_CRYPT_H

#define ENABLE_CCM_ENC_RC4

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef ENABLE_CCM_ENC_WALKERSUN
_UINT32 UT_WsTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_WsTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_WsMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_WsMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
#endif

#ifdef ENABLE_CCM_ENC_RC4
_UINT32 UT_Rc4TokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_Rc4TokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_Rc4MediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_Rc4MediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
#endif

#ifdef ENABLE_CCM_ENC_ECCP
 _UINT32 utEncEccpMsgEncrypt(_VOID** ppMsg, _UINT32* puiLen, _CHAR8* pcKey, _VOID* data);
 _UINT32 utEncEccpMsgDecrypt(_VOID** ppMsg, _UINT32* puiLen, _CHAR8* pcKey, _VOID* data);
#endif

#ifdef ENABLE_CCM_ENC_SEAWOLF
_UINT32 UT_SwTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_SwTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_SwMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_SwMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
#endif

#ifdef ENABLE_CCM_ENC_PCCOM
_UINT32 UT_TeaTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_TeaTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_TeaMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_TeaMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
#endif

#ifdef ENABLE_CCM_ENC_EOS2K
_UINT32 UT_Vos2kTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_Vos2kTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_Vos2kMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_Vos2kMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
#endif

#ifdef ENABLE_CCM_ENC_ERC4
_INT32 UT_ERc4Key(const _CHAR8* pubKey, const _CHAR8* priKey, _CHAR8* key, _INT32 size);
_UINT32 UT_ERc4TokenEnc(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
_UINT32 UT_ERc4TokenDec(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data);
#define UT_ERc4MediaEnc     UT_Rc4MediaEncrypt
#define UT_ERc4MediaDec     UT_Rc4MediaDecrypt
#endif

#if defined(__cplusplus)
}
#endif

#endif


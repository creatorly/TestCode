
#include "eos.h"
#include "utl_rc4.h"
#include "ut_crypt.h"

#if defined(__cplusplus)
extern "C" {
#endif

_UINT32 UT_Rc4TokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utl_rc4_code(*pkg, *len, key, strlen(key), *pkg, *len);
}

_UINT32 UT_Rc4TokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utl_rc4_code(*pkg, *len, key, strlen(key), *pkg, *len);
}

_UINT32 UT_Rc4MediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utl_rc4_code(*pkg, *len, key, strlen(key), *pkg, *len);
}

_UINT32 UT_Rc4MediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utl_rc4_code(*pkg, *len, key, strlen(key), *pkg, *len);
}

#if defined(__cplusplus)
}
#endif


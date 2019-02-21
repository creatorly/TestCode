
#include "eos.h"
#include "ut_crypt.h"
#include "ut_debug.h"

#if defined(__cplusplus)
extern "C" {
#endif

_UINT32 utSwEncrypt(_VOID** pkg, _UINT32* len, 
                    _CHAR8* key, _UINT32 uiPrefixLen, 
                    _VOID* data)
{
    _UINT32 uiKeyLen = strlen(key);

    if (uiKeyLen > 0)
    {
        _CHAR8* pcPkg = (_CHAR8*)(*pkg);
        _UINT32 i = 0;

        for (; i < *len; i++)
        {   
            pcPkg[i] ^= key[i % uiKeyLen];
        }

        pcPkg -= uiPrefixLen;
        pcPkg[6] = 0xff;
        pcPkg[7] = 0xff;

        *pkg = pcPkg;
        *len += uiPrefixLen;
    }
    
    return EOS_OK;
}

_UINT32 utSwDecrypt(_VOID** pkg, _UINT32* len, 
                    _CHAR8* key, _UINT32 uiPrefixLen, 
                    _VOID* data)
{
    _UINT32 uiKeyLen = strlen(key);

    if (uiKeyLen > 0)
    {
        _CHAR8* pcPkg = (_CHAR8*)(*pkg);
        _UINT32 i = 0;

        if (*len < uiPrefixLen)
        {
            *len = 0;
            return EOS_ERROR;
        }

        if ((0xff == (_UCHAR8)pcPkg[6]) && (0xff == (_UCHAR8)pcPkg[7]))
        {
            pcPkg += uiPrefixLen;
            *len -= uiPrefixLen;
            
            for (; i < *len; i++)
            {   
                pcPkg[i] ^= key[i % uiKeyLen];
            }

            *pkg = pcPkg;
        }
    }
    
    return EOS_OK;
}

_UINT32 UT_SwTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utSwEncrypt(pkg, len, key, 18, data);
}

_UINT32 UT_SwTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utSwDecrypt(pkg, len, key, 8, data);
}

_UINT32 UT_SwMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utSwEncrypt(pkg, len, key, 8, data);
}

_UINT32 UT_SwMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    return utSwDecrypt(pkg, len, key, 8, data);
}

#if defined(__cplusplus)
}
#endif


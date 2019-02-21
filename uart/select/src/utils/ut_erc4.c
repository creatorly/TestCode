/********************************************************
* 
*
* xp-workroom IAD platform
* Copyright (C) 2011-2011, All rights reserved
* Designed by xp-workroom Development Team.
* 
* 
* File:         ut_erc4.c
* PACKAGE:      
*   Implement of:       Ejoin RC4 for crypt
*   Date:               01/14/2012
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#include <eos.h>
#include <utl_md5.h>

#include "ut_debug.h"
#include "ut_crypt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UT_ERC4_HEX_STR     "0123456789abcdef"

#define UT_ERC4_PUB_KEY     "^Cst_ERC4_PubKey@PJ_0228$"
#define UT_ERC4_PRI_KEY     "^Cst_ERC4_PriKey@PJ_1121$"

#define UT_ERC4_MAGIC       0x5ca6
#define UT_ERC4_ID          4

typedef struct tagstUtilsERc4Hdr
{
    unsigned short magic;
    unsigned char id;
    char key_len;
    char key[0];
}ST_UT_ERC4_HDR;

int utERc4Key(const char* pubKey, int pubKeyLen, 
              const char* priKey, int priKeyLen,
              char* key, int size)
{
    int len = 0;
    int i;

    utl_md5_ctx_t md5ctx;

    utl_md5_init(&md5ctx);

    if ((pubKey != NULL) && (pubKeyLen > 0))
    {
        utl_md5_update(&md5ctx, (unsigned char*)pubKey, pubKeyLen);
    }

    if ((priKey != NULL) && (priKeyLen > 0))
    {
        utl_md5_update(&md5ctx, (unsigned char*)priKey, priKeyLen);
    }

    utl_md5_update(&md5ctx, (_UCHAR8*)UT_ERC4_PRI_KEY, sizeof(UT_ERC4_PRI_KEY));

    utl_md5_final(&md5ctx);
    
    for (i = 0; i < size/2 && i < sizeof(md5ctx.digest); i++)
    {
        key[len++] = UT_ERC4_HEX_STR[(md5ctx.digest[i] >> 4) & 0xf];
        key[len++] = UT_ERC4_HEX_STR[(md5ctx.digest[i]) & 0xf];
    }

    if (len == size)
    {
        len = size - 1;
    }

    key[len] = 0;

    UT_INFO2("erc4 key = %s, len = %d\r\n", key, len);

    return len;
}

int UT_ERc4Key(const char* pubKey, const char* priKey, char* key, int size)
{
    if (!(pubKey && pubKey[0]))
    {
        pubKey = UT_ERC4_PUB_KEY;
    }

    if (!(priKey && priKey[0]))
    {
        priKey = UT_ERC4_PRI_KEY;
    }
    
    return utERc4Key(pubKey, strlen(pubKey), priKey, strlen(priKey), key, size);
}

_UINT32 UT_ERc4TokenEnc(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    char* indat = (char*)(*pkg);
    const char* call_id;

    ST_UT_ERC4_HDR hdr;
    int iHdrLen;
    
    char acKey[256];
    int iKeyLen;
    char acPubKey[16];
    int iPubKeyLen;

    acPubKey[0] = '\0';
    
    call_id = strstr(indat, "\r\nCall-ID:");
    if (call_id != NULL)
    {
        sscanf(call_id, " Call-ID: %15[^@\r\n]", acPubKey);
    }
    else
    {
        /* short header for Call-ID */
        call_id = strstr(indat, "\r\ni:");
        if (call_id != NULL)
        {
            sscanf(call_id, " i: %15[^@\r\n]", acPubKey);
        }    
    }

    acPubKey[sizeof(acPubKey) - 1] = '\0';
    iPubKeyLen = strlen(acPubKey);

    iHdrLen = sizeof(hdr) + iPubKeyLen;
    indat -= iHdrLen;

    hdr.magic = eosConvertHostToNetwork16(UT_ERC4_MAGIC);
    hdr.id = UT_ERC4_ID;
    hdr.key_len = iPubKeyLen;

    memcpy(indat, &hdr, sizeof(hdr));
    
    indat += sizeof(hdr);

    if (iPubKeyLen > 0)
    {
        memcpy(indat, acPubKey, iPubKeyLen);
        indat += iPubKeyLen;
    }

    /* calculate a new key */
    iKeyLen = utERc4Key(acPubKey, iPubKeyLen, key, strlen(key), acKey, sizeof(acKey));
    acKey[iKeyLen] = '\0';
    
    /* encode using the new key */
    UT_Rc4TokenEncrypt((_VOID*)&indat, len, acKey, data);
    
    *pkg = indat - iHdrLen;
    *len += iHdrLen;
    
    return EOS_OK;
}

_UINT32 UT_ERc4TokenDec(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    unsigned char* u_indat = (unsigned char*)(*pkg);
    char* indat = (char*)(*pkg);
    _UINT32 inlen = (_UINT32)(*len);

    char acKey[256];
    int iPubKeyLen;
    
    if (inlen < sizeof(ST_UT_ERC4_HDR))
    {
        UT_INFO2("invalid erc4! msg = %p, len = %d\r\n", *pkg, inlen);
        return 0;
    }

    if ((u_indat[0] != (UT_ERC4_MAGIC >> 8))
        || (u_indat[1] != (UT_ERC4_MAGIC & 0xff))
        || (u_indat[2] != UT_ERC4_ID))
    {
        UT_INFO4("invalid erc4! hdr = %02x%02x%02x%02x\r\n", 
            u_indat[0], u_indat[1], u_indat[2], u_indat[3]);
        return 0;
    }

    iPubKeyLen = u_indat[3];

    indat += sizeof(ST_UT_ERC4_HDR);
    inlen -= sizeof(ST_UT_ERC4_HDR);

    /* calculate a new key */
    utERc4Key(indat, iPubKeyLen, key, strlen(key), acKey, sizeof(acKey));

    indat += iPubKeyLen;
    inlen -= iPubKeyLen;

    /* decode using the new key */
    UT_Rc4TokenDecrypt((_VOID**)&indat, &inlen, acKey, data);

    *pkg = indat;
    *len = inlen;

    return EOS_OK;
}

#ifdef __cplusplus
}
#endif



#include "eos.h"
#include "ut_crypt.h"
#include "ut_debug.h"
#include "ccm_enc.h"

#include "utl_rc4.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_VOS2K_ID_TERMINATE 0
#define UT_VOS2K_ID_GATEWAY   1

typedef struct tagstUtVos2kCryptHdr
{
    _USHORT16 usProto;
    _USHORT16 usReserved;
    _USHORT16 usArith;
    _USHORT16 usLen;
}ST_UT_VOS2K_HDR;

typedef struct tagstUtVos2kCryptId
{
    _UCHAR8 ucType;
    _UCHAR8 ucLen;
    _CHAR8  acData[0];
}ST_UT_VOS2K_ID;

extern _BOOL sipIsFxoByUserName(const _CHAR8* username);
extern const _CHAR8* sipGetPwdByUserName(_CHAR8* username, _BOOL* pbIsFxo);
    
char* utVos2kMsgGetUserName(const char* msg, char* username)
{
    char* tmp = NULL;
    char* begin = NULL;
    char* end = NULL;
    const char* user = NULL;
    
    *username = '\0';

    if (!strncmp(msg, "SIP/2.0", sizeof("SIP/2.0") - 1))
    {
        /* message is sip response(begin with "SIP/2.0") */
        user = "To:";
    }
    else
    {
        /* message is sip request(begin with sip METHOD) */
        user = "From:";
    }

    tmp = strstr(msg, user);
    if (NULL == tmp)
        return NULL;

    tmp = strstr(tmp, "sip");
    if (NULL == tmp)
        return NULL;

    tmp = strchr(tmp+sizeof("sip")-1, ':');
    if (NULL == tmp)
        return NULL;

    begin = tmp + 1;
    end = strchr(begin+1, '@');
    if (NULL == end)
        return NULL;
    
    memcpy(username, begin, end - begin);
    username[end-begin] = '\0';

    return username;
}

unsigned char utVos2kRand(unsigned int max)
{
  unsigned int key;
  key = (unsigned int)rand();
  if (max != 0)
      key %= max;
  return key;
}

void utVos2kXor(const char* data, int len, char* key, char* buf)
{
    int i;
    int size = strlen(key);

    /* to avoid the bug because of null password */
    if (size > 0)
    {
        for (i=0; i<len; i++)
        {
            buf[i] = data[i] ^ key[i % size];
        }
    }
}

_UINT32 UT_Vos2kTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    _CHAR8* msg = (_CHAR8*)(*pkg);
    _CHAR8 acUserName[32];
    const _CHAR8* pcPwd = NULL;
    _BOOL bIsFxo = EOS_FALSE;

    _INT32 iHdrLen = 0;
    _INT32 iDatLen = 0;

    ST_UT_VOS2K_HDR hdr;
    ST_UT_VOS2K_ID id;

    utVos2kMsgGetUserName(msg, acUserName);

#ifdef ACOM_INCLUDE_SIP
    pcPwd = sipGetPwdByUserName(acUserName, &bIsFxo);
#endif
    if (NULL == pcPwd)
    {
        return EOS_ERROR;
    }

    id.ucType = bIsFxo ? UT_VOS2K_ID_GATEWAY : UT_VOS2K_ID_TERMINATE;
    id.ucLen = strlen(acUserName);
    
    iDatLen = sizeof(ST_UT_VOS2K_ID) + id.ucLen + (*len);

    hdr.usProto = eosConvertHostToNetwork16(0x4b53);
    hdr.usReserved = 0;
    hdr.usArith = eosConvertHostToNetwork16(0x0001);
    hdr.usLen = eosConvertHostToNetwork16(iDatLen);

    iHdrLen =  sizeof(ST_UT_VOS2K_HDR) + sizeof(ST_UT_VOS2K_ID) + id.ucLen;
    msg -= iHdrLen;
    
    memcpy(msg, &hdr, sizeof(ST_UT_VOS2K_HDR));
    msg += sizeof(ST_UT_VOS2K_HDR);
    memcpy(msg, &id, sizeof(ST_UT_VOS2K_ID));
    msg += sizeof(ST_UT_VOS2K_ID);
    memcpy(msg, acUserName, id.ucLen);
    msg += id.ucLen;
    
    utl_rc4_code(*pkg, *len, pcPwd, strlen(pcPwd), *pkg, *len);

    *pkg = msg - iHdrLen;
    *len += iHdrLen;
    
    return EOS_OK;
}

_UINT32 UT_Vos2kTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    _CHAR8* msg = (_CHAR8*)(*pkg);
    _CHAR8 acUserName[32];
    const _CHAR8* pcPwd = NULL;

    _INT32 iDatLen = 0;

    ST_UT_VOS2K_HDR hdr;
    ST_UT_VOS2K_ID id;

    memcpy(&hdr, msg, sizeof(ST_UT_VOS2K_HDR));
    msg += sizeof(ST_UT_VOS2K_HDR);
    memcpy(&id, msg, sizeof(ST_UT_VOS2K_ID));
    msg += sizeof(ST_UT_VOS2K_ID);

    hdr.usProto = eosConvertNetworkToHost16(hdr.usProto);
    hdr.usReserved = eosConvertNetworkToHost16(hdr.usReserved);
    hdr.usArith = eosConvertNetworkToHost16(hdr.usArith);
    hdr.usLen = eosConvertNetworkToHost16(hdr.usLen);

    if (hdr.usProto != 0x4b53)
    {
        UT_ERROR(EOS_ERROR, "vos2000 invalid protocal field!\r\n");
        return EOS_ERROR;
    }

    if (hdr.usArith != 0x0001)
    {
        UT_ERROR(EOS_ERROR, "vos2000 unsupported encrypt arithmetic!\r\n");
        return EOS_ERROR;
    }
        
    if ((id.ucType != UT_VOS2K_ID_TERMINATE) && (id.ucType != UT_VOS2K_ID_GATEWAY))
    {
        UT_ERROR(EOS_ERROR, "vos2000 invalid device type!\r\n");
        return EOS_ERROR;
    }

    memcpy(acUserName, msg, id.ucLen);
    acUserName[id.ucLen] = '\0'; 

#ifdef ACOM_INCLUDE_SIP
    pcPwd = sipGetPwdByUserName(acUserName, NULL);
#endif
    if (NULL == pcPwd)
    {
        return EOS_ERROR;
    }
    
    msg += id.ucLen;
    
    iDatLen = hdr.usLen - sizeof(ST_UT_VOS2K_ID) - id.ucLen;
    utl_rc4_code(msg, iDatLen, pcPwd, strlen(pcPwd), msg, iDatLen);

    *pkg = msg;
    *len = iDatLen;

    return EOS_OK;
}

_UINT32 UT_Vos2kMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    char* msg = (char*)(*pkg);

    msg[-1] = 0;
    msg[0] = *len;

    utVos2kXor(&msg[1], *len-1, key, &msg[1]);

    *pkg = msg - 1;
    *len += 1;
    
    return EOS_OK;
}

_UINT32 UT_Vos2kMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    char* msg = (char*)(*pkg);

    if ((msg[0] != 0) || ((_UCHAR8)msg[1] != (*len - 1)))
    {
        return EOS_ERROR;
    }
    
    *len = msg[1];
    
    msg[0] = 0x80;
    
    utVos2kXor(&msg[2], *len-1, key, &msg[1]);

    return EOS_OK;
}

#if defined(__cplusplus)
}
#endif


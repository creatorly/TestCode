
#include "eos.h"
#include "ut_crypt.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define DELTA 0x9e3779b9 /* sqr(5)-1 * 2^31 */


/******************************************************************************
Func:  tea_encipher_new
Desc:  ��8���Լ������Ľ���128λ��tea����
Input: const unsigned long *const v ---�����8���ֽڵ�����
        const unsigned long * const k---��Կ
Output: unsigned long *const w---���ܺ��8���ֽڵ�����
Return:
******************************************************************************/
void tea_encipher_new(const unsigned long *const v,unsigned long *const w,
           const unsigned long * const k , unsigned long rounds)
{
    register unsigned long       n=rounds,sum=0;
    unsigned long       y,z;
    
    memcpy(&y, (unsigned char*)v, 4);
    memcpy(&z, (unsigned char*)v+4,4);
    y = htonl(y);
    z = htonl(z);
    
    while(n-->0)
    {
      y += ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[sum&3];
      sum += DELTA;
      z += ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[sum>>11 & 3];
    }
    
    y = htonl(y);
    z = htonl(z);
    memcpy((unsigned char*)w, &y, 4);
    memcpy((unsigned char*)w+4, &z, 4);
}

/******************************************************************************
Func:  tea_decipher_new
Desc:  ��8���Լ������Ľ���tea����
Input: const unsigned long *const v ---�����8���ֽڵ�����
        const unsigned long * const k---��Կ
Output: unsigned long *const w---�����ܺ��8���ֽڵ�����
Return:
******************************************************************************/
void tea_decipher_new(const unsigned long *const v,unsigned long *const w,
   const unsigned long * const k, unsigned long rounds)
{
    register unsigned long       sum=DELTA*rounds, n=rounds;
    unsigned long       y,z;
    /* sum = delta<<5, in general sum = delta * n */
    memcpy(&y, (unsigned char*)v, 4);
    memcpy(&z, (unsigned char*)v+4,4);
    y = htonl(y);
    z = htonl(z);
    
    while(n-->0)
    {
      z -= ((y << 4) ^ (y >> 5)) + (y ^ sum) + k[sum>>11 & 3];
      sum -= DELTA;
      y -= ((z << 4) ^ (z >> 5)) + (z ^ sum) + k[sum&3];
    }
    
    y = htonl(y);
    z = htonl(z);
    memcpy((unsigned char*)w, &y, 4);
    memcpy((unsigned char*)w+4, &z, 4);
}

/******************************************************************************
Func:  tea_encrypt_fixedkey
Desc:  
     ����tea�㷨�����ݽ��м��ܣ���������ĵĳ��ȱ���Ϊ8�ֽڵı���
     ���ù̶�����Կ
Input: 
     unsigned char *InputBuf -- ���ģ����ȱ���Ϊ8�ֽڵı���
     unsigned long ulLen-- ���ĺ����ĵĳ���
Output:
     char *OutputBuf -- ����,���ܺ�����ĵĳ��Ⱥ����ĵĳ�����һ����
Return:
      N/A
******************************************************************************/

int tea_encrypt_fixedkey(unsigned char * InputBuf, unsigned long ulLen,
         unsigned char * OutputBuf)
{
    unsigned long key[4] = {15014,13102,18110,98476};
    unsigned long i;

    if(ulLen % 8)
        return -1;
 
    for(i = 0; i < ulLen; i = i + 8)
    {
        tea_encipher_new((unsigned long*)(InputBuf + i),(unsigned long*)(OutputBuf + i),key,8);    
    }

    return 0;
}


/******************************************************************************
Func:  tea_decrypt_fixedkey
Desc:  
     �Բ���tea�㷨���м��ܵ����ݽ��ܣ���������ĵĳ��ȱ���Ϊ8�ֽڵı���
     ���ù̶�����Կ
Input: 
     unsigned char *InputBuf -- �����ģ����ȱ���Ϊ8�ֽڵı���
     unsigned long ulLen-- ���ĺ����ĵĳ���
Output:
     char *OutputBuf -- ����,���ܺ�����ĵĳ��Ⱥ����ĵĳ�����һ����
Return:
      N/A
******************************************************************************/

int tea_decrypt_fixedkey(unsigned char * InputBuf, unsigned long ulLen,
     unsigned char * OutputBuf)
{
    unsigned long key[4] = {15014,13102,18110,98476};
    unsigned long i;

    if(ulLen % 8)
        return -1;

    for(i = 0; i < ulLen; i = i + 8)
    {
        tea_decipher_new((unsigned long*)(InputBuf + i),(unsigned long*)(OutputBuf + i),key,8);    
    }

    return 0;
}


/*********************************************************************************************
 * ����SIP����,��������ĩβ��'\0'�ַ�.
 */
_UINT32 UT_TeaTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{    /* �ⲿ���뱣֤o_buf�㹻�� */
    unsigned char* p = (unsigned char*)(*pkg);
    int i_len = strlen((char*)p) + 1;
    int o_len = EOS_ROUND_UP(i_len, 8);

    p -= (o_len - i_len);

    /* ��ͷ�����ո�,��ʹ���ȴﵽ8�������� */
    memset(p, ' ', o_len - i_len);

    /* ������������������ʹ��ͬһ�� */
    tea_encrypt_fixedkey(p, o_len, p);

    *pkg = p;
    *len = o_len;

    return EOS_OK;
}

_UINT32 UT_TeaTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    unsigned char* p = (unsigned char*)(*pkg);
    
    /* ���뻺�����ĳ��ȱ�����8�������� */
    if(*len % 8) {
        return EOS_ERROR;
    }
    
    /* ������������������ʹ��ͬһ�� */
    tea_decrypt_fixedkey(p, *len, p);

    /* ��ȥ����ǰ��Ŀո� */
    while (' ' == *p)
    {
        ++p;

    }

    *len -= ((char*)p - (char*)(*pkg));
    *pkg = p;

    return EOS_OK;
}

_UINT32 UT_TeaMediaEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{    
    if (key[0])
    {
        /* �����������ֽ���,�������contributorCount�� */
        int rtp_cc = EOS_ROUND_UP(*len, 8) - *len;
        unsigned char* out_data = (unsigned char*)(*pkg);

        out_data[0] = (out_data[0] & 0xF0) | (rtp_cc & 0x0F);

        if(rtp_cc > 0) {
            /* ���RTP���ĵĳ��Ȳ���8��������,��0�����ĩβ */
            memset(out_data + *len, 0, rtp_cc);
        }

        *len += rtp_cc;
        tea_encrypt_fixedkey(out_data, *len, out_data);
    }
    
    return EOS_OK;
}

_UINT32 UT_TeaMediaDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    if (key[0])
    {
        if((*len % 8) == 0) {
            unsigned char* out_data = (unsigned char*)(*pkg);
            
            /* ����RTP��,���Ұ�β��������ֽڶ���(������contributorCount��) */
            tea_decrypt_fixedkey(out_data, *len, out_data);

            /* ����ȥ��RTPͷ������ֽں�����ݳ��� */
            *len -= (out_data[0] & 0x0F);

            /* ���contributorCount�� */
            out_data[0] = out_data[0] & 0xF0;
        }
        else {
            *len = 0;
        }
    }
    
    return EOS_OK;
}

#if defined(__cplusplus)
}
#endif


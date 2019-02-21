
#include "eos.h"
#include "ut_crypt.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define DELTA 0x9e3779b9 /* sqr(5)-1 * 2^31 */


/******************************************************************************
Func:  tea_encipher_new
Desc:  对8个自己的明文进行128位的tea加密
Input: const unsigned long *const v ---输入的8个字节的明文
        const unsigned long * const k---密钥
Output: unsigned long *const w---加密后的8个字节的密文
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
Desc:  对8个自己的密文进行tea解密
Input: const unsigned long *const v ---输入的8个字节的密文
        const unsigned long * const k---密钥
Output: unsigned long *const w---解密密后的8个字节的明文
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
     采用tea算法对数据进行加密，输入的明文的长度必须为8字节的倍数
     采用固定的密钥
Input: 
     unsigned char *InputBuf -- 明文，长度必须为8字节的倍数
     unsigned long ulLen-- 明文和密文的长度
Output:
     char *OutputBuf -- 密文,加密后的密文的长度和明文的长度是一样的
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
     对采用tea算法进行加密的数据解密，输入的密文的长度必须为8字节的倍数
     采用固定的密钥
Input: 
     unsigned char *InputBuf -- 密文文，长度必须为8字节的倍数
     unsigned long ulLen-- 明文和密文的长度
Output:
     char *OutputBuf -- 明文,解密后的明文的长度和密文的长度是一样的
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
 * 加密SIP报文,包括报文末尾的'\0'字符.
 */
_UINT32 UT_TeaTokenEncrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{    /* 外部必须保证o_buf足够长 */
    unsigned char* p = (unsigned char*)(*pkg);
    int i_len = strlen((char*)p) + 1;
    int o_len = EOS_ROUND_UP(i_len, 8);

    p -= (o_len - i_len);

    /* 在头部填充空格,以使长度达到8的整数倍 */
    memset(p, ' ', o_len - i_len);

    /* 输入和输出缓冲区可以使用同一个 */
    tea_encrypt_fixedkey(p, o_len, p);

    *pkg = p;
    *len = o_len;

    return EOS_OK;
}

_UINT32 UT_TeaTokenDecrypt(_VOID** pkg, _UINT32* len, _CHAR8* key, _VOID* data)
{
    unsigned char* p = (unsigned char*)(*pkg);
    
    /* 输入缓冲区的长度必须是8的整数倍 */
    if(*len % 8) {
        return EOS_ERROR;
    }
    
    /* 输入和输出缓冲区可以使用同一个 */
    tea_decrypt_fixedkey(p, *len, p);

    /* 除去报文前面的空格 */
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
        /* 计算加密填充字节数,并存放在contributorCount中 */
        int rtp_cc = EOS_ROUND_UP(*len, 8) - *len;
        unsigned char* out_data = (unsigned char*)(*pkg);

        out_data[0] = (out_data[0] & 0xF0) | (rtp_cc & 0x0F);

        if(rtp_cc > 0) {
            /* 如果RTP报文的长度不是8的整数倍,用0填充在末尾 */
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
            
            /* 解密RTP包,并且把尾部的填充字节丢弃(长度在contributorCount中) */
            tea_decrypt_fixedkey(out_data, *len, out_data);

            /* 计算去除RTP头和填充字节后的数据长度 */
            *len -= (out_data[0] & 0x0F);

            /* 清除contributorCount域 */
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


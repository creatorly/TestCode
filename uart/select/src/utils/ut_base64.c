
#include "eos.h"
#include "ut_base64.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define UT_BASE64_CHARS   \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

#define UT_BASE64_LINELEN       76
#define UT_BASE64_UTF8_LINELEN  (UT_BASE64_LINELEN*3/4)

#define UT_BASE64_DEC(b64)  g_acUtBase64DecTab[(_UCHAR8)(b64)]

static _CHAR8 g_acUtBase64DecTab[128] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    62,             /* '+' */ 
    -1, -1, -1, 
    63,             /* '/' */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, /* '0'-'9' */
    -1, -1, -1, -1, -1, -1, -1,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, /* 'A'-'Z' */
    -1, -1, -1, -1, -1, -1,
    26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, /* 'a'-'z' */
    -1, -1, -1, -1, -1
};
    
/* UTF8 --> BASE64 */
_INT32 UT_Base64Encode(const _UCHAR8* utf8, _UINT32 len, _CHAR8* base64)
{
    _UINT32 i = 0;
    _INT32 j = 0;

    while (len >= UT_BASE64_UTF8_LINELEN)
    {
        for (i = 0; i < UT_BASE64_UTF8_LINELEN; i += 3)
        {
            base64[j++] = UT_BASE64_CHARS[utf8[i] >> 2];
            base64[j++] = UT_BASE64_CHARS[((utf8[i] & 0x03) << 4) | (utf8[i+1] >> 4)];
            base64[j++] = UT_BASE64_CHARS[((utf8[i+1] & 0x0f) << 2) | (utf8[i+2] >> 6)];
            base64[j++] = UT_BASE64_CHARS[utf8[i+2] & 0x3f];
        }

        if (len > UT_BASE64_UTF8_LINELEN)
        {
            base64[j++] = '\r';
            base64[j++] = '\n';
        }
        
        utf8 += UT_BASE64_UTF8_LINELEN;
        len -= UT_BASE64_UTF8_LINELEN;
    }

    if (len > 0)
    {
        for (i = 0; i < len/3*3; i += 3)
        {
            base64[j++] = UT_BASE64_CHARS[utf8[i] >> 2];
            base64[j++] = UT_BASE64_CHARS[((utf8[i] & 0x03) << 4) | (utf8[i+1] >> 4)];
            base64[j++] = UT_BASE64_CHARS[((utf8[i+1] & 0x0f) << 2) | (utf8[i+2] >> 6)];
            base64[j++] = UT_BASE64_CHARS[utf8[i+2] & 0x3f];
        }

        if (i + 1 == len)
        {
            /* one byte left, append two '=' */
            base64[j++] = UT_BASE64_CHARS[utf8[i] >> 2];
            base64[j++] = UT_BASE64_CHARS[(utf8[i] & 0x03) << 4];
            base64[j++] = '=';
            base64[j++] = '=';
        }
        else if (i + 2 == len)
        {
            /* two byte left, append one '=' */
            base64[j++] = UT_BASE64_CHARS[utf8[i] >> 2];
            base64[j++] = UT_BASE64_CHARS[((utf8[i] & 0x03) << 4) | (utf8[i+1] >> 4)];
            base64[j++] = UT_BASE64_CHARS[((utf8[i+1] & 0x0f) << 2)];
            base64[j++] = '=';
        }
    }

    return j;
}

/* BASE64 --> UTF8 */
_INT32 UT_Base64Decode(const _CHAR8* base64, _UINT32 len, _UCHAR8* utf8)
{
    _UINT32 i = 0;
    _INT32 j = 0;
    _UINT32 mod3 = 0;
    _CHAR8 indexs[4];

    /* ignore the last "\r\n" */
    while ((len > 0) && (('\r' == base64[len-1]) || ('\n' == base64[len-1])))
    {
        --len;
    }

    /* ignore the suffix "==" or "=" */
    if ((len > 0) && ('=' == base64[len-1]))
    {
        if ((len > 1) && ('=' == base64[len-2]))
        {
            mod3 = 1;
            len -= 2;
        }
        else
        {
            mod3 = 2;
            --len;
        }
    }

    /* check the validation of the base64 code */
    for (i = 0; i < len; ++i)
    {
        if (base64[i] & 0x80)
        {
            printf("UT_Base64Decode: invalid base64: 0x%02x\r\n", base64[i]);
            return -1;
        }

        if (UT_BASE64_DEC(base64[i]) & 0x80)
        {
            if ((base64[i] != '\r') && (base64[i] != '\n'))
            {
                printf("UT_Base64Decode: invalid base64: 0x%02x\r\n", base64[i]);
                return -1;
            }
        }
    }

    while (len >= UT_BASE64_LINELEN)
    {
        for (i = 0; i < UT_BASE64_LINELEN; i += 4)
        {
            indexs[0] = UT_BASE64_DEC(base64[i]);
            indexs[1] = UT_BASE64_DEC(base64[i+1]);
            indexs[2] = UT_BASE64_DEC(base64[i+2]);
            indexs[3] = UT_BASE64_DEC(base64[i+3]);
            
            utf8[j++] = (indexs[0] << 2) | ((indexs[1] & 0x30) >> 4);
            utf8[j++] = ((indexs[1] & 0x0f) << 4) | ((indexs[2] & 0x3c) >> 2);
            utf8[j++] = ((indexs[2] & 0x03) << 6) | (indexs[3]);
        }

        base64 += UT_BASE64_LINELEN;
        len -= UT_BASE64_LINELEN;

        if ((len > 0) && ('\n' == base64[0]))
        {
            base64++;
            len--;
        }
        else if ((len > 1) && ('\r' == base64[0]) && ('\n' == base64[1]))
        {
            base64 += 2;
            len -= 2;
        }
    }

    if (len > 0)
    {
        for (i = 0; i < (len & ~3); i += 4)
        {
            indexs[0] = UT_BASE64_DEC(base64[i]);
            indexs[1] = UT_BASE64_DEC(base64[i+1]);
            indexs[2] = UT_BASE64_DEC(base64[i+2]);
            indexs[3] = UT_BASE64_DEC(base64[i+3]);
            
            utf8[j++] = (indexs[0] << 2) | ((indexs[1] & 0x30) >> 4);
            utf8[j++] = ((indexs[1] & 0x0f) << 4) | ((indexs[2] & 0x3c) >> 2);
            utf8[j++] = ((indexs[2] & 0x03) << 6) | (indexs[3]);
        }

        if ((1 == mod3) && (i + 2 == len))
        {
            indexs[0] = UT_BASE64_DEC(base64[i]);
            indexs[1] = UT_BASE64_DEC(base64[i+1]);

            utf8[j++] = (indexs[0] << 2) | ((indexs[1] & 0x30) >> 4);
        }
        else if ((2 == mod3) && (i + 3 == len))
        {
            indexs[0] = UT_BASE64_DEC(base64[i]);
            indexs[1] = UT_BASE64_DEC(base64[i+1]);
            indexs[2] = UT_BASE64_DEC(base64[i+2]);

            utf8[j++] = (indexs[0] << 2) | ((indexs[1] & 0x30) >> 4);
            utf8[j++] = ((indexs[1] & 0x0f) << 4) | ((indexs[2] & 0x3c) >> 2);
        }
    }

    return j;
}

#if defined(__cplusplus)
}
#endif


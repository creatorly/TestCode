
#ifndef __UT_CHARSET_H__
#define __UT_CHARSET_H__

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef UT_ENABLE_GB2312
int UT_GB2312ToUTF8(const char* gb2312, int len, char* utf8, int size);
int UT_GB2312ToUniCode(const char* gb2312, int len, char* uc, int size);
int UT_GB2312ToHzGB2312(const char* gb2312, int len, char* hz_gb, int size);
int UT_GB2312ToAscii(const char* gb2312, int len, char* ascii, int size);

int UT_UniCodeToGB2312(const char* uc, int len, char* gb2312, int size);
int UT_HzGB2312ToGB2312(const char* hz_gb, int len, char* gb2312, int size);
#endif

int UT_UTF8ToUniCode(const char* utf8, int len, char* uc, int size);
int UT_UTF8ToAscii(const char* utf8, int len, char* ascii, int size);

int UT_UniCodeToUTF8(const char* uc, int len, char* utf8, int size);
int UT_UniCodeToAscii(const char* uc, int len, char* ascii, int size);

int UT_StrToJsJson(const char* s, char* json, int size);
int UT_StrToJson(const char* s, char* json, int size);

#if defined(__cplusplus)
}
#endif

#endif


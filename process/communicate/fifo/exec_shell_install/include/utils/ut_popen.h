#ifndef  __UT_POPEN_H__
#define  __UT_POPEN_H__

#ifdef __cplusplus
extern "C" {
#endif

int UT_Popen(const char* cmd, char* outbuf, int osize);
int UT_PopenNolog(const char* cmd, char* outbuf, int osize);

#ifdef __cplusplus
}
#endif

#endif

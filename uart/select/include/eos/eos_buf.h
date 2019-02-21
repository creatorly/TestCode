
#ifndef __EOS_BUF_H__
#define __EOS_BUF_H__

#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif 

#define eos_buf_strcpy(b, s) eos_buf_strncpy(b, s, sizeof(b))

#define eos_buf_strncpy(b, s, n) \
    do { \
        eos_strncpy(b, s, n); \
        (b)[n - 1] = '\0'; \
    } while(0)

#if defined(__cplusplus)
}
#endif 

#endif


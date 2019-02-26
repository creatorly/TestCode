
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define EOS_STR_DEBUG

#ifdef EOS_STR_DEBUG

#define EOS_MEMCPY_DEBUG
#define EOS_MEMSET_DEBUG
#define EOS_MEMCMP_DEBUG
#define EOS_MEMMOVE_DEBUG

#define EOS_STRLEN_DEBUG
#define EOS_STRCPY_DEBUG
#define EOS_STRNCPY_DEBUG
#define EOS_STRCAT_DEBUG
#define EOS_STRNCAT_DEBUG
#define EOS_STRCMP_DEBUG
#define EOS_STRNCMP_DEBUG
#define EOS_STRCASECMP_DEBUG
#define EOS_STRNCASECMP_DEBUG
#define EOS_STRCHR_DEBUG
#define EOS_STRRCHR_DEBUG
#define EOS_STRSTR_DEBUG

#define EOS_PRINTF_DEBUG
#define EOS_FPRINTF_DEBUG
#define EOS_SPRINTF_DEBUG
#define EOS_SNPRINTF_DEBUG

#define EOS_VPRINTF_DEBUG
#define EOS_VFPRINTF_DEBUG
#define EOS_VSPRINTF_DEBUG
#define EOS_VSNPRINTF_DEBUG

#define EOS_SCANF_DEBUG
#define EOS_FSCANF_DEBUG
#define EOS_SSCANF_DEBUG

/* if defined(EOS_MEMCPY_DEBUG) and len > this value, a syslog will be wrote */
#define EOS_MEMCPY_MAX      1024
#define EOS_MEMSET_MAX      1024
#define EOS_MEMMOVE_MAX     1024

#define EOS_STRNCPY_MAX     1024
#define EOS_STRNCAT_MAX     1024

#define EOS_SNPRINTF_MAX    1024
#define EOS_VSNPRINTF_MAX   1024

void* __eos_memcpy(const char* fi, int li, void* dest, const void* src, size_t n);
void* __eos_memset(const char* fi, int li, void* s, int c, size_t n);
void* __eos_memmove(const char* fi, int li, void *dest, const void *src, size_t n);
int __eos_memcmp(const char* fi, int li, const void *s1, const void *s2, size_t n);

size_t __eos_strlen(const char* fi, int li, const char *s);
char* __eos_strcpy(const char* fi, int li, char *dest, const char *src);
char* __eos_strncpy(const char* fi, int li, char *dest, const char *src, size_t n);
char* __eos_strcat(const char* fi, int li, char *dest, const char *src);
char* __eos_strncat(const char* fi, int li, char *dest, const char *src, size_t n);

int __eos_strcmp(const char* fi, int li, const char *s1, const char *s2);
int __eos_strncmp(const char* fi, int li, const char *s1, const char *s2, size_t n);
int __eos_strcasecmp(const char* fi, int li, const char *s1, const char *s2);
int __eos_strncasecmp(const char* fi, int li, const char *s1, const char *s2, size_t n);

char* __eos_strchr(const char* fi, int li, const char *s, int c);
char* __eos_strrchr(const char* fi, int li, const char *s, int c);
char* __eos_strstr(const char* fi, int li, const char *s, const char *ss);

#ifdef __GNUC__
#define EOS_CHK_FMTARGS(fmt_pos) \
            __attribute__((format(printf, fmt_pos, fmt_pos+1)))
#define EOS_CHK_FMTINARGS(fmt_pos) \
            __attribute__((format(scanf, fmt_pos, fmt_pos+1)))
#else
#define EOS_CHK_FMTARGS(fmt_pos)
#define EOS_CHK_FMTINARGS(fmt_pos)
#endif

int EOS_CHK_FMTARGS(3) 
    __eos_printf(const char* fi, int li, const char *format, ...);
int EOS_CHK_FMTARGS(4) 
    __eos_fprintf(const char* fi, int li, FILE *stream, const char *format, ...);
int EOS_CHK_FMTARGS(4) 
    __eos_sprintf(const char* fi, int li, char *str, const char *format, ...);
int EOS_CHK_FMTARGS(5) 
    __eos_snprintf(const char* fi, int li, char *str, size_t size, const char *format, ...);

int __eos_vprintf(const char* fi, int li, const char *format, va_list ap);
int __eos_vfprintf(const char* fi, int li, FILE *stream, const char *format, va_list ap);
int __eos_vsprintf(const char* fi, int li, char *str, const char *format, va_list ap);
int __eos_vsnprintf(const char* fi, int li, char *str, size_t size, const char *format, va_list ap);

int EOS_CHK_FMTINARGS(3) 
    __eos_scanf(const char* fi, int li, const char *format, ...);
int EOS_CHK_FMTINARGS(4) 
    __eos_fscanf(const char* fi, int li, FILE *stream, const char *format, ...);
int EOS_CHK_FMTINARGS(4) 
    __eos_sscanf(const char* fi, int li, const char *str, const char *format, ...);

#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_MEMCPY_DEBUG)
#define eos_memcpy(d, s, n)     __eos_memcpy(__FILE__, __LINE__, d, s, n)
#else
#define eos_memcpy(d, s, n)     memcpy(d, s, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_MEMSET_DEBUG)
#define eos_memset(s, c, n)     __eos_memset(__FILE__, __LINE__, s, c, n)
#else
#define eos_memset(s, c, n)     memset(s, c, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_MEMCMP_DEBUG)
#define eos_memcmp(s1, s2, n)   __eos_memcmp(__FILE__, __LINE__, s1, s2, n)
#else
#define eos_memcmp(s1, s2, n)   memcmp(s1, s2, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_MEMMOVE_DEBUG)
#define eos_memmove(d, s, n)    __eos_memmove(__FILE__, __LINE__, d, s, n)
#else
#define eos_memmove(d, s, n)    memmove(d, s, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRLEN_DEBUG)
#define eos_strlen(s)           __eos_strlen(__FILE__, __LINE__, s)
#else
#define eos_strlen(s)           strlen(s)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRCPY_DEBUG)
#define eos_strcpy(d, s)        __eos_strcpy(__FILE__, __LINE__, d, s)
#else
#define eos_strcpy(d, s)        strcpy(d, s)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRNCPY_DEBUG)
#define eos_strncpy(d, s, n)    __eos_strncpy(__FILE__, __LINE__, d, s, n)
#else
#define eos_strncpy(d, s, n)    strncpy(d, s, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRCAT_DEBUG)
#define eos_strcat(d, s)        __eos_strcat(__FILE__, __LINE__, d, s)
#else
#define eos_strcat(d, s)        strcat(d, s)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRNCAT_DEBUG)
#define eos_strncat(d, s, n)    __eos_strncat(__FILE__, __LINE__, d, s, n)
#else
#define eos_strncat(d, s, n)    strncat(d, s, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRCMP_DEBUG)
#define eos_strcmp(s1, s2)      __eos_strcmp(__FILE__, __LINE__, s1, s2)
#else
#define eos_strcmp(s1, s2)      strcmp(s1, s2)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRNCMP_DEBUG)
#define eos_strncmp(s1, s2, n)  __eos_strncmp(__FILE__, __LINE__, s1, s2, n)
#else
#define eos_strncmp(s1, s2, n)  strncmp(s1, s2, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRCASECMP_DEBUG)
#define eos_strcasecmp(s1, s2)  __eos_strcasecmp(__FILE__, __LINE__, s1, s2)
#else
#define eos_strcasecmp(s1, s2)  strcasecmp(s1, s2)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRNCASECMP_DEBUG)
#define eos_strncasecmp(s1, s2, n)  __eos_strncasecmp(__FILE__, __LINE__, s1, s2, n)
#else
#define eos_strncasecmp(s1, s2, n)  strncasecmp(s1, s2, n)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRCHR_DEBUG)
#define eos_strchr(s, c)        __eos_strchr(__FILE__, __LINE__, s, c)
#else
#define eos_strchr(s, c)        strchr(s, c)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRRCHR_DEBUG)
#define eos_strrchr(s, c)       __eos_strrchr(__FILE__, __LINE__, s, c)
#else
#define eos_strrchr(s, c)       strrchr(s, c)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_STRSTR_DEBUG)
#define eos_strstr(s, ss)       __eos_strstr(__FILE__, __LINE__, s, ss)
#else
#define eos_strstr(s, ss)       strstr(s, ss)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_PRINTF_DEBUG)
#define eos_printf(fmt, arg...) __eos_printf(__FILE__, __LINE__, fmt, ##arg)
#else
#define eos_printf(fmt, arg...) printf(fmt, ##arg)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_FPRINTF_DEBUG)
#define eos_fprintf(fp, fmt, arg...)    __eos_fprintf(__FILE__, __LINE__, fp, fmt, ##arg)
#else
#define eos_fprintf(fp, fmt, arg...)    fprintf(fp, fmt, ##arg)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_SPRINTF_DEBUG)
#define eos_sprintf(s, fmt, arg...)     __eos_sprintf(__FILE__, __LINE__, s, fmt, ##arg)
#else
#define eos_sprintf(s, fmt, arg...)     sprintf(s, fmt, ##arg)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_SNPRINTF_DEBUG)
#define eos_snprintf(s, n, fmt, arg...) __eos_snprintf(__FILE__, __LINE__, s, n, fmt, ##arg)
#else
#define eos_snprintf(s, n, fmt, arg...) snprintf(s, n, fmt, ##arg)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_VPRINTF_DEBUG)
#define eos_vprintf(fmt, ap)        __eos_vprintf(__FILE__, __LINE__, fmt, ap)
#else
#define eos_vprintf(fmt, ap)        vprintf(fmt, ap)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_VFPRINTF_DEBUG)
#define eos_vfprintf(fp, fmt, ap)    __eos_vfprintf(__FILE__, __LINE__, fp, fmt, ap)
#else
#define eos_vfprintf(fp, fmt, ap)    vfprintf(fp, fmt, ap)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_VSPRINTF_DEBUG)
#define eos_vsprintf(s, fmt, ap)     __eos_vsprintf(__FILE__, __LINE__, s, fmt, ap)
#else
#define eos_vsprintf(s, fmt, ap)     vsprintf(s, fmt, ap)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_VSNPRINTF_DEBUG)
#define eos_vsnprintf(s, n, fmt, ap) __eos_vsnprintf(__FILE__, __LINE__, s, n, fmt, ap)
#else
#define eos_vsnprintf(s, n, fmt, ap) vsnprintf(s, n, fmt, ap)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_SCANF_DEBUG)
#define eos_scanf(fmt, arg...)      __eos_scanf(__FILE__, __LINE__, fmt, ##arg)
#else
#define eos_scanf(fmt, arg...)      scanf(fmt, ##arg)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_FSCANF_DEBUG)
#define eos_fscanf(fp, fmt, arg...) __eos_fscanf(__FILE__, __LINE__, fp, fmt, ##arg)
#else
#define eos_fscanf(fp, fmt, arg...) fscanf(fp, fmt, ##arg)
#endif

#if defined(EOS_STR_DEBUG) && defined(EOS_SSCANF_DEBUG)
#define eos_sscanf(s, fmt, arg...)  __eos_sscanf(__FILE__, __LINE__, s, fmt, ##arg)
#else
#define eos_sscanf(s, fmt, arg...)  sscanf(s, fmt, ##arg)
#endif

char* eos_strdup(const char* in_str);

#if defined(__cplusplus)
}
#endif


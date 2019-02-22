#ifndef __CPE_LOG_H__
#define __CPE_LOG_H__

#include <stdio.h>
#include <string.h>
#include <time.h>

#define filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x

#define CPE_LOG_DEBUG(fmt, ...) printf("%s |%s| %s:%d| DEBUG | "fmt"\r\n", get_cur_time(), filename(__FILE__), __func__, __LINE__, ##__VA_ARGS__)
#define CPE_LOG_INFO(fmt, ...) printf("%s |%s| %s:%d| INFO | "fmt"\r\n", get_cur_time(), filename(__FILE__), __func__, __LINE__, ##__VA_ARGS__)
#define CPE_LOG_WRAN(fmt, ...) printf("%s |%s| %s:%d| WRAN | "fmt"\r\n", get_cur_time(), filename(__FILE__), __func__, __LINE__, ##__VA_ARGS__)
#define CPE_LOG_ERROR(fmt, ...) printf("%s |%s| %s:%d| ERROR |"fmt"\r\n", get_cur_time(), filename(__FILE__), __func__, __LINE__, ##__VA_ARGS__)

char *get_cur_time();

/* __CPE_LOG_H__ */ 
#endif

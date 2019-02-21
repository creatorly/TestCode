
#ifndef __EOS_TIME_H
#define __EOS_TIME_H

#include <eos_config.h>

#include <time.h>
#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
#include <sys/times.h>
#else
#include <sys/time.h>
#endif

time_t eos_time(time_t *t);

int eos_gettimeofday(struct timeval* tv, struct timezone* tz);
int eos_settimeofday(const struct timeval* tv , const struct timezone* tz);
int eos_settimeofday_once(const struct timeval* tv , const struct timezone* tz);

struct tm* eos_localtime_r(const time_t* timep, struct tm* result);
int eos_mktime(struct tm* tm);

#endif


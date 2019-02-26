
#ifndef __UT_SPY_H__
#define __UT_SPY_H__

#if defined(__cplusplus)
extern "C" {
#endif

int ut_spy_init(int period, int time_slots);

int ut_spy_start(void);
int ut_spy_stop(void);

int ut_spy_get_cpu(int* usages, int time_slots);
int ut_spy_report(char* buf, int size);

#if defined(__cplusplus)
}
#endif

#endif


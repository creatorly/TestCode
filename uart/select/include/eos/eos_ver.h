
#ifndef __EOS_VER_H
#define __EOS_VER_H

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************************************************
 * 201 write a syslog when free a invalid memory
 * 202 alignment for 64-bit machine
 * 203 fixed the issue of watch dog when clear in main process
 * 204 add the feature of top
 * 205 add the feature of customized reboot command for watch dog
 * 206 add the feature of pid recording of the thread started by taskSpawn()
 * 207 add the macros eos_calloc_t() and eos_malloc_t()
 * 208 add the api of eos_set_max_fds()
 *
 * 210 add the support of tracing for the system APIs of mem, str and fmt inout
 * 211 fixed the issue of crash when creating eos_hash in __eos_mem_init()
 * 212 add the total memory size printing
 * 213 fixed the issue of not implementing some functions like eos_xxx
 * 214 use the optimization flag -Os
 * 215 add the feature of eos_stat for system stat
 * 216 compiled in android
 * 217 add the function of eos_mktime()
 * 218 add the function of eos_timer_reset()
 * 219 add some protection for eos_timer_xxx()
 * 220 fixed the issue of failure to convert a string ip with a space ended to ipv4
 * 221 fixed the issue of getting invalid ticks when clock is overflowed in 64-bit system 
 * 222 add the API of eos_srand()
 * 223 get tick via gettimeofday() in OS android
 * 224 add the API of eos_socket_is_valid()
 * 225 save the thread ID to pid file
 * 226 add the API of eos_fd_popen()
 * 227 add the API of eos_settimeofday_once()
 * 228 fixed the issue of loop invoking the callback of periodic timer when NTP time is got in ANDROID
 * 229 add the API of eos_timer_get_tmo()
 * 230 add the API of eos_wdg_set_idle_time()
 * 231 fixed the issue getting invalid ticks when NTP is got OK
 * 232 add the API of eos_timer_start_ex(), eos_timer_set_hook_begin/end()
 * 233 optimized the performance of EOS timer
 * 234 add the thread variables support for eos task
 * 235 fixed the issue of failure to reset the timer in it's callback function
 * 236 fixed the issue of failure to start a timer in the timer callback sometimes
 * 2018.10.23 237: add the running times for period timer
 * 2018.10.30 238: add a random prefix to the timer ID
 *                 add some trace infomation to eos timer
 * 2018.11.01 239: changed the timer's params from integer to pointer
 * 2018.11.02 240: add the API of eos_free_func()
 * 2019.02.14 241: add a returned value for eos_timer_stop/reset
 *
 **************************************************************************/

#define     EOS_VERSION         "241"

#if defined(__cplusplus)
}
#endif

#endif


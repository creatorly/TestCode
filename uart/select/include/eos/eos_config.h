#ifndef __EOS_CONFIG_H
#define __EOS_CONFIG_H


#if defined(__cplusplus)
extern "C" {
#endif

#define    EOS_ARCH_LITTLE_ENDIAN       1234
#define    EOS_ARCH_BIG_ENDIAN          4321

#ifndef    EOS_ARCH_ENDIAN
#define    EOS_ARCH_ENDIAN  EOS_ARCH_LITTLE_ENDIAN
#endif

#define    EJOIN_OS_TYPE_VXWORKS        0
#define    EJOIN_OS_TYPE_LINUX          1
#define    EJOIN_OS_TYPE_WIN32          2
#define    EJOIN_OS_TYPE_IOS            3

#if defined(__VXWORKS_OS__)
#define    EJOIN_OS_TYPE    EJOIN_OS_TYPE_VXWORKS
#elif defined(__WIN32_OS__)
#define    EJOIN_OS_TYPE    EJOIN_OS_TYPE_WIN32
#elif defined(__MAC_OS__)
#define    EJOIN_OS_TYPE    EJOIN_OS_TYPE_IOS
#else
#define    EJOIN_OS_TYPE    EJOIN_OS_TYPE_LINUX
#endif

#define    EOS_MAX_IODRVS               32
#define    EOS_MAX_TIMERS               1024

#define    EOS_MAX_THREADS              1024
#define    EOS_PTHREAD_STACK_MIN        (32*1024)

/* module configuration information    */
#if (EJOIN_OS_TYPE == EJOIN_OS_TYPE_VXWORKS)
/* young 20070305    
#define    EOS_CALL_ALLOW_MAX_NUM               10    */
#define    EOS_CALL_ALLOW_MAX_NUM               20

#define    EOS_MSG_ALLOW_MAX_NUM                5
#define    EOS_FDS_ALLOW_MAX_NUM_PER_MSG        48
#define    EOS_TIMER_ALLOW_MAX_NUM              8
/* V.008    */

#define    EOS_TIMER_ALLOW_MAX_NUM_PER_TIMER    48


#define    EOS_SEM_ALLOW_MAX_NUM                EOS_TIMER_ALLOW_MAX_NUM+64
#define    EOS_TASK_ALLOW_MAX_NUM               8
#define    EOS_IODRV_ALLOW_MAX_NUM              8


#define    EOS_TRACE_ALLOW_MAX_NUM              16
/* 20061130 
#define    EOS_TRACE_BUFFER_ALLOW_MAX_NUM       256
#define    EOS_TRACE_BUFFER_ALLOW_MAX_LEN       256    */
#define    EOS_TRACE_BUFFER_ALLOW_MAX_NUM       64
#define    EOS_TRACE_BUFFER_ALLOW_MAX_LEN       1024


#define    EOS_MEM_ALLOW_MAX_NUM                10
#define    TICKS_PER_SECOND                     200        

#define    EOS_CRLF                             "\r\n"

#elif (EJOIN_OS_TYPE == EJOIN_OS_TYPE_WIN32)

#define    EOS_CALL_ALLOW_MAX_NUM               10
#define    EOS_MSG_ALLOW_MAX_NUM                5
#define    EOS_FDS_ALLOW_MAX_NUM_PER_MSG        8
#define    EOS_TIMER_ALLOW_MAX_NUM              5
/* V.008    */
#define    EOS_TIMER_ALLOW_MAX_NUM_PER_TIMER    50


#define EOS_SEM_ALLOW_MAX_NUM                   EOS_TIMER_ALLOW_MAX_NUM+64
#define    EOS_TASK_ALLOW_MAX_NUM               8
#define    EOS_IODRV_ALLOW_MAX_NUM              8


#define    EOS_TRACE_ALLOW_MAX_NUM              16
#define    EOS_TRACE_BUFFER_ALLOW_MAX_NUM       256
#define    EOS_TRACE_BUFFER_ALLOW_MAX_LEN       256

#define    EOS_MEM_ALLOW_MAX_NUM                5

#define    EOS_CRLF                             "\r\n"

#elif (EJOIN_OS_TYPE == EJOIN_OS_TYPE_LINUX)

#define    EOS_CALL_ALLOW_MAX_NUM               512
#define    EOS_MSG_ALLOW_MAX_NUM                5
#define    EOS_FDS_ALLOW_MAX_NUM_PER_MSG        8

#define    EOS_TIMER_ALLOW_MAX_NUM              5
/* V.008    */
#define    EOS_TIMER_ALLOW_MAX_NUM_PER_TIMER    (EOS_CALL_ALLOW_MAX_NUM*2)


#define  EOS_SEM_ALLOW_MAX_NUM                  EOS_TIMER_ALLOW_MAX_NUM+64
#define    EOS_TASK_ALLOW_MAX_NUM               8
#define    EOS_IODRV_ALLOW_MAX_NUM              8


#define    EOS_TRACE_ALLOW_MAX_NUM              16
#define    EOS_TRACE_BUFFER_ALLOW_MAX_NUM       EOS_CALL_ALLOW_MAX_NUM*16
#define    EOS_TRACE_BUFFER_ALLOW_MAX_LEN       256

#define    EOS_MEM_ALLOW_MAX_NUM                5

#define    EOS_CRLF                             "\n"

#elif (EJOIN_OS_TYPE == EJOIN_OS_TYPE_IOS)

#define    EOS_CALL_ALLOW_MAX_NUM               512
#define    EOS_MSG_ALLOW_MAX_NUM                5
#define    EOS_FDS_ALLOW_MAX_NUM_PER_MSG        8

#define    EOS_TIMER_ALLOW_MAX_NUM              5
/* V.008    */
#define    EOS_TIMER_ALLOW_MAX_NUM_PER_TIMER    (EOS_CALL_ALLOW_MAX_NUM*2)


#define  EOS_SEM_ALLOW_MAX_NUM                  EOS_TIMER_ALLOW_MAX_NUM+64
#define    EOS_TASK_ALLOW_MAX_NUM               8
#define    EOS_IODRV_ALLOW_MAX_NUM              8


#define    EOS_TRACE_ALLOW_MAX_NUM              16
#define    EOS_TRACE_BUFFER_ALLOW_MAX_NUM       EOS_CALL_ALLOW_MAX_NUM*16
#define    EOS_TRACE_BUFFER_ALLOW_MAX_LEN       256

#define    EOS_MEM_ALLOW_MAX_NUM                5

#define    EOS_CRLF                             "\r"

#endif

#if defined(__cplusplus)
}
#endif 
#endif

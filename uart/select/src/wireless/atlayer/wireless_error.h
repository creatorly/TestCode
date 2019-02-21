/**
*
*
* File name: wireless_error.h
* Function: 
*
*/
#ifndef WIRELESS_ERROR_H
#define WIRELESS_ERROR_H

/** Define service completion status constants.  */

#define         WIRELESS_SUCCESS                      0
#define         WIRELESS_END_OF_LOG                   -1
#define         WIRELESS_GROUP_DELETED                -2
#define         WIRELESS_INVALID_DELETE               -3
#define         WIRELESS_INVALID_DRIVER               -4
#define         WIRELESS_INVALID_ENABLE               -5
#define         WIRELESS_INVALID_ENTRY                -6
#define         WIRELESS_INVALID_FUNCTION             -7
#define         WIRELESS_INVALID_GROUP                -8
#define         WIRELESS_INVALID_HISR                 -9
#define         WIRELESS_INVALID_MAILBOX              -10
#define         WIRELESS_INVALID_MEMORY               -11
#define         WIRELESS_INVALID_MESSAGE              -12
#define         WIRELESS_INVALID_OPERATION            -13
#define         WIRELESS_INVALID_PIPE                 -14
#define         WIRELESS_INVALID_POINTER              -15
#define         WIRELESS_INVALID_POOL                 -16
#define         WIRELESS_INVALID_PREEMPT              -17
#define         WIRELESS_INVALID_PRIORITY             -18
#define         WIRELESS_INVALID_QUEUE                -19
#define         WIRELESS_INVALID_RESUME               -20
#define         WIRELESS_INVALID_SEMAPHORE            -21
#define         WIRELESS_INVALID_SIZE                 -22
#define         WIRELESS_INVALID_START                -23
#define         WIRELESS_INVALID_SUSPEND              -24
#define         WIRELESS_INVALID_TASK                 -25
#define         WIRELESS_INVALID_TIMER                -26
#define         WIRELESS_INVALID_VECTOR               -27
#define         WIRELESS_MAILBOX_DELETED              -28
#define         WIRELESS_MAILBOX_EMPTY                -29
#define         WIRELESS_MAILBOX_FULL                 -30
#define         WIRELESS_MAILBOX_RESET                -31
#define         WIRELESS_NO_MEMORY                    -32
#define         WIRELESS_NO_MORE_LISRS                -33
#define         WIRELESS_NO_PARTITION                 -34
#define         WIRELESS_NOT_DISABLED                 -35
#define         WIRELESS_NOT_PRESENT                  -36
#define         WIRELESS_NOT_REGISTERED               -37
#define         WIRELESS_NOT_TERMINATED               -38
#define         WIRELESS_PIPE_DELETED                 -39
#define         WIRELESS_PIPE_EMPTY                   -40
#define         WIRELESS_PIPE_FULL                    -41
#define         WIRELESS_PIPE_RESET                   -42
#define         WIRELESS_POOL_DELETED                 -43
#define         WIRELESS_QUEUE_DELETED                -44
#define         WIRELESS_QUEUE_EMPTY                  -45
#define         WIRELESS_QUEUE_FULL                   -46
#define         WIRELESS_QUEUE_RESET                  -47
#define         WIRELESS_SEMAPHORE_DELETED            -48
#define         WIRELESS_SEMAPHORE_RESET              -49
#define         WIRELESS_TIMEOUT                      -50
#define         WIRELESS_UNAVAILABLE                  -51
#define         WIRELESS_INVALID_DESCRIPTION          -52
#define         WIRELESS_INVALID_REGION               -53
#define         WIRELESS_MEMORY_CORRUPT               -54
#define         WIRELESS_INVALID_DEBUG_ALLOCATION     -55
#define         WIRELESS_EMPTY_DEBUG_ALLOCATION_LIST  -56
#define         WIRELESS_EXIST_MESSAGE                -57



/** common*/
#define         WIRELESS_EFAILED                      1
#define         WIRELESS_ENOMEMORY                    2
#define         WIRELESS_EBADPARAM                    3
#define         WIRELESS_EUNSUPPORT                   4
#define         WIRELESS_EBADCLASS                    5
#define         WIRELESS_ENOTFOUND                    6

/** Define constants that are target dependent and/or are used for internal
   purposes.  */

#define         WIRELESS_MIN_STACK_SIZE               240
#define         WIRELESS_MAX_NAME                     8
#define         WIRELESS_MAX_VECTORS                  64
#define         WIRELESS_MAX_LISRS                    8

/** Define constants for the number of UNSIGNED words in each of the basic
   system data structures.  */

#define         WIRELESS_TASK_SIZE                    42
#define         WIRELESS_HISR_SIZE                    22
#define         WIRELESS_MAILBOX_SIZE                 13
#define         WIRELESS_QUEUE_SIZE                   18
#define         WIRELESS_PIPE_SIZE                    18
#define         WIRELESS_SEMAPHORE_SIZE               10
#define         WIRELESS_EVENT_GROUP_SIZE             9
#define         WIRELESS_PARTITION_POOL_SIZE          15
#define         WIRELESS_MEMORY_POOL_SIZE             17
#define         WIRELESS_TIMER_SIZE                   17
#define         WIRELESS_PROTECT_SIZE                 2
#define         WIRELESS_DRIVER_SIZE                  3

/** Define interrupt lockout and enable constants.  */

#define         WIRELESS_DISABLE_INTERRUPTS           0xC0
#define         WIRELESS_ENABLE_INTERRUPTS            0x00

/** Define system errors.  */
#define         WIRELESS_ERROR_CREATING_TIMER_HISR    1
#define         WIRELESS_ERROR_CREATING_TIMER_TASK    2
#define         WIRELESS_STACK_OVERFLOW               3
#define         WIRELESS_UNHANDLED_INTERRUPT          4


/** Define I/O driver constants.  */

#define         WIRELESS_IO_ERROR                     -1
#define         WIRELESS_INITIALIZE                   1
#define         WIRELESS_ASSIGN                       2
#define         WIRELESS_RELEASE                      3
#define         WIRELESS_INPUT                        4
#define         WIRELESS_OUTPUT                       5
#define         WIRELESS_STATUS                       6
#define         WIRELESS_TERMINATE                    7

/*=================================================================================*/
/*#################################################################################*/
/*=================================================================================*/
/*
#define WIRELESS_IS_SUCCESS(ret)    (ret == AK_SUCCESS)
#define WIRELESS_IS_FAILURE(ret)    (ret != AK_SUCCESS)
*/
#endif


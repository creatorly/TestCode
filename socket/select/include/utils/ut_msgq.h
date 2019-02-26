
#ifndef __UT_MSGQ_H__
#define __UT_MSGQ_H__

#include <utl_msgq.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef utl_msgq_t ST_UT_MSGQ;

/**
 * Initialize a message queue with specified size
 * 
 * @param maxMsgs   max size of the message queue
 * @param maxMsgLen max length of each message
 *
 * @return          the pointer to message queue
 */
ST_UT_MSGQ* UT_MsgqInit(_INT32 maxMsgs, _INT32 maxMsgLen);

/**
 * Free a given message queue
 * 
 * @param msgq      the message queue to be freed
 */
_VOID UT_MsgqFree(ST_UT_MSGQ* msgq);

/**
 * Get the message count in a message queue
 * 
 * @param msgq      the message queue to be worked on
 * 
 * @return          the message count
 */
_INT32 UT_MsgqSize(ST_UT_MSGQ* msgq);

/**
 * Receive a message from a given message queue in block mode
 * 
 * @param msgq      the message queue to be worked on
 * @param buf       the buffer used to store the message got
 * @param size      the size of given buffer
 * @param timeout   the blocking time in milliseconds, -1 means wait forever
 *
 * @return          the length of message got on success, ERROR(-1) otherwise
 */
_INT32 UT_MsgqRecv(ST_UT_MSGQ* msgq, _CHAR8* buf, _INT32 size, _INT32 timeout);

/**
 * Get a message from a given message queue
 * 
 * @param msgq      the message queue to be worked on
 * @param buf       the buffer used to store the message got
 * @param size      the size of given buffer
 *
 * @return          the length of message got on success, ERROR(-1) otherwise
 */
_INT32 UT_MsgqGrab(ST_UT_MSGQ* msgq, _VOID* buf, _INT32 size);

/**
 * Send a message to a given message queue
 * 
 * @param msgq      the message queue to be worked on
 * @param msg       the message to be sent
 * @param len       the message length
 *
 * @return          OK(0) on success or ERROR(-1) otherwise.
 */
_INT32 UT_MsgqSend(ST_UT_MSGQ* msgq, const _VOID* msg, _INT32 len);

/**
 * Statistic a given message queue
 * 
 * @param msgq      the message queue to be worked on
 * @param buf       the buffer used to restore statistics information
 * @param size      the buffer size
 *
 * @return          the length of statistics information.
 */
_INT32 UT_MsgqStat(ST_UT_MSGQ* msgq, _CHAR8* buf, _INT32 size);

#if defined(__cplusplus)
}
#endif

#endif



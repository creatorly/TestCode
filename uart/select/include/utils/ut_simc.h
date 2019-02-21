/********************************************************
* 
*
* XP workroom platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by XP workroom.
* 
* 
* File:         ut_smic.h
* PACKAGE:      libutl
*   Header of:          ut_smic.c
*   Date:               2013.04.14
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UT_SIMC_H__
#define __UT_SIMC_H__

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct tagstUtlSimCloundClient* PST_UT_SIMC;

PST_UT_SIMC UT_SimCCreate(const char* loc_ip, int loc_port, 
                          const char* rem_ip, int rem_port);
void UT_SimCDestroy(PST_UT_SIMC pSimC);

/**
 * Send a message to the remote ip and port
 * 
 * @param msgq      the message queue to be worked on
 * @param msg       the message to be sent
 * @param len       the message length
 *
 * @return          the length on success or ERROR(-1) otherwise.
 */
int UT_SimCSend(PST_UT_SIMC pSimC, const void* data, int len);

/**
 * Receive a message via the local ip and port
 * 
 * @param pSimC     the simc client to be worked on
 * @param buf       the buffer used to store the message got
 * @param size      the size of given buffer
 * @param timeout:  the timeout in milli-seconds, 0 means no wait, -1 means wait forever
 *
 * @return          the length of message got on success, negative number otherwise
 */
int UT_SimCRecv(PST_UT_SIMC pSimC, char* buf, int size, int timeout);

#if defined(__cplusplus)
}
#endif

#endif


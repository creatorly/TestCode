/********************************************************
* 
*
* XP workroom platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by XP workroom.
* 
* 
* File:         ut_simc.c
* PACKAGE:      libutl
*   Implements of:      utl sim clound client
*   Date:               2013.04.14
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifdef __VXWORKS_OS__
#include <selectLib.h>
#else
#include <sys/select.h>
#endif

#include <eos.h>

#include "ut_mem.h"
#include "ut_debug.h"
#include "ut_simc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagstUtlSimCloundClient
{
    eosAddress stLocAddr;
    eosAddress stRemAddr;

    eosSocket sock;
}ST_UT_SIMC;

PST_UT_SIMC UT_SimCCreate(const char* loc_ip, int loc_port, 
                          const char* rem_ip, int rem_port)
{
    ST_UT_SIMC* pSimC;
    _UINT32 uiIPv4 = 0;

    pSimC = UT_TALLOC(ST_UT_SIMC);
    if (NULL == pSimC)
    {
        UT_WARN(-1, EOS_ERROR, "failed to allocate for ST_UT_SIMC!", UT_NOARG);
        return NULL;
    }

    memset(pSimC, 0, sizeof(ST_UT_SIMC));

    eos_address_string_to_ipv4(&uiIPv4, loc_ip);
    eos_address_construct_ipv4(&pSimC->stLocAddr, uiIPv4, loc_port);

    eos_address_string_to_ipv4(&uiIPv4, rem_ip);
    eos_address_construct_ipv4(&pSimC->stRemAddr, uiIPv4, rem_port);

    eos_socket_construct(&pSimC->sock, EOS_ADDRESS_TYPE_IPV4, eosSocketProtocolUdp);
    /* eos_socket_set_blocking(&pSimC->sock, EOS_FALSE); */
    
    if (eos_socket_bind(&pSimC->sock, &pSimC->stLocAddr, NULL))
    {
        eos_socket_destruct(&pSimC->sock, EOS_FALSE, NULL);
        UT_FREE(pSimC);

        UT_WARN(-1, EOS_ERROR, "bind simc port failed!", UT_NOARG);
        return NULL;
    }

    return pSimC;
}

void UT_SimCDestroy(PST_UT_SIMC pSimC)
{
    if (pSimC != NULL)
    {
        eos_socket_destruct(&pSimC->sock, EOS_FALSE, NULL);
        UT_FREE(pSimC);
    }
}

int UT_SimCSend(PST_UT_SIMC pSimC, const void* data, int len)
{
    EOSSize_t nSent = 0;
    
    eos_socket_send_buffer(&pSimC->sock, 
        (_UCHAR8*)data, len, &pSimC->stRemAddr, &nSent);

    return nSent;
}

int UT_SimCRecv(PST_UT_SIMC pSimC, char* buf, int size, int ms)
{
    fd_set fds;
    int ret;

    FD_ZERO(&fds);
    FD_SET(pSimC->sock, &fds);

    if (ms >= 0)
    {
        struct timeval tv = {ms/1000, (ms%1000)*1000};

        ret = select(pSimC->sock+1, &fds, NULL, NULL, &tv);
    }
    else
    {
        ret = select(pSimC->sock+1, &fds, NULL, NULL, NULL);
    }

    if (ret > 0 && FD_ISSET(pSimC->sock, &fds))
    {
        eos_socket_receive_buffer(&pSimC->sock, buf, size, &ret, NULL);
    }

    return ret;
}

#ifdef __cplusplus
}
#endif



/********************************************************
* 
*
* XP-WorkRoom platform
* Copyright (C) 2013-2013, All rights reserved
* Designed by XP-WorkRoom Development Team.
* 
* 
* File:         ut_address.h
* PACKAGE:      libUtl
*   Header of:          ut_address.c
*   Date:               2013.03.17
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 1.0   to replace the implement of eos address
* 
*********************************************************/
 
#ifndef __UT_ADDRESS_H__
#define __UT_ADDRESS_H__

#include <eos_typedef.h>
#include <eos_address.h>

#if defined(__cplusplus)
extern "C" {
#endif

eosAddress * ut_address_construct(eosAddress *addr, _INT32 addrtype);
eosAddress * ut_address_copy(eosAddress *result, __CONST__ eosAddress *source);
EOSBool ut_address_compare(__CONST__ eosAddress *addr1, __CONST__ eosAddress *addr2, _UINT32 comparetype);
_CHAR8* ut_address_get_string(__CONST__ eosAddress *addr, _UCHAR8 *buf, EOSSize_t bufsize);
eosAddress * ut_address_set_string(eosAddress *addr, __CONST__ _CHAR8 *buf);
_USHORT16 ut_address_get_port(__CONST__ eosAddress *addr);
eosAddress * ut_address_set_port(eosAddress *addr, _USHORT16 port);
eosAddress * ut_address_construct_ipv4(eosAddress *addr,_UINT32 ip, _USHORT16 port);
_UCHAR8 *ut_address_ipv4_to_string(_CHAR8 *buf, EOSSize_t bufsize, _UINT32 ip);
EOSBool ut_address_string_to_ipv4(_UINT32 *ip, __CONST__ _CHAR8 *buf);
eosAddressIpv4 * ut_address_ipv4_set_ip(eosAddressIpv4 *addr, _UINT32 ip);

#ifdef __cplusplus
}
#endif

#endif  


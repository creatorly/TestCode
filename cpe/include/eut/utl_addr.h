/********************************************************
* 
*
* Ejoin Utils platform
* Copyright (C) 2008-2008, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         utl_addr.h
* PACKAGE:      libeut
*   Implement of:       utl_address.c
*   Date:               07/01/08
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef __UTL_ADDRESS_H__
#define __UTL_ADDRESS_H__

#include <stdio.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* IPV4 definitions */
#define UTL_ADDR_IPV4_STRINGSIZE 16 /* Max size of IPV4 address string */
#define UTL_ADDR_IPV4_ADDRSIZE 4 /* Size of IPV4 address */
#define UTL_ADDR_IPV4_ANYADDRESS (0)
#define UTL_ADDR_IPV4_BROADCAST (0xFFFFFFFF)
#define UTL_ADDR_IPV4_ANYPORT (0)

/* IPV6 definitions */
#define UTL_ADDR_IPV6_STRINGSIZE 46 /* Max size of IPV6 address string */
#define UTL_ADDR_IPV6_ADDRSIZE 16 /* Size of IPV6 address */
#define UTL_ADDR_IPV6_ANYPORT (0)

/* Combined Constants */
#define UTL_ADDR_MAXSTRINGSIZE UTL_ADDR_IPV6_STRINGSIZE /* Max size of any address string */
#define UTL_ADDR_MAXADDRSIZE UTL_ADDR_IPV6_ADDRSIZE /* Max address size of any address. */

typedef enum
{
    UTL_ADDRTYPE_IPV4 = 0,      /* AF_IPV4 */
    UTL_ADDRTYPE_IPV6,          /* AF_IPV6 */
    UTL_ADDRTYPE_UNIX,          /* AF_UNIX */
    
    UTL_ADDRTYPE_SIZE
}utl_addr_type_e;

/* 4 byte IP address, network format */
typedef unsigned int utl_addr_ipv4;
/* 16 byte IP address, network format */
typedef unsigned char utl_addr_ipv6[16];
/* p char pointer to the unix local path */
typedef char* utl_addr_unix;

typedef struct tagstUtlAddr
{
    utl_addr_type_e enType;

    unsigned short usPort;      /* network format */
    unsigned short usMaxPort;   /* network format */
    union
    {
        utl_addr_ipv4 ipv4;
        utl_addr_ipv6 ipv6;
        utl_addr_unix path;
    }ip;
}utl_addr_t;

int utl_addr_init(utl_addr_t *addr, 
                  utl_addr_type_e type,
                  const char* ip, int port);
int utl_addr_init_ipv4(utl_addr_t *addr, utl_addr_ipv4 ipv4, int port);
int utl_addr_free(utl_addr_t *addr);

bool utl_addr_equ(utl_addr_t *addr1, utl_addr_t *addr2);
bool utl_addr_ip_equ(utl_addr_t *addr1, utl_addr_t *addr2);

int utl_addr_cmp(utl_addr_t *addr1, utl_addr_t *addr2);
unsigned utl_addr_key(utl_addr_t* addr);

void utl_addr_copy(utl_addr_t* dst, utl_addr_t* src);

utl_addr_type_e utl_addr_get_type(utl_addr_t *addr);
void utl_addr_set_type(utl_addr_t* addr, int ver, int proto);

int utl_addr_get_ver(utl_addr_t *addr);
int utl_addr_get_proto(utl_addr_t *addr);
void utl_addr_set_proto(utl_addr_t* addr, int sock_proto);

char* utl_addr_get_ip(utl_addr_t *addr, char *ip);
int utl_addr_get_port(utl_addr_t *addr);
utl_addr_ipv4 utl_addr_get_ipv4(utl_addr_t *addr);
utl_addr_ipv6* utl_addr_get_ipv6(utl_addr_t *addr);

int utl_addr_set_ip(utl_addr_t *addr, const char* ip);
void utl_addr_set_port(utl_addr_t *addr, int port);
void utl_addr_set_ipv4(utl_addr_t *addr, utl_addr_ipv4 ipv4);
void utl_addr_set_ipv6(utl_addr_t *addr, utl_addr_ipv6 ipv6);

bool utl_addr_is_multicast(utl_addr_t *addr);
bool utl_addr_is_pub(utl_addr_t *addr);

#define utl_addr_is_ipv4(ip) utl_addr_str_to_ipv4(ip, NULL)
bool utl_addr_is_pub_ipv4(const char* ip);

bool utl_addr_ipv4_is_pub (utl_addr_ipv4 ipv4);
bool utl_addr_ipv4_net_same(utl_addr_ipv4 ipv4_1, utl_addr_ipv4 ipv4_2, utl_addr_ipv4 netmask);

#define utl_addr_ipv4_is_a(ipv4)    ((ntohl(ipv4) & 0x80000000) == 0)
#define utl_addr_ipv4_is_b(ipv4)    ((ntohl(ipv4) & 0xC0000000) == 0x80000000)
#define utl_addr_ipv4_is_c(ipv4)    ((ntohl(ipv4) & 0xE0000000) == 0xC0000000)
#define utl_addr_ipv4_is_d(ipv4)    ((ntohl(ipv4) & 0xF0000000) == 0xE0000000)
#define utl_addr_ipv4_is_e(ipv4)    ((ntohl(ipv4) & 0xF0000000) == 0xF0000000)

bool utl_addr_str_to_ipv4(const char* ip, utl_addr_ipv4* pIpv4);
char* utl_addr_ipv4_to_str(utl_addr_ipv4 ipv4, char* ip);

bool utl_addr_str_to_ipv6(const char *ip, utl_addr_ipv6 ipv6);
char* utl_addr_ipv6_to_str(utl_addr_ipv6 ipv6, char* ip);

int utl_addr_get_locip(const char* ifname, utl_addr_type_e type, char *address, size_t size);
int utl_addr_get_mac(const char* ifname, unsigned char aucMac[6], char acMac[18]);

#ifdef __cplusplus
}
#endif

#endif  


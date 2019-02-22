#ifndef __EOS_ADDRESS_H
#define __EOS_ADDRESS_H


#if defined(__cplusplus)
extern "C" {
#endif

#define EOS_ADDRESS_TYPE_IPV4 1
#define EOS_ADDRESS_TYPE_IPV6 2

/* IPV4 definitions */
#define EOS_ADDRESS_IPV4_STRINGSIZE 16 /* Max size of IPV4 address string */
#define EOS_ADDRESS_IPV4_ADDRSIZE 4 /* Size of IPV4 address */
#define EOS_ADDRESS_IPV4_ANYADDRESS (0)
#define EOS_ADDRESS_IPV4_BROADCAST (0xFFFFFFFF)
#define EOS_ADDRESS_IPV4_ANYPORT (0)
typedef struct {
    _UINT32 ip;    /* 4 byte IP address, network format */
    _USHORT16 port;  /* 2 byte port number, host format */
    _USHORT16 pads;
} eosAddressIpv4;

/* IPV6 definitions */
#define EOS_ADDRESS_IPV6_STRINGSIZE 46 /* Max size of IPV6 address string */
#define EOS_ADDRESS_IPV6_ADDRSIZE 16 /* Size of IPV6 address */
#define EOS_ADDRESS_IPV6_ANYPORT (0)
typedef struct {
    _UCHAR8 ip[16]; /* 16 byte IP address, network format */
    _UINT32 port;  /* 2 byte port number, host format */
} eosAddressIpv6;

typedef struct
{
    _UINT32   addrtype; /* Address type */
    union
    {
        /* One for each address type supported. */
        eosAddressIpv4 ipv4;
        eosAddressIpv6 ipv6;
    } data;
} eosAddress;

/* For comparisons */
#define EOS_ADDRESS_FULLADDRESS 0 /* Compare entire address. */
#define EOS_ADDRESS_BASEADDRESS 1 /* Compare only the base address */

/* Combined Constants */
#define EOS_ADDRESS_MAXSTRINGSIZE EOS_ADDRESS_IPV6_STRINGSIZE /* Max size of any address string */
#define EOS_ADDRESS_MAXADDRSIZE EOS_ADDRESS_IPV6_ADDRSIZE /* Max address size of any address. */

typedef struct
{
    _USHORT16    fromPort; /* Port the range starts from */
    _USHORT16    toPort; /* Port the range ends in */
    _UINT32      numberOfFreePorts; /* The number of free ports in the port range */

    _USHORT16    next; /* Next port to allocate */
} EOSPortRange;

eosAddress * eos_address_construct(eosAddress *addr, _INT32 addrtype);
_INT32 eos_address_get_type(__CONST__ eosAddress *addr);
eosAddress * eos_address_copy(eosAddress *result, __CONST__ eosAddress *source);
EOSBool eos_address_compare(__CONST__ eosAddress *addr1, __CONST__ eosAddress *addr2, _UINT32 comparetype);
_CHAR8* eos_address_get_string(__CONST__ eosAddress *addr, _CHAR8 *buf, EOSSize_t bufsize);
eosAddress * eos_address_set_string(eosAddress *addr, __CONST__ _CHAR8 *buf);
_USHORT16 eos_address_get_port(__CONST__ eosAddress *addr);
eosAddress * eos_address_set_port(eosAddress *addr, _USHORT16 port);
EOSBool eos_address_is_multicastip(__CONST__ eosAddress *addr);
eosAddress * eos_address_construct_ipv4(eosAddress *addr,_UINT32 ip, _USHORT16 port);
_CHAR8 *eos_address_ipv4_to_string(_CHAR8 *buf, EOSSize_t bufsize, _UINT32 ip);
EOSBool eos_address_string_to_ipv4(_UINT32 *ip, __CONST__ _CHAR8 *buf);
__CONST__ eosAddressIpv4 * eos_address_get_ipv4(__CONST__ eosAddress *addr);
eosAddressIpv4 *eos_address_ipv4_construct(eosAddressIpv4 *addr, _UINT32 ip, _USHORT16 port);
EOSBool eos_address_ipv4_compare(__CONST__ eosAddressIpv4 *addr1, __CONST__ eosAddressIpv4 *addr2, _INT32 comparetype);
_CHAR8 * eos_address_ipv4_get_string(__CONST__ eosAddressIpv4 *addr, _CHAR8 *buf, EOSSize_t bufsize);
eosAddressIpv4 *eos_address_ipv4_set_string(eosAddressIpv4 *addr, __CONST__ _CHAR8 *buf);
_UINT32 eos_address_ipv4_get_ip(__CONST__ eosAddressIpv4 *addr);
eosAddressIpv4 * eos_address_ipv4_set_ip(eosAddressIpv4 *addr, _UINT32 ip);
_USHORT16 eos_address_ipv4_get_port(__CONST__ eosAddressIpv4 *addr);
eosAddressIpv4 * eos_address_ipv4_set_port(eosAddressIpv4 *addr, _USHORT16 port);
EOSBool eos_address_ipv4_is_multicast(__CONST__ eosAddressIpv4 *addr);
eosAddress * eos_address_construct_ipv6(eosAddress *addr, __CONST__ _UCHAR8 *ip, _USHORT16 port);
_CHAR8 * eos_address_ipv6_to_string(_CHAR8 *buf, EOSSize_t bufsize, __CONST__ _UCHAR8 *ip);
EOSBool eos_address_string_to_ipv6(_UCHAR8 *ip, __CONST__ _CHAR8 *buf);
__CONST__ eosAddressIpv6 * eos_address_get_ipv6(__CONST__ eosAddress *addr);
eosAddressIpv6 *eos_address_ipv6_construct(eosAddressIpv6 *addr, __CONST__ _UCHAR8 *ip, _USHORT16 port);
EOSBool eos_address_ipv6_compare(__CONST__ eosAddressIpv6 *addr1, __CONST__ eosAddressIpv6 *addr2, _INT32 comparetype);
_CHAR8 * eos_address_ipv6_get_string(__CONST__ eosAddressIpv6 *addr, _CHAR8 *buf, EOSSize_t bufsize);
eosAddressIpv6 * eos_address_ipv6_set_string(eosAddressIpv6 *addr, __CONST__ _CHAR8 *buf);
__CONST__ _UCHAR8 * eos_address_ipv6_get_ip(__CONST__ eosAddressIpv6 *addr);
eosAddressIpv6 * eos_address_ipv6_set_ip(eosAddressIpv6 *addr, __CONST__ _UCHAR8 *ip);
_USHORT16 eos_address_ipv6_get_port(__CONST__ eosAddressIpv6 *addr);
eosAddressIpv6 *eos_address_ipv6_set_port(eosAddressIpv6 *addr, _USHORT16 port);
EOSBool eos_address_ipv6_is_multicast(__CONST__ eosAddressIpv6 *addr);

_INT32 eos_port_range_get_range(EOSPortRange*  portRange,_UINT32* fromPort,_UINT32* toPort);
_INT32 eos_port_range_get_number_of_free_ports(EOSPortRange* portRange, _UINT32* freePorts);
_INT32 eos_port_range_get_port(EOSPortRange* portRange, _UINT32*  port);
_INT32 eos_port_range_release_port(EOSPortRange* portRange,_UINT32 portToRelease);

#ifdef __cplusplus
}
#endif

#endif  

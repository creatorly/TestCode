
#ifndef UT_NETINET_H
#define UT_NETINET_H

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef __VXWORKS__
#define _ut_pack __attribute__((__packed__))
#else
#define    _ut_pack
#endif

/***************** definition of ETHER struction **************************/

/*
 * Ethernet address - 6 octets
 * this is only used by the ethers(3) functions.
 */
struct tagstUtEtherAddr {
    _UCHAR8 ether_addr_octet[6];
} _ut_pack;

/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct    tagstUtEtherHeader {
    _UCHAR8    ether_dhost[6];
    _UCHAR8    ether_shost[6];
    _USHORT16    ether_type;
    _UCHAR8 ether_ip[0];
} _ut_pack;

/* Ether header for tagged frames */
struct  tagstUtEtherTagHeader {
        _UCHAR8  ether_dhost[6];
        _UCHAR8  ether_shost[6];
        _UCHAR8  etherTag[4];
        _USHORT16 ether_type;
        _UCHAR8 ether_ip[0];
} _ut_pack;

typedef struct tagstUtEtherAddr ST_UT_ETHER_ADDR;
typedef struct tagstUtEtherHeader ST_UT_ETHER_HEADER;
typedef struct tagstUtEtherTagHeader ST_UT_ETHER_TAG_HEADER;

#define    UT_ETHERTYPE_PUP        0x0200    /* PUP protocol */
#define    UT_ETHERTYPE_IP        0x0800    /* IP protocol */
#define    UT_ETHERTYPE_ARP        0x0806    /* address resolution protocol */
#define    UT_ETHERTYPE_REVARP    0x8035    /* reverse addr resolution protocol */
#define    UT_ETHERTYPE_PPPOE_DISC        0x8863
#define    UT_ETHERTYPE_PPPOE_SESS        0x8864    

/***************** definition of IP struction **************************/
struct tagstUtPppoe
{
    _UCHAR8 aucPppoeHdr[6];
    _USHORT16 usProto;
} _ut_pack;
typedef struct tagstUtPppoe ST_UT_PPPOE;

#define UT_PPPOEPROTO_IP        0x0021

/***************** definition of IP struction **************************/

typedef struct tagstUtInAddr {
    _ULONG32 s_addr;
}ST_UT_INADDR;

struct tagstUtIp {
#if EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN
    _UINT32    ip_hl:4,        /* header length */
        ip_v:4,            /* version */
#elif EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN
    _UINT32    ip_v:4,            /* version */
        ip_hl:4,        /* header length */
#endif
        ip_tos:8,        /* type of service */
        ip_len:16;        /* total length */
    _USHORT16    ip_id;            /* identification */
    _SHORT16    ip_off;            /* fragment offset field */

    _UCHAR8    ip_ttl;            /* time to live */
    _UCHAR8    ip_p;            /* protocol */
    _USHORT16    ip_sum;            /* checksum */
    ST_UT_INADDR ip_src,ip_dst;    /* source and dest address */
    _UCHAR8 ip_udptcp[0];
} _ut_pack;
typedef struct tagstUtIp ST_UT_IP;
/*
 * Protocols
 */
#define    UT_IPPROTO_IP        0        /* dummy for IP */
#define    UT_IPPROTO_ICMP        1        /* control message protocol */
#define    UT_IPPROTO_IGMP        2        /* group mgmt protocol */
#define    UT_IPPROTO_GGP        3        /* gateway^2 (deprecated) */
#define    UT_IPPROTO_TCP        6        /* tcp */
#define    UT_IPPROTO_EGP        8        /* exterior gateway protocol */
#define    UT_IPPROTO_PUP        12        /* pup */
#define    UT_IPPROTO_UDP        17        /* user datagram protocol */
#define    UT_IPPROTO_IDP        22        /* xns idp */
#define    UT_IPPROTO_TP        29         /* tp-4 w/ class negotiation */
#define    UT_IPPROTO_EON        80        /* ISO cnlp */
#define    UT_IPPROTO_OSPF        89        /* OSPF version 2  */
#define    UT_IPPROTO_ENCAP        98        /* encapsulation header */

#define    UT_IPPROTO_RAW        255        /* raw IP packet */
#define    UT_IPPROTO_MAX        256

/***************** definition of TCP struction **************************/

typedef    u_long    _UT_TCPSEQ;

struct tagstTcpHdr {
    _USHORT16    th_sport;        /* source port */
    _USHORT16    th_dport;        /* destination port */
    _UT_TCPSEQ    th_seq;            /* sequence number */
    _UT_TCPSEQ    th_ack;            /* acknowledgement number */
#if EOS_ARCH_ENDIAN == EOS_ARCH_LITTLE_ENDIAN
    _UINT32    th_x2:4,        /* (unused) */
        th_off:4,        /* data offset */
#elif EOS_ARCH_ENDIAN == EOS_ARCH_BIG_ENDIAN
    _UINT32    th_off:4,        /* data offset */
        th_x2:4,        /* (unused) */
#endif
        th_flags:8,
        th_win:16;        /* window */
    _USHORT16    th_sum;            /* checksum */
    _USHORT16    th_urp;            /* urgent pointer */
    _UCHAR8     th_app[0];
} _ut_pack;
typedef struct tagstTcpHdr ST_UT_TCPHDR;

/***************** definition of UDP struction **************************/

struct tagstUdpHdr {
    _USHORT16    uh_sport;        /* source port */
    _USHORT16    uh_dport;        /* destination port */
    _SHORT16    uh_ulen;        /* udp length */
    _USHORT16    uh_sum;            /* udp checksum */
    _UCHAR8     uh_app[0];
} _ut_pack;
typedef struct tagstUdpHdr ST_UT_UDPHDR;

/***************** definition of ICMP struction **************************/

struct tagstIcmpHdr {
    _UCHAR8     ih_type;
    _UCHAR8        ih_code;
    _USHORT16    ih_chksum;        /* check sum of the icmp package */
    _USHORT16    ih_id;            /* identify of the icmp process */
    _USHORT16   ih_seq;         /* sequence of the icmp request */
} _ut_pack;
typedef struct tagstIcmpHdr ST_UT_ICMPHDR;

/**************************************************************************/

#define UT_NTOHL(x) eosConvertNetworkToHost32(x)
#define UT_HTONL(x) eosConvertHostToNetwork32(x)
#define UT_NTOHS(x) eosConvertNetworkToHost16(x)
#define UT_HTONS(x) eosConvertHostToNetwork16(x)

#define UtEtherGetShost(ether)  ((ether)->ether_shost)
#define UtEtherGetDhost(ether)  ((ether)->ether_dhost)
#define UtEtherGetProto(ether)  (UT_NTOHS((ether)->ether_type))
#define UtEtherGetIp(ether)     ((ether)->ether_ip)

#define UtPppGetProto(ppp)      (UT_NTOHS((ppp)->usProto))

#define UtIpGetProto(ip)        ((ip)->ip_p)
#define UtIpGetHdrLen(ip)       (((_UINT32)(ip)->ip_hl) << 2)
#define UtIpGetTotLen(ip)       (UT_NTOHS((ip)->ip_len))
#define UtIpGetSum(ip)            (UT_NTOHS((ip)->ip_sum))
#define UtIpGetSrc(ip)            (UT_NTOHL((ip)->ip_src))
#define UtIpGetDst(ip)            (UT_NTOHL((ip)->ip_dst))
#define UtIpGetUdpTcp(ip)       ((_UCHAR8*)(ip) + UtIpGetHdrLen(ip))

#define UtTcpHdrGetSport(tcp)   (UT_NTOHS((tcp)->th_sport))
#define UtTcpHdrGetDport(tcp)   (UT_NTOHS((tcp)->th_dport))
#define UtTcpHdrGetHdrLen(tcp)  (((_UINT32)(tcp)->th_off) << 2)
#define UtTcpHdrGetData(tcp)    ((_UCHAR8*)(tcp) + UtTcpHdrGetHdrLen(tcp))
#define UtIpTcpGetLen(ip, tcp)  \
    (UtIpGetTotLen(ip) - UtIpGetHdrLen(ip) - UtTcpHdrGetHdrLen(tcp))

#define UtUdpHdrGetSport(udp)   (UT_NTOHS((udp)->uh_sport))
#define UtUdpHdrGetDport(udp)   (UT_NTOHS((udp)->uh_dport))
#define UtUdpHdrGetData(udp)    ((udp)->uh_app)
#define UtUdpHdrGetLen(udp)     (UT_NTOHS((udp)->uh_ulen))

#define UT_ISIP(ether)  (UtEtherGetProto(ether) == UT_ETHERTYPE_IP)
#define UT_ISPPPDISC(ether)  (UtEtherGetProto(ether) == UT_ETHERTYPE_PPPOE_DISC)
#define UT_ISPPPSESS(ether)  (UtEtherGetProto(ether) == UT_ETHERTYPE_PPPOE_SESS)

#define UT_PPP_ISIP(ppp)    (UtPppGetProto(ppp) == UT_PPPOEPROTO_IP)

#define UT_IP_ISICMP(ip)    (UtIpGetProto(ip) == UT_IPPROTO_ICMP)

#define UT_IP_ISTCP(ip)     (UtIpGetProto(ip) == UT_IPPROTO_TCP)
#define UT_IP_ISUDP(ip)     (UtIpGetProto(ip) == UT_IPPROTO_UDP)
#define UT_IP_ISUDPTCP(ip)  (UT_IP_ISUDP(ip) || UT_IP_ISTCP(ip))

#if defined(__cplusplus)
}
#endif

#endif


/* include original header */
#include_next <linux/autoconf.h>

#undef CONFIG_PROC_FS

/*
 * Networking options
 */
#undef  CONFIG_PACKET
#undef  CONFIG_PACKET_MMAP
#undef  CONFIG_NETLINK_DEV
#undef  CONFIG_NETFILTER
#undef  CONFIG_FILTER
#undef  CONFIG_UNIX
#undef  CONFIG_INET
#define CONFIG_INET 1
#undef  CONFIG_IP_MULTICAST
#undef  CONFIG_IP_ADVANCED_ROUTER
#undef  CONFIG_IP_PNP
#undef  CONFIG_NET_IPIP
#undef  CONFIG_NET_IPGRE
#undef  CONFIG_IP_MROUTE
#undef  CONFIG_ARPD
#undef  CONFIG_INET_ECN
#undef  CONFIG_SYN_COOKIES
#undef  CONFIG_IPV6
#undef  CONFIG_KHTTPD
/*
 *    SCTP Configuration (EXPERIMENTAL)
 */
#undef  CONFIG_IPV6_SCTP__
#undef  CONFIG_IP_SCTP
#undef  CONFIG_ATM
#undef  CONFIG_VLAN_8021Q
#undef  CONFIG_IPX
#undef  CONFIG_ATALK

/*
 * Appletalk devices
 */
#undef  CONFIG_DEV_APPLETALK
#undef  CONFIG_DECNET
#undef  CONFIG_BRIDGE
#undef  CONFIG_X25
#undef  CONFIG_LAPB
#undef  CONFIG_LLC
#undef  CONFIG_NET_DIVERT
#undef  CONFIG_ECONET
#undef  CONFIG_WAN_ROUTER
#undef  CONFIG_NET_FASTROUTE
#undef  CONFIG_NET_HW_FLOWCONTROL

/*
 * QoS and/or fair queueing
 */
#undef  CONFIG_NET_SCHED

/*
 * Network testing
 */
#undef  CONFIG_NET_PKTGEN

/*
 * IPSEC (FREESWAN)
 */
#undef  CONFIG_IPSEC
#define CONFIG_IPSEC 1
#undef  CONFIG_IPSEC_IPIP
#define CONFIG_IPSEC_IPIP 1
#undef  CONFIG_IPSEC_AH
#define CONFIG_IPSEC_AH 1
#undef  CONFIG_IPSEC_AUTH_HMAC_MD5
#define CONFIG_IPSEC_AUTH_HMAC_MD5 1
#undef  CONFIG_IPSEC_AUTH_HMAC_SHA1
#define CONFIG_IPSEC_AUTH_HMAC_SHA1 1
#undef  CONFIG_IPSEC_ESP
#define CONFIG_IPSEC_ESP 1
#undef  CONFIG_IPSEC_ENC_3DES
#define CONFIG_IPSEC_ENC_3DES 1
#undef  CONFIG_IPSEC_IPCOMP
#define CONFIG_IPSEC_IPCOMP 1
#undef  CONFIG_IPSEC_DEBUG
#define CONFIG_IPSEC_DEBUG 1
#undef  CONFIG_IPSEC_DYNDEV
#define CONFIG_IPSEC_DYNDEV 1
#undef  CONFIG_IPSEC_REGRESS
#define CONFIG_IPSEC_REGRESS 0
/*
 * Network device support
 */
#undef  CONFIG_NETDEVICES
#define CONFIG_NETDEVICES 1

/*
 * ARCnet devices
 */
#undef  CONFIG_ARCNET
#undef  CONFIG_DUMMY
#define CONFIG_DUMMY 1
#undef  CONFIG_DUMMY_MODULE
#undef  CONFIG_BONDING
#undef  CONFIG_EQUALIZER
#undef  CONFIG_TUN
#define CONFIG_L4TUN 1
#undef  CONFIG_NET_SB1000
/*
 * Ethernet (10 or 100Mbit)
 */
#undef  CONFIG_NET_ETHERNET
#define CONFIG_NET_ETHERNET 1
/*
 * PCMCIA network device support
 */
#undef CONFIG_NET_PCMCIA
#undef CONFIG_PCMCIA_PCNET
#undef CONFIG_NET_PCMCIA_RADIO
#undef CONFIG_PCMCIA_RAYCS
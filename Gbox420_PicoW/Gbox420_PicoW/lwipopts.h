#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

// (see https://www.nongnu.org/lwip/2_1_x/group__lwip__opts.html)

// allow override in some examples
#ifndef NO_SYS
#define NO_SYS 1 // NO_SYS==1: Use lwIP without OS-awareness (no thread, semaphores, mutexes or mboxes). This means threaded APIs cannot be used (socket, netconn, i.e. everything in the 'api' folder), only the callback-style raw API is available (and you have to watch out for yourself that you don't access lwIP functions/structures from more than one context at a time!)
#endif
// allow override in some examples
#ifndef LWIP_SOCKET
#define LWIP_SOCKET 0 // LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
#endif
#if PICO_CYW43_ARCH_POLL
#define MEM_LIBC_MALLOC 1 // MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library instead of the lwip internal allocator. Can save code size if you already use it.
#else
// MEM_LIBC_MALLOC is incompatible with non polling versions
#define MEM_LIBC_MALLOC 0 // MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library instead of the lwip internal allocator. Can save code size if you already use it.
#endif
#define MEM_ALIGNMENT 4                                                    // MEM_ALIGNMENT: should be set to the alignment of the CPU 4 byte alignment -> #define MEM_ALIGNMENT 4 2 byte alignment -> #define MEM_ALIGNMENT 2
#define MEM_SIZE 4000                                                      // MEM_SIZE: the size of the heap memory. If the application will send a lot of data that needs to be copied, this should be set high.
#define MEMP_NUM_TCP_SEG 32                                                // MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments. (requires the LWIP_TCP option
#define MEMP_NUM_ARP_QUEUE 10                                              // MEMP_NUM_ARP_QUEUE: the number of simultaneously queued outgoing packets (pbufs) that are waiting for an ARP request (to resolve their destination address) to finish. (requires the ARP_QUEUEING option)
#define PBUF_POOL_SIZE 24                                                  // PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
#define LWIP_ARP 1                                                         // LWIP_ARP==1: Enable ARP functionality.
#define LWIP_ETHERNET 1                                                    // LWIP_ETHERNET==1: enable ethernet support even though ARP might be disabled
#define LWIP_ICMP 1                                                        // LWIP_ICMP==1: Enable ICMP module inside the IP stack. Be careful, disable that make your product non-compliant to RFC1122
#define LWIP_RAW 1                                                         // LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
#define TCP_WND (8 * TCP_MSS)                                              // TCP_WND: The size of a TCP window. This must be at least (2 * TCP_MSS) for things to work well. ATTENTION: when using TCP_RCV_SCALE, TCP_WND is the total size with scaling applied. Maximum window value in the TCP header will be TCP_WND >> TCP_RCV_SCALE
#define TCP_MSS 1460                                                       // TCP_MSS: TCP Maximum segment size. (default is 536, a conservative default, you might want to increase this.) For the receive side, this MSS is advertised to the remote side when opening a connection. For the transmit size, this MSS sets an upper limit on the MSS advertised by the remote host.
#define TCP_SND_BUF (8 * TCP_MSS)                                          // TCP_SND_BUF: TCP sender buffer space (bytes). To achieve good performance, this should be at least 2 * TCP_MSS.
#define TCP_SND_QUEUELEN ((4 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS)) // TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work.
#define LWIP_NETIF_STATUS_CALLBACK 1                                       // LWIP_NETIF_STATUS_CALLBACK==1: Support a callback function whenever an interface changes its up/down status (i.e., due to DHCP IP acquisition)
#define LWIP_NETIF_LINK_CALLBACK 1                                         // LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface whenever the link changes (i.e., link down)
#define LWIP_NETIF_HOSTNAME 1                                              // LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname field.
#define LWIP_NETCONN 0                                                     // LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
#define MEM_STATS 0                                                        // MEM_STATS==1: Enable mem.c stats.
#define SYS_STATS 0                                                        // SYS_STATS==1: Enable system stats (sem and mbox counts, etc).
#define MEMP_STATS 0                                                       // MEMP_STATS==1: Enable memp.c pool stats.
#define LINK_STATS 0                                                       // LINK_STATS==1: Enable link stats.
// #define ETH_PAD_SIZE                2  // ETH_PAD_SIZE: number of bytes added before the ethernet header to ensure alignment of payload after that header. Since the header is 14 bytes long, without this padding e.g. addresses in the IP header will not be aligned on a 32-bit boundary, so setting this to 2 can speed up 32-bit-platforms.
#define LWIP_CHKSUM_ALGORITHM 3
#define LWIP_DHCP 1                 // LWIP_DHCP==1: Enable DHCP module.
#define LWIP_IPV4 1                 // LWIP_IPV4==1: Enable IPv4
#define LWIP_TCP 1                  // LWIP_TCP==1: Turn on TCP.
#define LWIP_UDP 1                  // LWIP_UDP==1: Turn on UDP.
#define LWIP_DNS 1                  // LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS transport.
#define LWIP_TCP_KEEPALIVE 1        // LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set in seconds. (does not require sockets.c, and will affect tcp.c)
#define LWIP_NETIF_TX_SINGLE_PBUF 1 // LWIP_NETIF_TX_SINGLE_PBUF: if this is set to 1, lwIP tries to put all data to be sent into one single pbuf. This is for compatibility with DMA-enabled MACs that do not support scatter-gather. Beware that this might involve CPU-memcpy before transmitting that would not be needed without this flag! Use this only if you need to
#define DHCP_DOES_ARP_CHECK 0       // DHCP_DOES_ARP_CHECK==1: Do an ARP check on the offered address.
#define LWIP_DHCP_DOES_ACD_CHECK 0

#ifndef NDEBUG
#define LWIP_DEBUG 1         // Enable debug message printing, but only if debug message type is enabled AND is of correct type AND is at least LWIP_DBG_LEVEL.
#define LWIP_STATS 1         // WIP_STATS==1: Enable statistics collection in lwip_stats.
#define LWIP_STATS_DISPLAY 1 // WIP_STATS_DISPLAY==1: Compile in the statistics output functions.
#endif

#define ETHARP_DEBUG LWIP_DBG_OFF
#define NETIF_DEBUG LWIP_DBG_OFF
#define PBUF_DEBUG LWIP_DBG_OFF
#define API_LIB_DEBUG LWIP_DBG_OFF
#define API_MSG_DEBUG LWIP_DBG_OFF
#define SOCKETS_DEBUG LWIP_DBG_OFF
#define ICMP_DEBUG LWIP_DBG_OFF
#define INET_DEBUG LWIP_DBG_OFF
#define IP_DEBUG LWIP_DBG_OFF
#define IP_REASS_DEBUG LWIP_DBG_OFF
#define RAW_DEBUG LWIP_DBG_OFF
#define MEM_DEBUG LWIP_DBG_OFF
#define MEMP_DEBUG LWIP_DBG_OFF
#define SYS_DEBUG LWIP_DBG_OFF
#define TCP_DEBUG LWIP_DBG_OFF
#define TCP_INPUT_DEBUG LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG LWIP_DBG_OFF
#define TCP_RTO_DEBUG LWIP_DBG_OFF
#define TCP_CWND_DEBUG LWIP_DBG_OFF
#define TCP_WND_DEBUG LWIP_DBG_OFF
#define TCP_FR_DEBUG LWIP_DBG_OFF
#define TCP_QLEN_DEBUG LWIP_DBG_OFF
#define TCP_RST_DEBUG LWIP_DBG_OFF
#define UDP_DEBUG LWIP_DBG_OFF
#define TCPIP_DEBUG LWIP_DBG_OFF
#define PPP_DEBUG LWIP_DBG_OFF
#define SLIP_DEBUG LWIP_DBG_OFF
#define DHCP_DEBUG LWIP_DBG_OFF

#define MEMP_NUM_SYS_TIMEOUT (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1) /* Needed for MQTT - Plus one per each MQTT server */
#define MQTT_REQ_MAX_IN_FLIGHT (5)                               /* Needed for MQTT - Maximum subscribe requests */

#endif /* __LWIPOPTS_H__ */
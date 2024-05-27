// Common settings used in most of the pico_w examples
// (see https://www.nongnu.org/lwip/2_1_x/group__lwip__opts.html for details)
#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

#define NO_SYS 0 ///< Use the OS (Operating System) mode. Enables threads, semaphores, mutexes or mboxes
#define TCPIP_THREAD_STACKSIZE 4096  ///< Stack size for the TCP/IP thread.
#define DEFAULT_THREAD_STACKSIZE 4096  ///< Default stack size for other threads.
#define DEFAULT_RAW_RECVMBOX_SIZE 8 ///< Default size of the mailbox for raw API receive.
#define TCPIP_MBOX_SIZE 8 ///< Size of the mailbox for TCP/IP messages.
#define LWIP_TIMEVAL_PRIVATE 0 ///< Use the system's timeval structure.
#define LWIP_TCPIP_CORE_LOCKING_INPUT 1 ///< Enable core locking for input (optional).
#define LWIP_SO_RCVTIMEO 1 ///< Enable socket receive timeout feature.

#define LWIP_SOCKET 0 ///< Disable the socket API.
#if PICO_CYW43_ARCH_POLL
#define MEM_LIBC_MALLOC 1 ///< Use libc malloc for memory allocation.
#else
#define PICO_USE_STACK_GUARDS 1
#define MEM_LIBC_MALLOC 0 ///< Use lwIP's memory allocation if not polling.
#endif
#define MEM_ALIGNMENT 4 ///< Memory alignment for efficient access.
#define MEM_SIZE 65536   ///< 64KB - Size of the heap memory in Bytes. Typically, after accounting for the firmware, application code, and other necessities, you might have around 200,000 Bytes or less available for dynamic memory allocation from the 264,000 Bytes of SRAM on the PicoW: TODO: Reduce mem allocation and see where lwip crashes
#define MEMP_NUM_TCP_SEG 32 ///< Number of simultaneously queued TCP segments.
#define MEMP_NUM_ARP_QUEUE 10 ///< Number of queued outgoing packets for ARP.
#define PBUF_POOL_BUFSIZE 2048 ///< 2KB - Packet buffer size in bytes. Stores network packet data
#define PBUF_POOL_SIZE 16 ///< Number of buffers in the pbuf pool. //TODO: Decrease until network packet drops 
#define LWIP_ARP 1 ///< Enable ARP (Address Resolution Protocol).
#define LWIP_ETHERNET 1 ///< Enable Ethernet support.
#define LWIP_ICMP 1 ///< Enable ICMP (Internet Control Message Protocol).
#define LWIP_RAW 1 ///< Enable raw API support.
#define TCP_WND (8 * TCP_MSS) ///< TCP window size.
#define TCP_MSS 1460 ///< TCP Maximum Segment Size.
#define TCP_SND_BUF (8 * TCP_MSS) ///< TCP sender buffer size.
#define TCP_SND_QUEUELEN ((8 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS)) ///< TCP sender queue length.
#define LWIP_NETIF_STATUS_CALLBACK 1 ///< Enable netif status change callback.
#define LWIP_NETIF_LINK_CALLBACK 1 ///< Enable netif link change callback.
#define LWIP_NETIF_HOSTNAME 1 ///< Enable netif hostname support.
#define LWIP_NETCONN 0 ///< Disable Netconn API.
#define MEM_STATS 1 ///< Enable memory statistics.
#define SYS_STATS 1 ///< Enable system statistics.
#define MEMP_STATS 1 ///< Enable memory pool statistics.
#define LINK_STATS 1 ///< Enable link statistics.
// #define ETH_PAD_SIZE                2 ///< Optional padding for Ethernet frames.
#define LWIP_CHKSUM_ALGORITHM 3 ///< Checksum algorithm to use.
#define LWIP_DHCP 1 ///< Enable DHCP (Dynamic Host Configuration Protocol).
#define LWIP_IPV4 1 ///< Enable IPv4 support.
#define LWIP_TCP 1 ///< Enable TCP (Transmission Control Protocol).
#define LWIP_UDP 1 ///< Enable UDP (User Datagram Protocol).
#define LWIP_DNS 1 ///< Enable DNS (Domain Name System).
/*
#define LWIP_HTTPD_SSI   ///< HTTP Server - Enable Server Side Include support
#define LWIP_HTTPD_CGI    ///< HTTP Server - Enable Common Gateway Interface support
#define LWIP_HTTPD_DYNAMIC_HEADERS  ///< HTTP Server - Insert the headers automatically based on the extension of the file being served
*/
#define LWIP_TCP_KEEPALIVE 1 ///< Enable TCP keep-alive functionality.
#define LWIP_NETIF_TX_SINGLE_PBUF 1 ///< Enable single pbuf transmission. Enabling single pbuf transmission means that lwIP will attempt to send the entire packet as a single pbuf, rather than splitting it into multiple pbufs
#define DHCP_DOES_ARP_CHECK 1 ///< Enable ARP check for DHCP.
#define LWIP_DHCP_DOES_ACD_CHECK 1 ///< Enable ACD check for DHCP.

#define LWIP_DEBUG 1 ///< Enable debugging.
#define LWIP_STATS 1 ///< Enable statistics.
#define LWIP_STATS_DISPLAY 1 ///< Enable statistics display.


#define ETHARP_DEBUG LWIP_DBG_OFF ///< Disable ARP debugging.
#define NETIF_DEBUG LWIP_DBG_OFF ///< Disable netif debugging.
#define PBUF_DEBUG LWIP_DBG_OFF ///< Disable pbuf debugging.
#define API_LIB_DEBUG LWIP_DBG_OFF ///< Disable API library debugging.
#define API_MSG_DEBUG LWIP_DBG_OFF ///< Disable API message debugging.
#define SOCKETS_DEBUG LWIP_DBG_OFF ///< Disable sockets debugging.
#define ICMP_DEBUG LWIP_DBG_OFF ///< Disable ICMP debugging.
#define INET_DEBUG LWIP_DBG_OFF ///< Disable INET debugging.
#define IP_DEBUG LWIP_DBG_OFF ///< Disable IP debugging.
#define IP_REASS_DEBUG LWIP_DBG_OFF ///< Disable IP reassembly debugging.
#define RAW_DEBUG LWIP_DBG_OFF ///< Disable raw API debugging.
#define MEM_DEBUG LWIP_DBG_OFF ///< Disable memory debugging.
#define MEMP_DEBUG LWIP_DBG_OFF ///< Disable memory pool debugging.
#define SYS_DEBUG LWIP_DBG_OFF ///< Disable system debugging.
#define TCP_DEBUG LWIP_DBG_OFF ///< Disable TCP debugging.
#define TCP_INPUT_DEBUG LWIP_DBG_OFF ///< Disable TCP input debugging.
#define TCP_OUTPUT_DEBUG LWIP_DBG_OFF ///< Disable TCP output debugging.
#define TCP_RTO_DEBUG LWIP_DBG_OFF ///< Disable TCP RTO debugging.
#define TCP_CWND_DEBUG LWIP_DBG_OFF ///< Disable TCP CWND debugging.
#define TCP_WND_DEBUG LWIP_DBG_OFF ///< Disable TCP window debugging.
#define TCP_FR_DEBUG LWIP_DBG_OFF ///< Disable TCP fast retransmit debugging.
#define TCP_QLEN_DEBUG LWIP_DBG_OFF ///< Disable TCP queue length debugging.
#define TCP_RST_DEBUG LWIP_DBG_OFF ///< Disable TCP reset debugging.
#define UDP_DEBUG LWIP_DBG_OFF ///< Disable UDP debugging.
#define TCPIP_DEBUG LWIP_DBG_OFF ///< Disable TCP/IP debugging.
#define PPP_DEBUG LWIP_DBG_OFF ///< Disable PPP debugging.
#define SLIP_DEBUG LWIP_DBG_OFF ///< Disable SLIP debugging.
#define DHCP_DEBUG LWIP_DBG_OFF ///< Disable DHCP debugging.

#define MEMP_NUM_SYS_TIMEOUT (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1) ///< Number of system timeouts, plus one for MQTT.
//Redefining MQTT Options from https://www.nongnu.org/lwip/2_1_x/group__mqtt__opts.html
#undef MQTT_OUTPUT_RINGBUF_SIZE
#define MQTT_OUTPUT_RINGBUF_SIZE 4096   ///< Output ring-buffer size for MQTT. must fit outgoing publish message topic (MaxShotTextLength) + payloads (MaxLongTextLength) */
#undef MQTT_VAR_HEADER_BUFFER_LEN
#define MQTT_VAR_HEADER_BUFFER_LEN 1160 ///< Receive buffer size for MQTT.
#undef MQTT_REQ_MAX_IN_FLIGHT
#define MQTT_REQ_MAX_IN_FLIGHT 4  ///< Max pending MQTT requests.
#undef MQTT_CYCLIC_TIMER_INTERVAL
#define MQTT_CYCLIC_TIMER_INTERVAL 1 ///< Interval for MQTT cyclic timer calls.
#undef MQTT_REQ_TIMEOUT
#define MQTT_REQ_TIMEOUT 10 ///< Timeout for MQTT requests in seconds.
#undef MQTT_CONNECT_TIMOUT
#define MQTT_CONNECT_TIMOUT 10 ///< Timeout for MQTT connect response in seconds.

#endif /* __LWIPOPTS_H__ */

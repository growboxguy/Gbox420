/*! \file
 *  \brief     Update built-in Real Time Clock with an NTP query
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#pragma once

#include <string.h>
#include <time.h>
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"

typedef struct
{
    ip_addr_t ntp_server_address;
    bool dns_request_sent;
    struct udp_pcb *ntp_pcb;
    absolute_time_t ntp_test_time;
    alarm_id_t ntp_resend_alarm;
} NTP_T;

static void initializeRTC();  //Initialize the Real Time Clock and do an NTP update 
static void ntp_result(NTP_T *state, int status, time_t *result);  // Called with the NTP results, updates the Real Time Clock
static void ntp_request(NTP_T *state);  // Make an NTP request
static int64_t ntp_failed_handler(alarm_id_t id, void *user_data); //In case NTP query fails
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg); //When the IP address of NTP server is found initiates an ntp_request
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port); // NTP data received
static void ntp_update(); // Initiate an NTP Server query for getting the current time

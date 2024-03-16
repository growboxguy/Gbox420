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
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "../Settings.h"
#include "420Common.h"

#define NTP_MSG_LEN 48
#define NTP_DELTA 2208988800 // seconds between 1 Jan 1900 and 1 Jan 1970
#define NTP_RESEND_TIME (10 * 1000)

class NtpClient
{
public:
    NtpClient(Settings::NtpClientSettings *DefaultSettings);                                                       ///< Initialize the Real Time Clock and do an NTP update
    void ntpResult(int status, time_t *result);                                                                    ///< Called with the NTP results, updates the Real Time Clock
    void ntpRequest();                                                                                             ///< Make an NTP request
    static void ntpReceived(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port); ///< NTP data received
    void ntpUpdate();                                                                                              ///< Initiate an NTP Server query for getting the current time

private:
protected:
    char *NtpServerDNS; ///< NTP server DNS name, "" to use NtpServerIP instead
    char *NtpServerIP;  ///< NTP server IP. Used when NtpServerDNS is empty, or the DNS lookup fails
    ip_addr_t NtpServerAddress;
    bool NtpRefreshInprogress = false;
    struct udp_pcb *NtpPcb;
    uint16_t *NtpServerPort;    ///< NTP server Port
    int8_t *TimeZoneDifference; ///<  UTC time and current timezone difference
    uint32_t *TimeoutSeconds;   ///< Duration in seconds that an NTP client waits for a response from an NTP server
};
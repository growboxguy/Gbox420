/*! \file
 *  \brief     Resolve DNS name to IP
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/raspberry-pi-pico-w
 *  \version   4.20
 */

#pragma once

#include "pico/cyw43_arch.h"
#include "lwip/err.h"
#include "lwip/ip_addr.h"
#include "lwip/dns.h"

bool DnsLookup(char *DnsName, ip_addr_t *ResultIP); ///< Start a DNS lookup for DnsName, update ResultIP with the result. Returns true if DNS lookup was successful
void DnsLookupResult(const char *Hostname, const ip_addr_t *ResultIP, void *Arg);   ///< Callback with the lookup result
static bool dnsLookupInProgress = false; ///< true: DNS lookup in progress
static bool dnsLookupSuccess = false; ///< true: DNS lookup resulted in an IP


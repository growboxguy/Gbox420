#include "DnsClient.h"

bool DnsLookup(char *DnsName, ip_addr_t *ResultIP)
{
    dnsLookupSuccess = false;
    absolute_time_t LastRefresh = get_absolute_time(); // Used to track timeouts
    printf("Looking up IP for %s...", DnsName);
    err_t err = dns_gethostbyname(DnsName, ResultIP, DnsLookupResult, ResultIP);

    if (err == ERR_OK) // DNS name found in cache
    {
        printf("Found cached address\n");
        dnsLookupInProgress = false;
        dnsLookupSuccess = true;
    }
    else
    {
        dnsLookupInProgress = true;
    }

    while (dnsLookupInProgress) // Waiting for the DNS lookup to finish and DnsLookupResult callback to trigger
    {
        sleep_ms(100);
        if (absolute_time_diff_us(LastRefresh, get_absolute_time()) > 10000000) // 10sec timeout
        {
            printf("DNS lookup timeout\n");
            return false;
        }
    }
    return dnsLookupSuccess;
}

void DnsLookupResult(const char *Hostname, const ip_addr_t *FoundIP, void *ResultIP) // DNS lookup callback
{
    if (FoundIP)
    {
        printf("Found address: %s\n", ipaddr_ntoa(FoundIP));
        ip_addr_copy(*(ip_addr_t *)ResultIP, *FoundIP);
        dnsLookupSuccess = true;
    }
    else
    {
        printf("DNS lookup failed\n");
    }
    dnsLookupInProgress = false;
}
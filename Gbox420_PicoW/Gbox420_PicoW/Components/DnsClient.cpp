#include "DnsClient.h"

bool DnsLookup(char *DnsName, ip_addr_t *ResultIP)
{
    dnsLookupSuccess = false;
    absolute_time_t Timeout = make_timeout_time_ms(10000); // 10sec from now
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
        if (get_absolute_time() > Timeout) // 10sec timeout
        {
            printf("DNS lookup timeout\n");
            return false;
        }
        sleep_ms(500);
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
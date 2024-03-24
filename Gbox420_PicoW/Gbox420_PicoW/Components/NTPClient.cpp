#include "NtpClient.h"

// Initialize the built-in RTC
NtpClient::NtpClient(Settings::NtpClientSettings *DefaultSettings) : Common(DefaultSettings->Name)
{
    NtpServerIP = DefaultSettings->NtpServerIP;
    NtpServerDNS = DefaultSettings->NtpServerDNS;
    NtpServerPort = &DefaultSettings->NtpServerPort;
    TimeZoneDifference = &DefaultSettings->TimeZoneDifference;
    TimeoutSeconds = &DefaultSettings->TimeoutSeconds;
    NtpPcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    udp_recv(NtpPcb, ntpReceived, this);
    rtc_init(); // Initialize "hardware/rtc.h"
    rtcUpdateTrigger(true);
    printf("   NtpClient ready - ");
    getCurrentTime(true);
}

void NtpClient::rtcUpdateTrigger(bool WaitForResult)
{
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP) // Returns the status of the WiFi link: CYW43_LINK_DOWN(0)-link is down,CYW43_LINK_JOIN(1)-Connected to WiFi,CYW43_LINK_NOIP(2)-Connected to WiFi, but no IP address,CYW43_LINK_UP  (3)-Connect to WiFi with an IP address,CYW43_LINK_FAIL(-1)-Connection failed,CYW43_LINK_NONET(-2)-No matching SSID found (could be out of range, or down),CYW43_LINK_BADAUTH(-3)-Authentication failure
    {
        ip4addr_aton(NtpServerIP, &NtpServerAddress); // If NtpServerDNS is defined and the DNS lookup is successful this will be overwritten
        if (NtpServerDNS[0] != '\0')                  // If an NTP server DNS name is specified -> Look up the IP
        {
            if (WaitForResult)
            {
                DnsLookup(NtpServerDNS, &NtpServerAddress);
                rtcUpdate(WaitForResult);
            }
            else
            {
                std::function<void(ip_addr_t *)> callbackFunctionPtr = std::bind(&NtpClient::rtcDNSResolvedCallback, this, std::placeholders::_1);
                DnsLookup_Async(NtpServerDNS, &NtpServerAddress, callbackFunctionPtr);
            }
        }
    }
    else
    {
        ntpResult(-1, NULL);
    }
}

void NtpClient::rtcUpdate(bool WaitForResult)
{
    NtpRefreshInprogress = true;
    ntpRequest();
    if (WaitForResult)
    {
        absolute_time_t TimeoutTime = make_timeout_time_ms(*TimeoutSeconds * 1000); // Used to track timeouts
        while (NtpRefreshInprogress)                                                // Waiting for the MQTT connection to establish
        {
            if (get_absolute_time() > TimeoutTime) // No response from the server
            {
                ntpResult(-1, NULL);
                break;
            }
            sleep_ms(500);
        }
    }
}

void NtpClient::rtcDNSResolvedCallback(ip_addr_t *ServerIP)
{
    if (ServerIP)
    {
        NtpServerAddress = *ServerIP;
        rtcUpdate(false);
    }
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void NtpClient::run30min()
{
    getCurrentTime(true);
    //TODO: add logic to sync every 24 hours
    //TODO: run sync every time WiFi reconnects
}

// Make an NTP request
void NtpClient::ntpRequest()
{
    cyw43_arch_lwip_begin();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *)p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b;
    udp_sendto(NtpPcb, p, &NtpServerAddress, *NtpServerPort);
    pbuf_free(p);
    cyw43_arch_lwip_end();
}

// NTP data received
void NtpClient::ntpReceived(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);

    // Check the result
    if (ip_addr_cmp(addr, &((NtpClient *)arg)->NtpServerAddress) && port == *((NtpClient *)arg)->NtpServerPort && p->tot_len == NTP_MSG_LEN &&
        mode == 0x4 && stratum != 0)
    {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
        time_t epoch = seconds_since_1970;
        ((NtpClient *)arg)->ntpResult(0, &epoch);
    }
    else
    {
        ((NtpClient *)arg)->ntpResult(-1, NULL);
    }
    pbuf_free(p);
}

// Called in an interrupt with results of the NTP request
void NtpClient::ntpResult(int status, time_t *result)
{
    datetime_t timeTemp;
    if (status == 0 && result)
    {
        struct tm *utc = gmtime(result); // https://cplusplus.com/reference/ctime/tm/
        timeTemp = {
            .year = (int16_t)(utc->tm_year + 1900), // utc->tm_year: years since 1900
            .month = (int8_t)(utc->tm_mon + 1),     // utc->tm_mon: months since January (0-11)
            .day = (int8_t)utc->tm_mday,
            .dotw = (int8_t)utc->tm_wday, // utc->tm_wday: days since Sunday (0-6)
            .hour = (int8_t)(utc->tm_hour + *TimeZoneDifference),
            .min = (int8_t)utc->tm_min,
            .sec = (int8_t)utc->tm_sec};
    }
    else // NTP failed, set a fixed date
    {

        timeTemp = {
            .year = 2024, // tm_year: years since 1900
            .month = 4,   // tm_mon: months since January (0-11)
            .day = 20,
            .dotw = 5, // tm_wday: days since Sunday (0-6)
            .hour = 16,
            .min = 20,
            .sec = 00};
        printf("   NTP sync failed, fallback set");
    }
    rtc_set_datetime(&timeTemp);
    sleep_ms(200); // Wait for the RTC to update
    NtpRefreshInprogress = false;
}
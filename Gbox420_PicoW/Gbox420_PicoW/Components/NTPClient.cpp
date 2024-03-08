#include "NTPClient.h"

// Initialize the built-in RTC
void initializeRTC()
{
    printf("Initializing RTC...");
    rtc_init();
    ntp_update();
}

// Called in an interrupt with results of the NTP request
void ntp_result(NTP_T *state, int status, time_t *result)
{
    if (status == 0 && result)
    {
        struct tm *utc = gmtime(result);   // https://cplusplus.com/reference/ctime/tm/
        datetime_t timeTemp = {
            .year = (int16_t)(utc->tm_year + 1900),  // tm_year: years since 1900
            .month = (int8_t)(utc->tm_mon +1),  // tm_mon: months since January (0-11)
            .day = (int8_t)utc->tm_mday,
            .dotw = (int8_t)utc->tm_wday, // tm_wday: days since Sunday (0-6)
            .hour = (int8_t)(utc->tm_hour + TIMEZONEDIFFERENCE),
            .min = (int8_t)utc->tm_min,
            .sec = (int8_t)utc->tm_sec};

        rtc_set_datetime(&timeTemp);
    }

    if (state->ntp_resend_alarm > 0)
    {
        cancel_alarm(state->ntp_resend_alarm);
        state->ntp_resend_alarm = 0;
    }
    state->ntp_test_time = make_timeout_time_ms(NTP_TEST_TIME);
    state->dns_request_sent = false;
       
    free(state);
}

int64_t ntp_failed_handler(alarm_id_t id, void *user_data);

// Make an NTP request
void ntp_request(NTP_T *state)
{
    cyw43_arch_lwip_begin();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *)p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b;
    udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT);
    pbuf_free(p);
    cyw43_arch_lwip_end();
}

int64_t ntp_failed_handler(alarm_id_t id, void *user_data)
{
    NTP_T *state = (NTP_T *)user_data;
    printf("NTP request failed\n");
    ntp_result(state, -1, NULL);
    return 0;
}

// Call back with a DNS result
void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg)
{
    NTP_T *state = (NTP_T *)arg;
    if (ipaddr)
    {
        state->ntp_server_address = *ipaddr;
        // printf("ntp address %s\n", ipaddr_ntoa(ipaddr));
        ntp_request(state);
    }
    else
    {
        printf("NTP DNS request failed\n");
        ntp_result(state, -1, NULL);
    }
}

// NTP data received
void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    NTP_T *state = (NTP_T *)arg;
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);

    // Check the result
    if (ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN &&
        mode == 0x4 && stratum != 0)
    {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
        time_t epoch = seconds_since_1970;
        ntp_result(state, 0, &epoch);
    }
    else
    {
        printf("Invalid NTP response\n");
        ntp_result(state, -1, NULL);
    }
    pbuf_free(p);
}

void ntp_update()
{
    NTP_T *state = (NTP_T *)calloc(1, sizeof(NTP_T));
    state->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    udp_recv(state->ntp_pcb, ntp_recv, state);

    state->ntp_resend_alarm = add_alarm_in_ms(NTP_RESEND_TIME, ntp_failed_handler, state, true);
    cyw43_arch_lwip_begin();
    dns_gethostbyname(NTP_SERVER, &state->ntp_server_address, ntp_dns_found, state);
    cyw43_arch_lwip_end();
}
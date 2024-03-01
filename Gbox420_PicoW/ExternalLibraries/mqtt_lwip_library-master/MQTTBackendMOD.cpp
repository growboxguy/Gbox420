#include "MQTTBackend.h"
#include <stdint.h>
#include <string.h>

extern volatile uint32_t msTime;

void MQTTBackend::mqttInit(const char *Name, Module *Parent, Settings::MQTTSettings *DefaultSettings, mqttReceivedCallback fn)
{
    mqttReceivedCallback = fn;
    //pcb = tcp_new();
    Storage->server = serverIp;
    Storage->port = port;
    Storage->connected = 0;
    Storage->deviceId = devID;
    Storage->autoConnect = 0;
}


err_t MQTTBackend::recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    // printf("TCP callback from %d.%d.%d.%d\r\n", ip4_addr1(&(pcb->remote_ip)),ip4_addr2(&(pcb->remote_ip)),ip4_addr3(&(pcb->remote_ip)),ip4_addr4(&(pcb->remote_ip)));
    uint8_t *mqttData;
    Mqtt *this = arg;

    // uint8_t strLen = p->tot_len - 4;
    // char out[strLen + 1];
    /* Check if status is ok and data is arrived. */
    if (err == ERR_OK && p != NULL)
    {
        /* Inform TCP that we have taken the data. */

        tcp_recved(pcb, p->tot_len);
        mqttData = (uint8_t *)(p->payload);

        uint8_t *topic = mqttData + 2 + 2; // ok
        uint16_t topicLen = (mqttData[2] << 8) | mqttData[3];
        uint8_t *data = &mqttData[2 + 2 + topicLen];
        uint32_t dataLen = p->tot_len - (2 + 2 + topicLen);

        switch (mqttData[0] & 0xF0)
        {
        case MQTT_MSGT_PINGRESP:
            printf("PingResp\n");
            break;

        case MQTT_MSGT_PUBLISH:

            printf("Publish in\n");
            this->mqttReceivedCallback(this, topic, topicLen, data, dataLen);
            break;

        case MQTT_MSGT_CONACK:
            this->connected = 1;
            break;

        default:
            printf("default:\n");
        }

        //
    }
    else
    {
        /* No data arrived */
        /* That means the client closes the connection and sent us a packet with FIN flag set to 1. */
        /* We have to cleanup and destroy out TCPConnection. */
        // printf("Connection closed by client.\r\n");
        tcp_close(pcb);
    }
    pbuf_free(p);
    return ERR_OK;
}

/* Accept an incomming call on the registered port */
err_t MQTTBackend::accept_callback(void *arg, struct tcp_pcb *npcb, err_t err)
{    // LWIP_UNUSED_ARG(arg);
    Mqtt *this = arg;
    // flag++;

    printf("Recieve from broker.\n");
    // printf("\r\n");
    // this->connected = 1;    
    tcp_recv(npcb, recv_callback);     /* Subscribe a receive callback function */
    return ERR_OK; /* Don't panic! Everything is fine. */
}

void MQTTBackend::mqttDisconnectForced(Mqtt *this)
{
    if (!this->connected)
        return;

    printf("forceDisconnect()\n");

    // tcp_close(this->pcb);
    tcp_abort(this->pcb);

    this->connected = 0;
}


static void MQTTBackend::conn_err(void *arg, err_t err)
{

    LWIP_UNUSED_ARG(err);

    printf("conn_err\n");
}

static err_t MQTTBackend::http_poll(void *arg, struct tcp_pcb *pcb)
{
    // struct http_state *hs;
    // hs = arg;
    // LWIP_DEBUGF(HTTPD_DEBUG, ("http_poll 0x%08x\n", pcb));
    /*  printf("Polll\n");*/

    /*
    if ((hs == NULL) && (pcb->state == ESTABLISHED)) {

      tcp_abort(pcb);
      return ERR_ABRT;
    } else {
      ++hs->retries;
      if (hs->retries == 4) {
        tcp_abort(pcb);
        return ERR_ABRT;
      }

      // If this connection has a file open, try to send some more data. If
       // it has not yet received a GET request, don't do this since it will
      // cause the connection to close immediately.
      if(hs && (hs->handle)) {
        send_data(pcb, hs);
      }
    }*/
    Mqtt *this = arg;

    if (!this->connected && this->pollAbortCounter == 4)
    {
        printf("http_poll(ABORT!)\n");
        tcp_abort(pcb);
        return ERR_ABRT;
    }

    this->pollAbortCounter++;

    printf("http_poll()\n");

    return ERR_OK;
}

uint8_t MQTTBackend::mqttConnect(Mqtt *Storage)
{

    if (Storage->connected)
        return 1;

    Storage->pollAbortCounter = 0;

    Storage->pcb = tcp_new();
    err_t err = tcp_connect(Storage->pcb, &(Storage->server), 1883, accept_callback);

    if (err != ERR_OK)
    {
        printf("Connection Error : %d\r\n", err);
        mqttDisconnectForced(Storage);
        return 1;
    }
    else
    {
        printf("Connected..\r\n");
    }

    // Delay_ms(10);

    tcp_arg(Storage->pcb, this);

    tcp_err(Storage->pcb, conn_err);
    tcp_poll(Storage->pcb, http_poll, 4);
    tcp_accept(Storage->pcb, &accept_callback);

    // device_poll();

    // variable header
    uint8_t var_header[] = {0x00, 0x06, 0x4d, 0x51, 0x49, 0x73, 0x64, 0x70, 0x03, 0x02, 0x00, KEEPALIVE / 1000, 0x00, strlen(this->deviceId)};

    // fixed header: 2 bytes, big endian
    uint8_t fixed_header[] = {MQTTCONNECT, 12 + strlen(this->deviceId) + 2};

    char packet[sizeof(fixed_header) + sizeof(var_header) + strlen(this->deviceId)];

    // Clear memory
    memset(packet, 0, sizeof(packet));

    // Copy fixed header
    memcpy(packet, fixed_header, sizeof(fixed_header));
    // Copy variable header
    memcpy(packet + sizeof(fixed_header), var_header, sizeof(var_header));
    // Copy device name
    memcpy(packet + sizeof(fixed_header) + sizeof(var_header), this->deviceId, strlen(this->deviceId));

    // Send MQTT identification message to broker.
    err = tcp_write(this->pcb, (void *)packet, sizeof(packet), 1);

    if (err == ERR_OK)
    {
        tcp_output(this->pcb);
        this->lastActivity = msTime;
        // this->connected = 1;
        printf("Identification message sent...\n");
        return 0;
    }
    else
    {
        printf("Failed to send the identification message to broker...\n");
        printf("Error is: %d\n", err);
        mqttDisconnectForced(this);
        return 1;
    }

    return 0;
}

uint8_t MQTTBackend::mqttPublish(Mqtt *this, char *pub_topic, char *msg)
{

    if (!this->connected)
    {
        printf("Publish err:no this->connected");
        return 1;
    }

    uint8_t var_header_pub[strlen(pub_topic) + 3];
    strcpy((char *)&var_header_pub[2], pub_topic);
    var_header_pub[0] = 0;
    var_header_pub[1] = strlen(pub_topic);
    var_header_pub[sizeof(var_header_pub) - 1] = 0;

    uint8_t fixed_header_pub[] = {MQTTPUBLISH, sizeof(var_header_pub) + strlen(msg)};

    uint8_t packet_pub[sizeof(fixed_header_pub) + sizeof(var_header_pub) + strlen(msg)];
    memset(packet_pub, 0, sizeof(packet_pub));
    memcpy(packet_pub, fixed_header_pub, sizeof(fixed_header_pub));
    memcpy(packet_pub + sizeof(fixed_header_pub), var_header_pub, sizeof(var_header_pub));
    memcpy(packet_pub + sizeof(fixed_header_pub) + sizeof(var_header_pub), msg, strlen(msg));

    // Publish message
    err_t err = tcp_write(this->pcb, (void *)packet_pub, sizeof(packet_pub), 1); // TCP_WRITE_FLAG_MORE

    if (err == ERR_OK)
    {
        tcp_output(this->pcb);
        printf("Publish: %s ...\r\n", msg);
    }
    else
    {
        printf("Failed to publish...\r\n");
        printf("Error is: %d\r\n", err);
        mqttDisconnectForced(this);
        return 1;
    }
    // printf("\r\n");
    // device_poll();
    return 0;
}

static void MQTTBackend::close_conn(struct tcp_pcb *pcb)
{
    err_t err;
    // LWIP_DEBUGF(HTTPD_DEBUG, ("Closing connection 0x%08x\n", pcb));

    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);

    err = tcp_close(pcb);
    if (err != ERR_OK)
    {
        // LWIP_DEBUGF(HTTPD_DEBUG, ("Error %d closing 0x%08x\n", err, pcb));
    }
}

uint8_t MQTTBackend::mqttDisconnect(Mqtt *this)
{

    if (!this->connected)
        return 1;

    uint8_t packet_224[] = {2, 2, 4};
    tcp_write(this->pcb, (void *)packet_224, sizeof(packet_224), 1);
    tcp_write(this->pcb, (void *)(0), sizeof((0)), 1);
    // socket->send((char*)224,sizeof((char*)224));
    // socket->send((uint8_t)0,sizeof((uint8_t)0));
    tcp_close(this->pcb);
    // this->lastActivity = timer.read_ms();
    this->connected = 0;

    return 0;
}

uint8_t MQTTBackend::mqttSubscribe(Mqtt *this, char *topic)
{

    if (!this->connected)
        return -1;

    uint8_t var_header_topic[] = {0, 10};
    uint8_t fixed_header_topic[] = {MQTTSUBSCRIBE, sizeof(var_header_topic) + strlen(topic) + 3};

    // utf topic
    uint8_t utf_topic[strlen(topic) + 3];
    strcpy((char *)&utf_topic[2], topic);

    utf_topic[0] = 0;
    utf_topic[1] = strlen(topic);
    utf_topic[sizeof(utf_topic) - 1] = 0;

    char packet_topic[sizeof(var_header_topic) + sizeof(fixed_header_topic) + strlen(topic) + 3];
    memset(packet_topic, 0, sizeof(packet_topic));
    memcpy(packet_topic, fixed_header_topic, sizeof(fixed_header_topic));
    memcpy(packet_topic + sizeof(fixed_header_topic), var_header_topic, sizeof(var_header_topic));
    memcpy(packet_topic + sizeof(fixed_header_topic) + sizeof(var_header_topic), utf_topic, sizeof(utf_topic));

    // Send message
    err_t err = tcp_write(this->pcb, (void *)packet_topic, sizeof(packet_topic), 1); // TCP_WRITE_FLAG_MORE
    if (err == ERR_OK)
    {
        tcp_output(this->pcb);
        printf("Subscribe sucessfull to: %s...\r\n", topic);
    }
    else
    {
        printf("Failed to subscribe to: %s...\r\n", topic);
        printf("Error is: %d\r\n", err);
        mqttDisconnectForced(this);
        return 1;
    }
    printf("\r\n");
    // device_poll();

    return 0;
}

int MQTTBackend::mqttPing(Mqtt *this)
{
    MqttFixedHeader pingReq;

    if (!this->connected)
        return -1;

    pingReq.header = MQTT_PINGREQ_HEADER;
    pingReq.remainingLength = 0x00;

    // Publish message
    err_t err = tcp_write(this->pcb, (void *)&pingReq, sizeof(pingReq), 1); // TCP_WRITE_FLAG_MORE

    if (err == ERR_OK)
    {
        tcp_output(this->pcb);
        printf("Pinreq...\r\n");
    }
    else
    {
        printf("Failed to send PingReq...\r\n");
        printf("Error is: %d\r\n", err);
        mqttDisconnectForced(this);
        return -1;
    }

    return 1;
}

uint8_t MQTTBackend::mqttLive(Mqtt *this)
{

    uint32_t t = msTime;
    if (t - this->lastActivity > (KEEPALIVE - 2000))
    {

        if (this->connected)
        {
            printf("Sending keep-alive\n");
            mqttPing(this);
        }
        else if (this->autoConnect)
        {
            mqttConnect(this);
        }

        this->lastActivity = t;
    }

    return 0;
}

/*
int PubSub_mbed::connected() {
int rc = 0;(int)socket.connected();
if (!rc) {
socket.close();
}
return rc;
}
*/

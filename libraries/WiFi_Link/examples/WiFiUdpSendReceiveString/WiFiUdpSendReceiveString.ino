
/*
  WiFi UDP Send and Receive String

 This sketch wait an UDP packet on localPort using an Arduino board with WiFi Link.
 When a packet is received an Acknowledge packet is sent to the client on port remotePort

 created 30 December 2012
 by dlf (Metodo2 srl)
 modified 10 March 2017
 by Sergio Tomasello and Andrea Cannistr√°
 modified 19 Nov 2017
 by Juraj Andr·ssy
 */

#include <WiFiLink.h>
#include <WiFiUdp.h>
//#include <UnoWiFiDevEdSerial1.h> // change Serial1.begin to 115200

#if !defined(ESP_CH_SPI) && !defined(HAVE_HWSERIAL1)
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back

WiFiUDP Udp;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

#if !defined(ESP_CH_SPI)
  Serial1.begin(9600); // speed must match with BAUDRATE_COMMUNICATION setting in firmware config.h
  WiFi.init(&Serial1);
#endif
  if (WiFi.checkFirmwareVersion("1.1.0")) {
    WiFi.resetESP(); // to clear 'sockets' after sketch upload
  }
  delay(3000); //wait while WiFiLink firmware connects to WiFi with Web Panel settings
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}

void loop() {

  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

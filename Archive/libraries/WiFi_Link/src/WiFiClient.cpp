/*
  WiFiClient.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

extern "C" {
  #include "utility/wl_definitions.h"
  #include "utility/wl_types.h"
  #include "string.h"
  #include "utility/debug.h"
}

#include "WiFiLink.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "utility/server_drv.h"

WiFiClient::WiFiClient() : _sock(255) {
  availData = 0;
}

WiFiClient::WiFiClient(uint8_t sock) : _sock(sock) {
  availData = 0;
}

int WiFiClient::connect(const char* host, uint16_t port) {
	IPAddress remote_addr;
	if (WiFi.hostByName(host, remote_addr))
	{
		return connect(remote_addr, port);
	}
	return 0;
}

int WiFiClient::connect(IPAddress ip, uint16_t port) {
    _sock = WiFiClass::getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
    	ServerDrv::startClient(uint32_t(ip), port, _sock);

    	if (!connected())
       	{
    		return 0;
    	}
      WiFiClass::_state[_sock] = _sock;
    }else{
    	Serial.println("No Socket available");
    	return 0;
    }
    return 1;
}

size_t WiFiClient::write(uint8_t b) {
	  return write(&b, 1);
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
  if (_sock >= MAX_SOCK_NUM)
  {
	  setWriteError();
	  return 0;
  }
  if (size==0)
  {
	  setWriteError();
      return 0;
  }
  //Serial.print("size:");Serial.println(size);
  //if(size<=54){ //23
  if (!ServerDrv::sendData(_sock, buf, size))
  {
    setWriteError();
      return 0;
  }
  // }
  // else{
  //   int pckt_len = ceil((float)size/23); //23 //54
  //     //Serial.println(pckt_len);
  //   for(int i=0;i<pckt_len;i++){
  //     if(size >= ((i+1)*23)){ //23 //54
  //       //Serial.println("a");
  //       if (!ServerDrv::sendData(_sock, buf+(i*23), 23)) //54
  //       {
  //     	  setWriteError();
  //           return 0;
  //       }
  //     }
  //     else{
  //       //Serial.println("b");
  //       if (!ServerDrv::sendData(_sock, buf+(i*23), size-(i*23))) //54
  //       {
  //         setWriteError();
  //           return 0;
  //       }
  //     }
  //   }
  //
  // }
  // if (!ServerDrv::checkDataSent(_sock))
  // {
	//   setWriteError();
  //     return 0;
  // }

  return size;
}

int WiFiClient::available() {
  if (_sock != 255)
  {
      if(availData == 0 ){
        availData = ServerDrv::availData(_sock);
        return availData;
      }
      else                        //EDIT by Andrea
        return availData;
  }

  return 0;
}

int WiFiClient::read() {
  uint8_t b;
  if (!available())
    return -1;
  availData--;
  ServerDrv::getData(_sock, &b);
  //availData--;  //EDIT by Andrea
  return b;
}


int WiFiClient::read(uint8_t* buf, size_t size) {
  // sizeof(size_t) is architecture dependent
  // but we need a 16 bit data type here
  uint16_t _size = size;
  if (!ServerDrv::getDataBuf(_sock, buf, &_size))
      return -1;
  availData -= _size;
  if (availData < 0) {
    availData = 0;
  }
  return _size;
}

int WiFiClient::peek() {
	  uint8_t b;
	  if (!available())
	    return -1;

	  ServerDrv::getData(_sock, &b, 1);
	  return b;
}

void WiFiClient::flush() {
  while (available())
    read();
}

void WiFiClient::stop() {

  if (_sock == 255)
    return;

  ServerDrv::stopClient(_sock);
  WiFiClass::_state[_sock] = NA_STATE;
  availData = 0;
//  int count = 0;
  // wait maximum 5 secs for the connection to close
  // while (status() != CLOSED && ++count < 50)
  //   delay(100);

  _sock = 255;
}

uint8_t WiFiClient::connected() {

  if (_sock == 255) {
    return 0;
  } else {
    return (status() == ESTABLISHED);
  }
}

uint8_t WiFiClient::status() {
    if (_sock == 255) {
    return CLOSED;
  } else {
    return ServerDrv::getClientState(_sock);
  }
}

WiFiClient::operator bool() {
  return _sock != 255;
}

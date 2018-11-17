/*
  spi_drv.h - Library for Arduino Wifi shield.
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

#if defined(ESP_CH_SPI)

#ifndef SPI_Drv_h
#define SPI_Drv_h

#ifndef NULL
  #ifdef __cplusplus
    #define NULL 0
  #else
    #define NULL ((void*)0)
  #endif
#endif

#include <inttypes.h>
#include "utility/wifi_spi.h"

#define NO_LAST_PARAM   0
#define LAST_PARAM      1
#define DUMMY_DATA  0x00
#define SPI_SLAVE_BUFFER 32
#define STARTCMD_WAIT 15						//delay need to manage the event ESP side (delay in us)
#define STATUS_WAIT 20
#define MAX_WAIT_TIME 30000					//max wait time for slave response (delay in us)
#define MULTI_PACKET_TIME 60				//delay for packet size greater than 32
#define END_TX_PACKET 1
#define END_32L_PACKET 2

#if defined (ARDUINO_NRF52_PRIMO)
#define SLAVESELECT 31              // ss primo board            
#define SLAVEREADY  30              // handshake pin primo board
#elif defined (__AVR_ATmega328P__)
#define SLAVESELECT 10              // ss uno wifi board                 
#define SLAVEREADY  7               // handshake pin uno wifi board
#endif


#define WAIT_FOR_SLAVE_SELECT()	 \
  	SpiDrv::commSlaveSelect(); \

class SpiDrv
{
private:

    static void waitForSlaveSign();

    static void getParam(uint8_t* param);
public:

    static void begin();

    static void end();

    static void commDriverInit();

    static void commSlaveSelect();

    static void commSlaveDeselect();

    static char commTransfer(volatile char data);

    static void waitForSlaveReady();

    static int waitCommChar(unsigned char waitChar);

    static int readAndCheckChar(char checkChar, char* readChar);

    static char readChar();

    static int waitResponseParams(uint8_t cmd, uint8_t numParam, tParam* params);

    static int waitResponseCmd(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len);

    static int waitResponseData8(uint8_t cmd, uint8_t* param, uint8_t* param_len);

    static int waitResponseData8_debug(uint8_t cmd, uint8_t* param, uint8_t* param_len);

    static int waitResponseData16(uint8_t cmd, uint8_t* param, uint16_t* param_len);

    static int waitResponse(uint8_t cmd, uint8_t* numParamRead, uint8_t** params, uint8_t maxNumParams);

    static void sendParam(uint8_t* param, uint8_t param_len, uint8_t lastParam = NO_LAST_PARAM);

    static void sendParamLen8(uint8_t param_len);

    static void sendParamLen16(uint16_t param_len);

    static uint8_t readParamLen8(uint8_t* param_len = NULL);

    static void endPacket();

    static void checkTransferPacket();

    static void checkReceiverPacket();

    static void writeStatus(uint32_t status);

    static int readStatus();

    static uint16_t readParamLen16(uint16_t* param_len = NULL);

    static void sendBuffer(uint8_t* param, uint16_t param_len, uint8_t lastParam = NO_LAST_PARAM);

    static void sendParam(uint16_t param, uint8_t lastParam = NO_LAST_PARAM);

    static void sendCmd(uint8_t cmd, uint8_t numParam);
};

#endif
#endif

/*
  spi_drv.cpp - Library for Arduino Wifi shield.
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

#include "Arduino.h"
#include <SPI.h>
#include "spi_drv.h"
#include "pins_arduino.h"

extern "C" {
#include "utility/debug.h"
}

enum ESP_SPI_COMMANDS
{
	ESP8266_STATUS_READ			= 0x04,
	ESP8266_STATUS_WRITE		= 0X01,
	ESP8266_DATA_READ			= 0x03,
	ESP8266_DATA_WRITE		= 0x02

} ESP_SPI_COMMANDS;


int byte_transfer = 0;		//number of transfered bytes
bool spi_begin = true;		//check spi begin

void SpiDrv::begin()
{

}

void SpiDrv::end() {
    SPI.end();
}

void SpiDrv::commSlaveSelect()
{
		if(spi_begin){
				delay(2000);
				//waitForSlaveSign();
        SPI.begin();

				pinMode(SLAVEREADY, INPUT);
				pinMode(SLAVESELECT, OUTPUT);

				// digitalWrite(SCK, LOW);
				// digitalWrite(MOSI, LOW);
				// digitalWrite(SS, HIGH);
				digitalWrite(SLAVESELECT, HIGH);
				spi_begin = false;

		 }
    digitalWrite(SLAVESELECT,LOW);
}


void SpiDrv::commSlaveDeselect()
{
    digitalWrite(SLAVESELECT,HIGH);
}


char SpiDrv::commTransfer(volatile char data)
{
    char result = SPI.transfer(data);
		byte_transfer++;

    return result;                    // return the received byte
}
int SpiDrv::readStatus()
{
		SPI.transfer(ESP8266_STATUS_READ);
		uint32_t status = (SPI.transfer(0) | ((uint32_t)(SPI.transfer(0)) << 8) | ((uint32_t)(SPI.transfer(0)) << 16) | ((uint32_t)(SPI.transfer(0)) << 24));
		return status;
}
void SpiDrv::writeStatus(uint32_t status)
{

		SPI.transfer(ESP8266_STATUS_WRITE);
		SPI.transfer(status & 0xFF);
		SPI.transfer((status >> 8) & 0xFF);
		SPI.transfer((status >> 16) & 0xFF);
		SPI.transfer((status >> 24) & 0xFF);

}

int SpiDrv::waitCommChar(unsigned char waitChar)
{
    int timeout = TIMEOUT_CHAR;
    unsigned char _readChar = 0;
    do{
        _readChar = readChar(); //get data byte
        if (_readChar == ERR_CMD)
        {
        	WARN("Err cmd received\n");
        	return -1;
        }
    }while((timeout-- > 0) && (_readChar != waitChar));
    return  (_readChar == waitChar);
}

int SpiDrv::readAndCheckChar(char checkChar, char* readChar)
{
    getParam((uint8_t*)readChar);

    return  (*readChar == checkChar);
}

char SpiDrv::readChar()
{
	uint8_t readChar = 0;
	getParam(&readChar);
	return readChar;
}

#define ESP_SPI_DATA_WRITE(x)                 \
    delayMicroseconds(x);         						\
    byte_transfer=0;                          \
    SPI.transfer(ESP8266_DATA_WRITE);	        \
    SPI.transfer(DUMMY_DATA);									\

#define ESP_SPI_DATA_READ(x)                  \
    delayMicroseconds(x);											\
    SPI.transfer(ESP8266_DATA_READ);	        \
    SPI.transfer(DUMMY_DATA);									\

#define WAIT_START_CMD(x) waitCommChar(START_CMD)

#define IF_CHECK_START_CMD(x)										\
    if (!WAIT_START_CMD(_data))                 \
    {                                           \
        WARN("Error waiting START_CMD");        \
        endPacket();														\
				Serial.print("Error");Serial.print(x,HEX);	\
        return 0;                               \
    }else                                       \

#define CHECK_DATA(check, x)										\
        if (!readAndCheckChar(check, &x)) 			\
        {                                       \
            WARN("Reply error");                \
            endPacket();												\
						Serial.print("ErrorCheck");Serial.print(check,HEX);Serial.print(x,HEX);	\
            return 0;                                   \
        }else                                           \

#define waitSlaveReady()                                        \
  unsigned long previousMillis = millis();                      \
  while(digitalRead(SLAVEREADY) == LOW){                        \
    unsigned long currentMillis = millis();                     \
    if (currentMillis - previousMillis >= MAX_WAIT_TIME)				\
    	break;                                                    \
  }                                                             \

#define waitSlaveSign() (digitalRead(SLAVEREADY) == HIGH)
#define waitSlaveSignalH() while(digitalRead(SLAVEREADY) != HIGH){}
#define waitSlaveSignalL() while(digitalRead(SLAVEREADY) != LOW){}

void SpiDrv::waitForSlaveSign()
{
	while (digitalRead(SLAVEREADY) != LOW);//)(!waitSlaveSign());
}

void SpiDrv::waitForSlaveReady()
{

	delayMicroseconds(STATUS_WAIT);
	writeStatus(1);
	commSlaveDeselect();
  waitSlaveReady();
	commSlaveSelect();
}

void SpiDrv::getParam(uint8_t* param)
{
    // Get Params data
    *param = commTransfer(DUMMY_DATA);

}

int SpiDrv::waitResponseCmd(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int ii = 0;

    //SPI read command for ESP
    ESP_SPI_DATA_READ(STARTCMD_WAIT);

    IF_CHECK_START_CMD(_data)
    {

        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        CHECK_DATA(numParam, _data);
        {
            readParamLen8(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
								checkReceiverPacket();
                getParam(&param[ii]);
            }
        }
				checkReceiverPacket();
        readAndCheckChar(END_CMD, &_data);
				endPacket();

    }
    return 1;
}

int SpiDrv::waitResponseData16(uint8_t cmd, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    uint16_t ii = 0;

    //SPI read command for ESP
    ESP_SPI_DATA_READ(STARTCMD_WAIT);

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();
        if (numParam != 0)
        {

            readParamLen16(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
								checkReceiverPacket();
                param[ii] = commTransfer(DUMMY_DATA);
								//Serial.println(param[ii]);
            }
        }
				checkReceiverPacket();
        readAndCheckChar(END_CMD, &_data);
				endPacket();
    }

    return 1;
}

int SpiDrv::waitResponseData8(uint8_t cmd, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int ii = 0;

    //SPI read command for ESP
    ESP_SPI_DATA_READ(STARTCMD_WAIT);

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();
        if (numParam != 0)
        {
            readParamLen8(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
								checkReceiverPacket();
                param[ii] = commTransfer(DUMMY_DATA);
            }
        }

				checkReceiverPacket();
        readAndCheckChar(END_CMD, &_data);
				endPacket();
    }

    return 1;
}

int SpiDrv::waitResponseParams(uint8_t cmd, uint8_t numParam, tParam* params)
{
    char _data = 0;
    int i =0, ii = 0;

    //SPI read command for ESP
    ESP_SPI_DATA_READ(STARTCMD_WAIT);

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t _numParam = readChar();
        if (_numParam != 0)
        {
            for (i=0; i<_numParam; ++i)
            {
								checkReceiverPacket();
                params[i].paramLen = readParamLen8();
                for (ii=0; ii<params[i].paramLen; ++ii)
                {
                    // Get Params data
										checkReceiverPacket();
                    params[i].param[ii] = commTransfer(DUMMY_DATA);
                }
            }
        } else
        {
            WARN("Error numParam == 0");
            return 0;
        }

        if (numParam != _numParam)
        {
            WARN("Mismatch numParam");
            return 0;
        }
				checkReceiverPacket();
        readAndCheckChar(END_CMD, &_data);
				endPacket();
    }
    return 1;
}

int SpiDrv::waitResponse(uint8_t cmd, uint8_t* numParamRead, uint8_t** params, uint8_t maxNumParams)
{
    char _data = 0;
    int i =0, ii = 0;

    char    *index[WL_SSID_MAX_LENGTH];

    for (i = 0 ; i < WL_NETWORKS_LIST_MAXNUM ; i++)
            index[i] = (char *)params + WL_SSID_MAX_LENGTH*i;

    //SPI read command for ESP
    ESP_SPI_DATA_READ(STARTCMD_WAIT);

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();

        if (numParam > maxNumParams)
        {
            numParam = maxNumParams;
        }
        *numParamRead = numParam;
        if (numParam != 0)
        {
            for (i=0; i<numParam; ++i)
            {
							checkReceiverPacket();
            	uint8_t paramLen = readParamLen8();
                for (ii=0; ii<paramLen; ++ii)
                {
                    // Get Params data
										checkReceiverPacket();
                    index[i][ii] = (uint8_t)commTransfer(DUMMY_DATA);

                }
                index[i][ii]=0;
            }
        } else
        {
            WARN("Error numParams == 0");
            readAndCheckChar(END_CMD, &_data);
            return 0;
        }
				checkReceiverPacket();
        readAndCheckChar(END_CMD, &_data);
				endPacket();
    }
    return 1;
}


void SpiDrv::sendParam(uint8_t* param, uint8_t param_len, uint8_t lastParam)
{
    int i = 0;

    // Send Spi paramLen
    sendParamLen8(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
			checkTransferPacket();
      commTransfer(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1){
				checkTransferPacket();
        commTransfer(END_CMD);
				endPacket();
		}

}

void SpiDrv::checkTransferPacket(){
	if(byte_transfer == SPI_SLAVE_BUFFER){

     //for size data greater then 32 byte
     ESP_SPI_DATA_WRITE(MULTI_PACKET_TIME);

	}
}

void SpiDrv::checkReceiverPacket(){
	if(byte_transfer == SPI_SLAVE_BUFFER){
		byte_transfer = 0;
		delayMicroseconds(STATUS_WAIT);
		writeStatus(2);
		commSlaveDeselect();
		delayMicroseconds(20);
    waitSlaveReady();
		commSlaveSelect();

    //SPI read command for ESP
    ESP_SPI_DATA_READ(STARTCMD_WAIT);

	}
}

void SpiDrv::endPacket(){

		for(int i=byte_transfer;i<32;i++)
				SPI.transfer(0);

		byte_transfer = 0;
}

void SpiDrv::sendParamLen8(uint8_t param_len)
{
    // Send Spi paramLen
    commTransfer(param_len);

}

void SpiDrv::sendParamLen16(uint16_t param_len)
{
    // Send Spi paramLen
    commTransfer((uint8_t)((param_len & 0xff00)>>8));
    commTransfer((uint8_t)(param_len & 0xff));

}

uint8_t SpiDrv::readParamLen8(uint8_t* param_len)
{
    uint8_t _param_len = commTransfer(DUMMY_DATA);
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}

uint16_t SpiDrv::readParamLen16(uint16_t* param_len)
{
    uint16_t _param_len = commTransfer(DUMMY_DATA)<<8 | (commTransfer(DUMMY_DATA)& 0xff);
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}


void SpiDrv::sendBuffer(uint8_t* param, uint16_t param_len, uint8_t lastParam)
{
    uint16_t i = 0;

    // Send Spi paramLen
    sendParamLen16(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
				checkTransferPacket();
        commTransfer(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1){
				checkTransferPacket();      //check size packet
        commTransfer(END_CMD);
				endPacket();

		}
}


void SpiDrv::sendParam(uint16_t param, uint8_t lastParam)
{
	// Send Spi paramLen
	sendParamLen8(2);

	commTransfer((uint8_t)((param & 0xff00)>>8));
	commTransfer((uint8_t)(param & 0xff));

	//if lastParam is 1 Send Spi END CMD
	if (lastParam == 1){
			commTransfer(END_CMD);
			endPacket();
	}

}

/* Cmd Struct Message */
/* _________________________________________________________________________________  */
/*| START CMD | C/R  | CMD  |[TOT LEN]| N.PARAM | PARAM LEN | PARAM  | .. | END CMD | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */
/*|   8 bit   | 1bit | 7bit |  8bit   |  8bit   |   8bit    | nbytes | .. |   8bit  | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */

void SpiDrv::sendCmd(uint8_t cmd, uint8_t numParam)
{

    //write SPI command need for ESP
		//SPI.beginTransaction(settings);
    ESP_SPI_DATA_WRITE(STARTCMD_WAIT);

		// Send Spi START CMD
    commTransfer(START_CMD);

    // Send Spi C + cmd
    commTransfer(cmd & ~(REPLY_FLAG));

    // Send Spi numParam
    commTransfer(numParam);

    // If numParam == 0 send END CMD
    if (numParam == 0){
        commTransfer(END_CMD);
				endPacket();
		}

}

#endif

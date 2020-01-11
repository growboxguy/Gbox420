/*
  uart_drv.cpp - Library for Arduino Wifi shield.
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
#if !defined(ESP_CH_SPI)

#include "Arduino.h"
#include "uart_drv.h"
#include "serial.h"
// #include "pins_arduino.h"
//#define _DEBUG_
extern "C" {
#include "utility/debug.h"
}


void UartDrv::begin()
{
	  //wfSerial.begin();
#ifdef _DEBUG_
	  INIT_TRIGGER()
#endif
}

void UartDrv::end() {
    return;
}

void UartDrv::commSlaveSelect()
{
    return;
}


void UartDrv::commSlaveDeselect()
{
    return;
}


/*
char UartDrv::commTransfer(volatile char data)
{
    char result = SPI.transfer(data);
    DELAY_TRANSFER();

    return result;                    // return the received byte
}
*/


#define WAIT_START_CMD(x) waitCommChar(START_CMD)

#define UART_IF_CHECK_START_CMD(x)                      \
    if (!WAIT_START_CMD(_data))                 \
    {                                           \
        TOGGLE_TRIGGER()                        \
        WARN("Error waiting START_CMD");        \
        return 0;                               \
    }else                                       \

#define UART_CHECK_DATA(check, x)                   \
        if (!readAndCheckChar(check, &x))   \
        {                                               \
        	TOGGLE_TRIGGER()                        \
            WARN("Reply error");                        \
            INFO2(check, (uint8_t)x);							\
            return 0;                                   \
        }else                                           \



int UartDrv::waitCommChar(unsigned char waitChar)
{
    int timeout = TIMEOUT_CHAR;
    unsigned char _readChar = 0;  //set to 0
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

int UartDrv::readAndCheckChar(char checkChar, char* readChar)
{
    getParam((uint8_t*)readChar);

    return  (*readChar == checkChar);
}

char UartDrv::readChar()
{
	uint8_t readChar = 0;
	getParam(&readChar);
	return readChar;
}

void UartDrv::waitForSlaveSign()
{
	return;
}

void UartDrv::waitForSlaveReady()
{
	return;
}

void UartDrv::getParam(uint8_t* param)
{
    // Get Params data
    do {
        delayMicroseconds(10);
    }while(!wfSerial.available());

    *param = wfSerial.read();

}

int UartDrv::waitResponseCmd(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int idx = 0;

	 	UART_IF_CHECK_START_CMD(_data){

		 		UART_CHECK_DATA(cmd | REPLY_FLAG, _data){};

	  		UART_CHECK_DATA(numParam, _data);
		 		{

					readParamLen8(param_len);

					for (int ii=0; ii<(*param_len); ++ii)
					{

							getParam(&param[ii]);

					}
		 		}

				readAndCheckChar(END_CMD, &_data);
		}
    return 1;
}
/*
int UartDrv::waitResponse(uint8_t cmd, uint8_t numParam, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        CHECK_DATA(numParam, _data);
        {
            readParamLen16(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = spiTransfer(DUMMY_DATA);
            }
        }

        readAndCheckChar(END_CMD, &_data);
    }

    return 1;
}
*/


//TODO
int UartDrv::waitResponseData16(uint8_t cmd, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    uint16_t ii = 0;

    UART_IF_CHECK_START_CMD(_data)
    {
        UART_CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();
        if (numParam != 0)
        {
            readParamLen16(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = readChar();
            }
        }

        readAndCheckChar(END_CMD, &_data);
    }

    return 1;
}

//TODO

int UartDrv::waitResponseData8(uint8_t cmd, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
		int ii = 0, idx=0;

		UART_IF_CHECK_START_CMD(_data)
    {
        UART_CHECK_DATA(cmd | REPLY_FLAG, _data){};
      //  uint8_t numParam = raw_p;
				uint8_t numParam = readChar();
				if (numParam != 0)
				{
					readParamLen8(param_len);
					for (ii=0; ii<(*param_len); ++ii)
					{
							// Get Params data
							getParam(&param[ii]);
					}
			}
			readAndCheckChar(END_CMD, &_data);
	}

    return 1;
}

//TODO
int UartDrv::waitResponseParams(uint8_t cmd, uint8_t numParam, tParam* params)
{
    char _data = 0;
    int i =0, ii = 0, idx=0;

    UART_IF_CHECK_START_CMD(_data)
    {
        UART_CHECK_DATA(cmd | REPLY_FLAG, _data){};
				uint8_t _numParam = readChar();
        if (_numParam != 0)
        {
            for (i=0; i<_numParam; ++i)
            {
                params[i].paramLen = readParamLen8();
                for (ii=0; ii<params[i].paramLen; ++ii)
                {
                    // Get Params data
                    params[i].param[ii] = readChar();
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

        readAndCheckChar(END_CMD, &_data);
		}
    return 1;
}

int UartDrv::waitResponse(uint8_t cmd, uint8_t* numParamRead, uint8_t** params, uint8_t maxNumParams)
{
    char _data = 0;
    int i =0, ii = 0,idx=0;

    char    *index[WL_SSID_MAX_LENGTH];

    for (i = 0 ; i < WL_NETWORKS_LIST_MAXNUM ; i++)
            index[i] = (char *)params + WL_SSID_MAX_LENGTH*i;

		UART_IF_CHECK_START_CMD(_data)
    {
        UART_CHECK_DATA(cmd | REPLY_FLAG, _data){};

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
            	uint8_t paramLen = readParamLen8();
                for (ii=0; ii<paramLen; ++ii)
                {
                    // Get Params data
										index[i][ii] = readChar();

                }
                index[i][ii]=0;
            }
        } else
        {
            WARN("Error numParams == 0");
            readAndCheckChar(END_CMD, &_data);
            return 0;
        }
        readAndCheckChar(END_CMD, &_data);
    }
    return 1;
}


void UartDrv::sendParam(uint8_t* param, uint8_t param_len, uint8_t lastParam)
{
    int i = 0;
    // Send Spi paramLen
    sendParamLen8(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
        wfSerial.write(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        wfSerial.write(END_CMD);
}

void UartDrv::sendParamLen8(uint8_t param_len)
{
    // Send Spi paramLen
    wfSerial.write(param_len);
}

void UartDrv::sendParamLen16(uint16_t param_len)
{
    // Send Spi paramLen
    wfSerial.write((uint8_t)((param_len & 0xff00)>>8));
    wfSerial.write((uint8_t)(param_len & 0xff));
}

uint8_t UartDrv::readParamLen8(uint8_t* param_len)
{
    uint8_t _param_len = readChar();
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}

uint16_t UartDrv::readParamLen16(uint16_t* param_len)
{
    uint16_t _param_len = readChar() <<8 | (readChar() & 0xff);
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}


void UartDrv::sendBuffer(uint8_t* param, uint16_t param_len, uint8_t lastParam)
{
    uint16_t i = 0;

    // Send Spi paramLen
    sendParamLen16(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
        wfSerial.write(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        wfSerial.write(END_CMD);
}


void UartDrv::sendParam(uint16_t param, uint8_t lastParam)
{
    // Send Spi paramLen
    sendParamLen8(2);

    wfSerial.write((uint8_t)((param & 0xff00)>>8));
    wfSerial.write((uint8_t)(param & 0xff));

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        wfSerial.write(END_CMD);
}

/* Cmd Struct Message */
/* _________________________________________________________________________________  */
/*| START CMD | C/R  | CMD  |[TOT LEN]| N.PARAM | PARAM LEN | PARAM  | .. | END CMD | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */
/*|   8 bit   | 1bit | 7bit |  8bit   |  8bit   |   8bit    | nbytes | .. |   8bit  | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */

void UartDrv::sendCmd(uint8_t cmd, uint8_t numParam)
{
    // Send Spi START CMD
    wfSerial.write(START_CMD);

    // Send Spi C + cmd
    wfSerial.write(cmd & ~(REPLY_FLAG));

    // Send Spi numParam
    wfSerial.write(numParam);

    // If numParam == 0 send END CMD
    if (numParam == 0)
        wfSerial.write(END_CMD);

}

#endif

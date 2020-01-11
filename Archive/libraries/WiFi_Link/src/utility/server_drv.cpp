/*
  server_drv.cpp - Library for Arduino Wifi shield.
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

//#define _DEBUG_

#include "utility/server_drv.h"

#include "Arduino.h"
#include "utility/comm.h"

extern "C" {
#include "utility/wl_types.h"
#include "utility/debug.h"
}


// Start server TCP on port specified
void ServerDrv::startServer(uint16_t port, uint8_t sock, uint8_t protMode)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(START_SERVER_TCP_CMD, PARAM_NUMS_3);
    commDrv.sendParam(port);
    commDrv.sendParam(&sock, 1);
    commDrv.sendParam(&protMode, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(START_SERVER_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
}

// Start server TCP on port specified
void ServerDrv::startClient(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_4);
    commDrv.sendParam((uint8_t*)&ipAddress, sizeof(ipAddress));
    commDrv.sendParam(port);
    commDrv.sendParam(&sock, 1);
    commDrv.sendParam(&protMode, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
}

// Start server TCP on port specified
void ServerDrv::stopClient(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(STOP_CLIENT_TCP_CMD, PARAM_NUMS_1);
    commDrv.sendParam(&sock, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(STOP_CLIENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
}


uint8_t ServerDrv::getServerState(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(GET_STATE_TCP_CMD, PARAM_NUMS_1);
    commDrv.sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(GET_STATE_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
   return _data;
}

uint8_t ServerDrv::getClientState(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(GET_CLIENT_STATE_TCP_CMD, PARAM_NUMS_1);
    commDrv.sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(GET_CLIENT_STATE_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
   return _data;
}

uint16_t ServerDrv::availData(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1);
    commDrv.sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
	uint16_t len = 0;

    commDrv.waitResponseCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1, (uint8_t*)&len,  &_dataLen);

    commDrv.commSlaveDeselect();

    return len;
}

bool ServerDrv::getData(uint8_t sock, uint8_t *data, uint8_t peek)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(GET_DATA_TCP_CMD, PARAM_NUMS_2);
    commDrv.sendParam(&sock, sizeof(sock));
    commDrv.sendParam(peek, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseData8(GET_DATA_TCP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
    if (_dataLen!=0)
    {
        *data = _data;
        return true;
    }
    return false;
}

bool ServerDrv::getDataBuf(uint8_t sock, uint8_t *_data, uint16_t *_dataLen)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(GET_DATABUF_TCP_CMD, PARAM_NUMS_2);
    commDrv.sendBuffer(&sock, sizeof(sock));
    uint8_t tmp[2];
    tmp[0] = (*_dataLen) >> 8;
    tmp[1] = (*_dataLen) & 0x00FF;
    commDrv.sendBuffer(tmp, (uint16_t) 2, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    if (!commDrv.waitResponseData16(GET_DATABUF_TCP_CMD, _data, _dataLen))
    {
        WARN("error waitResponse");
        (*_dataLen) = 0;
    }
    commDrv.commSlaveDeselect();
    if (*_dataLen!=0)
    {
        return true;
    }
    return false;
}

bool ServerDrv::insertDataBuf(uint8_t sock, const uint8_t *data, uint16_t _len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(INSERT_DATABUF_CMD, PARAM_NUMS_2);
    commDrv.sendBuffer(&sock, sizeof(sock));
    commDrv.sendBuffer((uint8_t *)data, _len, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseData8(INSERT_DATABUF_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}

bool ServerDrv::sendUdpData(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(SEND_DATA_UDP_CMD, PARAM_NUMS_1);
    commDrv.sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseData8(SEND_DATA_UDP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}


bool ServerDrv::sendData(uint8_t sock, const uint8_t *data, uint16_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(SEND_DATA_TCP_CMD, PARAM_NUMS_2);
    commDrv.sendBuffer(&sock, sizeof(sock));
    commDrv.sendBuffer((uint8_t *)data, len, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseData8(SEND_DATA_TCP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}


uint8_t ServerDrv::checkDataSent(uint8_t sock)
{
	const uint16_t TIMEOUT_DATA_SENT = 25;
    uint16_t timeout = 0;
	uint8_t _data = 0;
	uint8_t _dataLen = 0;

	do {
		WAIT_FOR_SLAVE_SELECT();
		// Send Command
		commDrv.sendCmd(DATA_SENT_TCP_CMD, PARAM_NUMS_1);
		commDrv.sendParam(&sock, sizeof(sock), LAST_PARAM);

		//Wait the reply elaboration
		commDrv.waitForSlaveReady();

		// Wait for reply
		if (!commDrv.waitResponseCmd(DATA_SENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
		{
			WARN("error waitResponse isDataSent");
		}
		commDrv.commSlaveDeselect();

		if (_data) timeout = 0;
		else{
			++timeout;
			delay(100);
		}

	}while((_data==0)&&(timeout<TIMEOUT_DATA_SENT));
    return (timeout==TIMEOUT_DATA_SENT)?0:1;
}

ServerDrv serverDrv;

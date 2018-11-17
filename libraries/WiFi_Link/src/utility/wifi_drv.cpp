/*
  wifi_drv.cpp - Library for Arduino Wifi shield.
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "Arduino.h"
#include "utility/comm.h"

#include "utility/wifi_drv.h"

//#define _DEBUG_

extern "C" {
#include "utility/wifi_spi.h"
#include "utility/wl_types.h"
#include "utility/debug.h"
}

// Array of data to cache the information related to the networks discovered
char 	WiFiDrv::_networkSsid[][WL_SSID_MAX_LENGTH] = {{"1"},{"2"},{"3"},{"4"},{"5"}};
int32_t WiFiDrv::_networkRssi[WL_NETWORKS_LIST_MAXNUM] = { 0 };
uint8_t WiFiDrv::_networkEncr[WL_NETWORKS_LIST_MAXNUM] = { 0 };

// Cached values of retrieved data
char 	WiFiDrv::_ssid[] = {0};
uint8_t	WiFiDrv::_bssid[] = {0};
uint8_t WiFiDrv::_mac[] = {0};
uint8_t WiFiDrv::_localIp[] = {0};
uint8_t WiFiDrv::_subnetMask[] = {0};
uint8_t WiFiDrv::_gatewayIp[] = {0};
// Firmware version
char WiFiDrv::fwVersion[] = {0};


// Private Methods

void WiFiDrv::getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip)
{
    tParam params[PARAM_NUMS_3] = { {0, (char*)ip}, {0, (char*)mask}, {0, (char*)gwip}};

    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_IPADDR_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    commDrv.sendParam(&_dummy, sizeof(_dummy), LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    commDrv.waitResponseParams(GET_IPADDR_CMD, PARAM_NUMS_3, params);

    commDrv.commSlaveDeselect();
}

void WiFiDrv::getRemoteData(uint8_t sock, uint8_t *ip, uint8_t *port)
{
    tParam params[PARAM_NUMS_2] = { {0, (char*)ip}, {0, (char*)port} };

    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_REMOTE_DATA_CMD, PARAM_NUMS_1);
    commDrv.sendParam(&sock, sizeof(sock), LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    commDrv.waitResponseParams(GET_REMOTE_DATA_CMD, PARAM_NUMS_2, params);

    commDrv.commSlaveDeselect();
}


// Public Methods


void WiFiDrv::wifiDriverInit()
{
    commDrv.begin();
}

void WiFiDrv::resetESP()
{
  WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(RESET_ESP_CMD, PARAM_NUMS_0);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = -1;
    uint8_t _dataLen = 0;

    commDrv.waitResponseCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    commDrv.commSlaveDeselect();
}

int8_t WiFiDrv::wifiSetNetwork(char* ssid, uint8_t ssid_len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(SET_NET_CMD, PARAM_NUMS_1);
    commDrv.sendParam((uint8_t*)ssid, ssid_len, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(500);
    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(SET_NET_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    commDrv.commSlaveDeselect();

    return(_data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
}

int8_t WiFiDrv::wifiSetPassphrase(char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_2);
    commDrv.sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    commDrv.sendParam((uint8_t*)passphrase, len, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(500);
    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    commDrv.commSlaveDeselect();
    return _data;
}


int8_t WiFiDrv::wifiSetKey(char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(SET_KEY_CMD, PARAM_NUMS_3);
    commDrv.sendParam((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
    commDrv.sendParam(&key_idx, KEY_IDX_LEN, NO_LAST_PARAM);
    commDrv.sendParam((uint8_t*)key, len, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(500);
    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(SET_KEY_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    commDrv.commSlaveDeselect();
    return _data;
}

void WiFiDrv::config(uint8_t validParams, uint32_t local_ip, uint32_t gateway, uint32_t subnet)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(SET_IP_CONFIG_CMD, PARAM_NUMS_4);
    commDrv.sendParam((uint8_t*)&validParams, 1, NO_LAST_PARAM);
    commDrv.sendParam((uint8_t*)&local_ip, 4, NO_LAST_PARAM);
    commDrv.sendParam((uint8_t*)&gateway, 4, NO_LAST_PARAM);
    commDrv.sendParam((uint8_t*)&subnet, 4, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(SET_IP_CONFIG_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    commDrv.commSlaveDeselect();
}

void WiFiDrv::setDNS(uint8_t validParams, uint32_t dns_server1, uint32_t dns_server2)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(SET_DNS_CONFIG_CMD, PARAM_NUMS_3);
    commDrv.sendParam((uint8_t*)&validParams, 1, NO_LAST_PARAM);
    commDrv.sendParam((uint8_t*)&dns_server1, 4, NO_LAST_PARAM);
    commDrv.sendParam((uint8_t*)&dns_server2, 4, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(SET_DNS_CONFIG_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
        _data = WL_FAILURE;
    }
    commDrv.commSlaveDeselect();
}



int8_t WiFiDrv::disconnect()
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(DISCONNECT_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    commDrv.sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    int8_t result = commDrv.waitResponseCmd(DISCONNECT_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    commDrv.commSlaveDeselect();

    return result;
}

uint8_t WiFiDrv::getConnectionStatus()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_0);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = -1;
    uint8_t _dataLen = 0;

    commDrv.waitResponseCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    commDrv.commSlaveDeselect();

    return _data;
}

uint8_t* WiFiDrv::getMacAddress()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_MACADDR_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    commDrv.sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
    commDrv.waitResponseCmd(GET_MACADDR_CMD, PARAM_NUMS_1, _mac, &_dataLen);

    commDrv.commSlaveDeselect();

    return _mac;
}

void WiFiDrv::getIpAddress(IPAddress& ip)
{
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	ip = _localIp;
}

 void WiFiDrv::getSubnetMask(IPAddress& mask)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	mask = _subnetMask;
 }

 void WiFiDrv::getGatewayIP(IPAddress& ip)
 {
	getNetworkData(_localIp, _subnetMask, _gatewayIp);
	ip = _gatewayIp;
 }

char* WiFiDrv::getCurrentSSID()
{
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    commDrv.sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(100);

    // Wait for reply
     uint8_t _dataLen = 0;
     commDrv.waitResponseCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1, (uint8_t*)_ssid, &_dataLen);

     commDrv.commSlaveDeselect();

    return _ssid;
}

uint8_t* WiFiDrv::getCurrentBSSID()
{
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    commDrv.sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(200);
    // Wait for reply
    uint8_t _dataLen = 0;
    commDrv.waitResponseCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1, _bssid, &_dataLen);
    commDrv.commSlaveDeselect();

    return _bssid;
}

int32_t WiFiDrv::getCurrentRSSI()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    commDrv.sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _dataLen = 0;
    int32_t rssi = 0;
    commDrv.waitResponseCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1, (uint8_t*)&rssi, &_dataLen);

    commDrv.commSlaveDeselect();

    return rssi;
}

uint8_t WiFiDrv::getCurrentEncryptionType()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1);

    uint8_t _dummy = DUMMY_DATA;
    commDrv.sendParam(&_dummy, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(2500);

    // Wait for reply
    uint8_t dataLen = 0;
    uint8_t encType = 0;
    commDrv.waitResponseCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1, (uint8_t*)&encType, &dataLen);

    commDrv.commSlaveDeselect();

    return encType;
}

int8_t WiFiDrv::startScanNetworks()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(START_SCAN_NETWORKS, PARAM_NUMS_0);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;

    if (!commDrv.waitResponseCmd(START_SCAN_NETWORKS, PARAM_NUMS_1, &_data, &_dataLen))
     {
         WARN("error waitResponse");
         _data = WL_FAILURE;
     }

    commDrv.commSlaveDeselect();
    return (_data == WL_FAILURE)? _data : WL_SUCCESS;
}


uint8_t WiFiDrv::getScanNetworks()
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(SCAN_NETWORKS, PARAM_NUMS_0);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(200);
    // Wait for reply
    uint8_t ssidListNum = 0;
    commDrv.waitResponse(SCAN_NETWORKS, &ssidListNum, (uint8_t**)_networkSsid, WL_NETWORKS_LIST_MAXNUM);

    commDrv.commSlaveDeselect();

    return ssidListNum;
}

char* WiFiDrv::getSSIDNetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;

	return _networkSsid[networkItem];
}

uint8_t WiFiDrv::getEncTypeNetowrks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;

	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_IDX_ENCT_CMD, PARAM_NUMS_1);

    commDrv.sendParam(&networkItem, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t dataLen = 0;
    uint8_t encType = 0;
    commDrv.waitResponseCmd(GET_IDX_ENCT_CMD, PARAM_NUMS_1, (uint8_t*)&encType, &dataLen);

    commDrv.commSlaveDeselect();

    return encType;
}

int32_t WiFiDrv::getRSSINetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
		return NULL;
	int32_t	networkRssi = 0;

	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(GET_IDX_RSSI_CMD, PARAM_NUMS_1);
    commDrv.sendParam(&networkItem, 1, LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();

    // Wait for reply
    uint8_t dataLen = 0;
    commDrv.waitResponseCmd(GET_IDX_RSSI_CMD, PARAM_NUMS_1, (uint8_t*)&networkRssi, &dataLen);

    commDrv.commSlaveDeselect();

	return networkRssi;
}

uint8_t WiFiDrv::reqHostByName(const char* aHostname)
{
	WAIT_FOR_SLAVE_SELECT();

    // Send Command
    commDrv.sendCmd(REQ_HOST_BY_NAME_CMD, PARAM_NUMS_1);
    commDrv.sendParam((uint8_t*)aHostname, strlen(aHostname), LAST_PARAM);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(700);   //TODOb need a delay
    //delayMicroseconds(500);

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    uint8_t result = commDrv.waitResponseCmd(REQ_HOST_BY_NAME_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    commDrv.commSlaveDeselect();

    return result;
}

int WiFiDrv::getHostByName(IPAddress& aResult)
{
	uint8_t  _ipAddr[WL_IPV4_LENGTH];
	IPAddress dummy(0xFF,0xFF,0xFF,0xFF);
	int result = 0;

	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(GET_HOST_BY_NAME_CMD, PARAM_NUMS_0);

    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    //delay(1000);

    // Wait for reply
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(GET_HOST_BY_NAME_CMD, PARAM_NUMS_1, _ipAddr, &_dataLen))
    {
        WARN("error waitResponse");
    }else{
    	aResult = _ipAddr;
    	result = (aResult != dummy);
    }
    commDrv.commSlaveDeselect();
    return result;
}

int WiFiDrv::getHostByName(const char* aHostname, IPAddress& aResult)
{
	uint8_t retry = 10;
	if (reqHostByName(aHostname))
	{
		while(!getHostByName(aResult) && --retry > 0)
		{
			delay(1000);
		}
	}else{
		return 0;
	}
	return (retry>0);
}

char*  WiFiDrv::getFwVersion()
{
	  WAIT_FOR_SLAVE_SELECT();
    // Send Command
    commDrv.sendCmd(GET_FW_VERSION_CMD, PARAM_NUMS_0);
    //Wait the reply elaboration
    commDrv.waitForSlaveReady();
    // Wait for reply
    uint8_t _dataLen = 0;
    if (!commDrv.waitResponseCmd(GET_FW_VERSION_CMD, PARAM_NUMS_1, (uint8_t*)fwVersion, &_dataLen))
    {
        WARN("error waitResponse");
    }
    commDrv.commSlaveDeselect();
    return fwVersion;
}

WiFiDrv wiFiDrv;

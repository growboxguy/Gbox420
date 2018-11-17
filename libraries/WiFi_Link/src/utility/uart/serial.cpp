/*
  serial.cpp - Library for Arduino Wifi shield.
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
//*********************************************/
//
//  File:   serial.cpp
//
//  Author: bing@arduino.org (arduino srl)
//  edit: andrea@arduino.org (arduino srl)
//
//********************************************/

#if !defined(ESP_CH_SPI)

#include "serial.h"

unsigned long _startMillis;
unsigned long _timeout = 3000;  //3 Second Serial Timeout

void WfSerial::init(Stream *serial)
{
  espSerial = serial;
}

int WfSerial::read()
{
  int c;
  c = espSerial->read();
  return c;
}

void WfSerial::write(unsigned char c)
{
  espSerial->write(c);
}

int WfSerial::available()
{
  int num;
  num = espSerial->available();
  return num;
}

int WfSerial::timedRead()
{
  int c;
  _startMillis = millis();
  do {
    c = read();
    if (c >= 0) return c;
  } while(millis() - _startMillis < _timeout);
  return -1;     // -1 indicates timeout
}

String WfSerial::readStringUntil(char terminator)
{
	String ret;
	int c = timedRead();
	while (c >= 0 && (char)c != terminator)
	{
		ret += (char)c;
		c = timedRead();
	}
	return ret;
}

WfSerial wfSerial;

#endif

#pragma once

/*!
 *  \brief     Printing serial messages to the Arduino Serial output and the esp-link Microcontroller Console (uC Console)
 *  \details   Uses templating to handle logging multiple variable types (int,float,char,bool..)
 *  \details   Functions implemented in header file due to: https://stackoverflow.com/questions/10632251/undefined-reference-to-template-function
 */

#include "Arduino.h"

extern HardwareSerial &ArduinoSerial;
extern HardwareSerial &ESPSerial;

void logToSerials(const __FlashStringHelper *ToPrint, bool BreakLine = true, uint8_t Indent = 3); ///< logs to both Arduino and ESP Link serial console, 2 optional parameters to adding a break line at after printing and the indentation in front
template <class logLine>
void logToSerials(logLine *ToPrint, bool BreakLine = true, uint8_t Indent = 3)
{
  while (Indent > 0)
  {
    ArduinoSerial.print(F(" "));
    ESPSerial.print(F(" "));
    Indent--;
  }
  ArduinoSerial.print((*ToPrint));
  ESPSerial.print((*ToPrint));
  if (BreakLine)
  {
    ArduinoSerial.println();
    ESPSerial.println();
  }
}

template <class logLine>
void logToSerials(logLine &ToPrint, bool BreakLine = true, uint8_t Indent = 3)
{
  while (Indent > 0)
  {
    ArduinoSerial.print(F(" "));
    ESPSerial.print(F(" "));
    Indent--;
  }
  ArduinoSerial.print(ToPrint);
  ESPSerial.print(ToPrint);
  if (BreakLine)
  {
    ArduinoSerial.println();
    ESPSerial.println();
  }
}
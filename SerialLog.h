#pragma once

///This is a collection of common functions that every component can use
///Needs to be included in the module's header (.h) file:  #include "420Common.h"

#include "Arduino.h"           ///every inheriting class have Arduino commands available

///Forward declaration
extern HardwareSerial &ArduinoSerial;
extern HardwareSerial &ESPSerial;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Logging to Arduino Serial and ESP-link's Microcontroller Console (uC Console)
///Uses templating to handle logging multiple variable types (int,float,char,bool..)
///Functions implemented in header file due to: https:///stackoverflow.com/questions/10632251/undefined-reference-to-template-function

void logToSerials(const __FlashStringHelper *ToPrint, bool BreakLine = true, byte Indent = 3); ///logs to both Arduino and ESP Link serial console, 2 optional parameters to adding a break line at after printing and the indentation in front
template <class logLine>
void logToSerials(logLine *ToPrint, bool BreakLine = true, byte Indent = 3)
{
  while (Indent > 0)
  {
    ArduinoSerial.print(F(" "));
    ESPSerial.print(F(" "));
    Indent--;
  }
  if (BreakLine)
  {
    ArduinoSerial.println((*ToPrint));
    ESPSerial.println((*ToPrint));
  }
  else
  {
    ArduinoSerial.print((*ToPrint));
    ESPSerial.print((*ToPrint));
  }
}

template <class logLine>
void logToSerials(logLine &ToPrint, bool BreakLine = true, byte Indent = 3)
{
  while (Indent > 0)
  {
    ArduinoSerial.print(F(" "));
    ESPSerial.print(F(" "));
    Indent--;
  }
  if (BreakLine)
  {
    ArduinoSerial.println(ToPrint);
    ESPSerial.println(ToPrint);
  }
  else
  {
    ArduinoSerial.print(ToPrint);
    ESPSerial.print(ToPrint);
  }
}
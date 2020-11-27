#pragma once

/*! 
 *  \brief     Printing serial messages to the Arduino Serial output and the esp-link Microcontroller Console (uC Console)
 *  \details   Uses templating to handle logging multiple variable types (int,float,char,bool..)
 *  \details   Functions implemented in header file due to: https:///stackoverflow.com/questions/10632251/undefined-reference-to-template-function
 */

#include "Arduino.h" ///every inheriting class have Arduino commands available

//Forward declaration
extern HardwareSerial &ArduinoSerial;

void logToSerials(const __FlashStringHelper *ToPrint, bool BreakLine = true, byte Indent = 3); //logs to both Arduino and ESP Link serial console, 2 optional parameters to adding a break line at after printing and the indentation in front

template <class logLine>
void logToSerials(logLine *ToPrint, bool BreakLine = true, byte Indent = 3)
{
  while (Indent > 0)
  {
    ArduinoSerial.print(F(" "));
    Indent--;
  }
  if (BreakLine)
  {
    ArduinoSerial.println((*ToPrint));
  }
  else
  {
    ArduinoSerial.print((*ToPrint));
  }
}

template <class logLine>
void logToSerials(logLine &ToPrint, bool BreakLine = true, byte Indent = 3)
{
  while (Indent > 0)
  {
    ArduinoSerial.print(F(" "));
    Indent--;
  }
  if (BreakLine)
  {
    ArduinoSerial.println(ToPrint);
  }
  else
  {
    ArduinoSerial.print(ToPrint);
  }
}

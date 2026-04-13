#pragma once

/*!
 *  \brief     Printing serial messages to the Arduino Serial output and the esp-link Microcontroller Console (uC Console)
 *  \details   Uses templating to handle logging multiple variable types (int,float,char,bool..)
 *  \details   Functions implemented in header file due to: https://stackoverflow.com/questions/10632251/undefined-reference-to-template-function
 */

#include "Arduino.h"

extern HardwareSerial &ArduinoSerial;
extern HardwareSerial &ESPSerial;

/**
 * @brief Logs a Flash-resident string to both serial consoles.
 * @param ToPrint   Flash string (e.g., F("My Message")).
 * @param BreakLine Append a newline (default: true).
 * @param Indent    Leading spaces (default: 3).
 * @example logToSerials(F("System Initialized"), true, 0);
 */
void logToSerials(const __FlashStringHelper *ToPrint, bool BreakLine = true, uint8_t Indent = 3);
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

/**
 * @brief Logs generic data types (int, float, String) to both Arduino and ESP Link.
 * @tparam logLine  The type of the data being logged.
 * @param ToPrint   Reference to the data or variable to be logged.
 * @param BreakLine Append a newline after printing (default: true).
 * @param Indent    Number of leading spaces (default: 3).
 */
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
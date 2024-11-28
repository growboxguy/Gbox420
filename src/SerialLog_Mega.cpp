#include "SerialLog.h"

///< Logging

void logToSerials(const __FlashStringHelper *ToPrint, bool BreakLine, uint8_t Indent)
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
    ArduinoSerial.println(ToPrint);
    ESPSerial.println(ToPrint);
  }
}
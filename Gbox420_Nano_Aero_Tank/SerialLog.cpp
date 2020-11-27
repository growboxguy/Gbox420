#include "SerialLog.h"

//////////////////////////////////////////
//Logging

void logToSerials(const __FlashStringHelper *ToPrint, bool BreakLine, byte Indent)
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

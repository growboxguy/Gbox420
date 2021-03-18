#include "420Common.h"

Common::Common(const __FlashStringHelper *Name)
{
  this->Name = Name;
  logToSerials(F("Initializing"), false, 2);
  logToSerials(this->Name, false, 1);
}

/*
Common::Common()
{
  logToSerials(F("Initializing "), false, 2);
  logToSerials(this->Name, false, 0);
}
*/

void Common::report()
{
  logToSerials(Name, false, 4);
  logToSerials(F("-"), false, 1); ///< Prints "    COMPONENTNAME -" to the console
}

void Common::refresh_Sec()
{
  logToSerials(Name, false, 2);
  logToSerials(F("refreshing (1sec)"), true, 1); ///< Prints "COMPONENTNAME refreshing (1 sec)" to the console
}

void Common::refresh_FiveSec()
{
  logToSerials(Name, false, 2);
  logToSerials(F("refreshing (5sec)"), true, 1); ///< Prints "COMPONENTNAME refreshing (5 sec)" to the console
}

void Common::refresh_Minute()
{
  logToSerials(Name, false, 2);
  logToSerials(F("refreshing (1min)"), true, 1); ///< Prints "COMPONENTNAME refreshing (1 min)" to the console
}

char *Common::getComponentName(const __FlashStringHelper *ComponentName)
{
  static char ReturnChar[MaxWordLength] = "";
  strcpy_P(ReturnChar, (PGM_P)Name);
  strcat_P(ReturnChar, (PGM_P)F("_"));
  strcat_P(ReturnChar, (PGM_P)ComponentName);
  return ReturnChar;
}

void Common::reportToJSON()
{
  strcat_P(LongMessage, (PGM_P)F("\""));
  strcat_P(LongMessage, (PGM_P)Name);
  strcat_P(LongMessage, (PGM_P)F("\":{"));
}

bool Common::isThisMyComponent(char const *lookupName)
{ ///< When a web component triggers an action, this function decides if the component belonged to the class
  ///< lookupName is in the form of: InstanceName_FunctionName . Examles: Lt1_On , Lt1_OnTime, LtSen1_Raw

  ///< Serial.print("Component :");
  ///< Serial.println(lookupName);
  ///< Serial.print("Object :");
  ///< Serial.println(Name);

  char *ReturnChar = ShortMessage; ///< return text will be loaded into a global temp buffer
  uint8_t CharacterCount = 0;
  char FlashCurrentChar;
  char RAMCurrentChar;

  const char *FlashAddressPointer = (const char PROGMEM *)Name;
  while (1)
  {
    FlashCurrentChar = pgm_read_byte(FlashAddressPointer++); ///< read back from the memory address on character, and then increment the pointer to the next char
    RAMCurrentChar = lookupName[CharacterCount++];           ///<
    ///< Serial.print(FlashCurrentChar);
    ///< Serial.print(RAMCurrentChar);
    if (FlashCurrentChar == 0)
      break; ///< if we have reached the string termination sign. ( null terminator is the numerical value 0, sometimes also marked as '\0')
    if (FlashCurrentChar != RAMCurrentChar)
    {
      ///< Serial.println("Not match");
      return false; ///< stop the while loop
    }
  }
  if (FlashCurrentChar == 0 && RAMCurrentChar == '_')
  { ///< if instance name is confirmed: continue reading the HTML component name describing the function
    int SafetyCount = 0;
    ///< Serial.print("Inside second check: ");
    while (1)
    {
      RAMCurrentChar = lookupName[CharacterCount++]; ///<
      ///< Serial.print(RAMCurrentChar);
      *ReturnChar++ = RAMCurrentChar;
      if (RAMCurrentChar == 0)
        break; ///< if we have reached the string termination sign. ( null terminator is the numerical value 0, sometimes also marked as '\0')
      if (SafetyCount++ > MaxWordLength)
      {
        logToSerials(F("Name too long:"), false, 3);
        logToSerials(lookupName, true, 1);
        return false;
      }
    }
    ///< Serial.println("Match");
    logToSerials(F("Function:"), false, 1);
    logToSerials(&ShortMessage, true, 1);
    return true;
  }
  else
  {
    ///< Serial.println("Not match");
    return false;
  }
}

void Common::appendName(bool Clear)
{
  if (Clear)
  {
    memset(&ShortMessage[0], 0, sizeof(ShortMessage)); //reset variable to store the Publish to path
  }
  strcpy_P(ShortMessage, (PGM_P)Name);
  strcat_P(ShortMessage, (PGM_P)F(" "));
}
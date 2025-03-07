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
  //logToSerials(F("Initializing without name"), false, 2);
}
*/

/**
 * @brief Report current state in a JSON format to the LongMessage buffer - Append the start of the JSON
 */
void Common::report(__attribute__((unused)) bool FriendlyFormat)
{
  strcat_P(LongMessage, (PGM_P)F("\""));
  strcat_P(LongMessage, (PGM_P)Name);
  strcat_P(LongMessage, (PGM_P)F("\":{"));
}

void Common::refresh_Sec()
{
  if (Debug)
  {
    logToSerials(Name, false, 2);
    logToSerials(F("- 1sec"), true, 1); ///< Prints "COMPONENTNAME - 1sec" to the console
  }
}

void Common::refresh_FiveSec()
{
  if (Debug)
  {
    logToSerials(Name, false, 2);
    logToSerials(F("- 5sec"), true, 1); ///< Prints "COMPONENTNAME - 5sec" to the console
  }
}

void Common::refresh_Minute()
{
  if (Debug)
  {
    logToSerials(Name, false, 2);
    logToSerials(F("- 1min"), true, 1); ///< Prints "COMPONENTNAME - 1min" to the console
  }
}

bool Common::isThisMine(char const *lookupName) ///< Returns true when the lookupName starts with the Name of the instance followed by _
{
  ///< Serial.print("Component :");
  ///< Serial.println(lookupName);
  ///< Serial.print("Object :");
  ///< Serial.println(Name);

  char *ReturnChar = ShortMessage; ///< return text will be loaded into a global temp buffer
  uint8_t CharacterCount = 0;      // Tracks which character is currently getting compared
  char FlashCurrentChar;           // Character read back from the Flash storage (Name is stored in flash)
  char RAMCurrentChar;             // Character read back from the RAM (lookupName is stored in RAM)

  const char *FlashAddressPointer = (const char PROGMEM *)Name; ///< Get the flash storage address of the first character of Name
  while (1)
  {
    FlashCurrentChar = pgm_read_byte(FlashAddressPointer++); ///< read the current character from the flash and increment the pointer to the next char
    RAMCurrentChar = lookupName[CharacterCount++];           ///< read the character at the same position from the lookupName
    ///< Serial.print(FlashCurrentChar);
    ///< Serial.print(RAMCurrentChar);
    if (FlashCurrentChar == '\0')
      break; ///< reached the string termination sign
    if (FlashCurrentChar != RAMCurrentChar)
    {
      ///< Serial.println("No match");
      return false; ///< stop the while loop
    }
  }
  if (FlashCurrentChar == '\0' && RAMCurrentChar == '_') ///< End of the Name, _ sign at the lookupName
  {                                                      ///< if instance name is confirmed: continue reading the remaining characters from the lookupName
    int SafetyCount = 0;
    ///< Serial.print("Inside second check: ");
    while (1)
    {
      RAMCurrentChar = lookupName[CharacterCount++]; ///< read the next lookName character from RAM
      ///< Serial.print(RAMCurrentChar);
      *ReturnChar++ = RAMCurrentChar;
      if (RAMCurrentChar == '\0')
        break; ///< reached the string termination sign
      if (SafetyCount++ > MaxWordLength)
      {
        *ReturnChar = '\0';
        logToSerials(F("Too long:"), false, 3);
        logToSerials(lookupName, true, 1);
        return false;
      }
    }
    return true;
  }
  else
  {
    return false;
  }
}

void Common::appendName(bool ClearBuffer)
{
  if (ClearBuffer)
  {
    memset(&ShortMessage[0], 0, MaxShotTextLength); // blank out the ShortMessage global buffer
  }
  strcpy_P(ShortMessage, (PGM_P)Name);
  strcat_P(ShortMessage, (PGM_P)F(" "));
}

/**
  \brief Returns the name of the caller component
  \param AppendToEnd Appended after the Name
  \param UnderscoreSeparator true - Sparate the name and the AppendToEnd with an underscore, false (default): Use space
*/

char *Common::getName(const __FlashStringHelper *AppendToEnd, bool UnderscoreSeparator)
{
  static char ReturnChar[MaxWordLength] = "";
  strcpy_P(ReturnChar, (PGM_P)Name);
  if (UnderscoreSeparator)
    strcat_P(ReturnChar, (PGM_P)F("_"));
  else
    strcat_P(ReturnChar, (PGM_P)F(" "));
  strcat_P(ReturnChar, (PGM_P)AppendToEnd);
  return ReturnChar;
}

char *Common::getName(char *AppendToEnd, bool UnderscoreSeparator)
{
  static char ReturnChar[MaxWordLength] = "";
  strcpy_P(ReturnChar, (PGM_P)Name);
  if (UnderscoreSeparator)
    strcat_P(ReturnChar, (PGM_P)F("_"));
  else
    strcat_P(ReturnChar, (PGM_P)F(" "));
  strcat(ReturnChar, AppendToEnd);
  return ReturnChar;
}
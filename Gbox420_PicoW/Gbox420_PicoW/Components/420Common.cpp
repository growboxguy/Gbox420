#include "420Common.h"

Common::Common(const char *Name)
{
  this->Name = Name;
  printf("  Initializing ");
  printf(this->Name);
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer - Append the start of the JSON
 */
void Common::report(__attribute__((unused)) bool FriendlyFormat)
{
  strcat(LongMessage, "\"");
  strcat(LongMessage, Name);
  strcat(LongMessage, "\":{");
}

void Common::refresh()
{
  RefreshCounter++;
  if (*Debug)
  {
    printf("  %s refreshing\n", Name); ///< Prints "COMPONENTNAME refreshing" to the console
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

  /*
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
          printf("   Too long:");
          printf(lookupName, true, 1);
          return false;
        }
      }
      return true;
    }
    else
    */
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
  strcpy(ShortMessage, Name);
  strcat(ShortMessage, " ");
}

/**
  \brief Returns the name of the caller component
  \param AppendToEnd Appended after the Name
  \param UnderscoreSeparator true - Sparate the name and the AppendToEnd with an underscore, false (default): Use space
*/

char *Common::getName(char *AppendToEnd, bool UnderscoreSeparator)
{
  static char ReturnChar[MaxWordLength] = "";
  strcpy(ReturnChar, Name);
  if (UnderscoreSeparator)
    strcat(ReturnChar, "_");
  else
    strcat(ReturnChar, " ");
  strcat(ReturnChar, AppendToEnd);
  return ReturnChar;
}
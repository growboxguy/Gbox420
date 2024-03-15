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

void Common::run1sec()
{
  if (*Debug)
  {
    printf("  1sec - %s\n", Name);
  }
}

void Common::run5sec()
{
  if (*Debug)
  {
    printf("  5sec - %s\n", Name);
  }
}

void Common::run1min()
{
  if (*Debug)
  {
    printf("  1min - %s\n", Name);
  }
}

void Common::runAll()
{
 run1sec();
 run5sec();
 run1min();
}


bool Common::isThisMine(char const *LookupName) ///< Returns true when the LookupName starts with the Name of the instance followed by _
{
  printf("LookupName :");
  printf("%s\n", LookupName);
  printf("Object :");
  printf("%s\n", Name);

  char *Command = ShortMessage; ///< return text will be loaded into a global temp buffer
  uint8_t CharacterCount = 0;      // Tracks which character is currently getting compared
  char NameCurrentChar;            // Current Name character being compared
  char LookupCurrentChar;          // Current LookupName character being compared

  while (1)
  {
    NameCurrentChar = Name[CharacterCount];           ///< read the current character from the flash and increment the pointer to the next char
    LookupCurrentChar = LookupName[CharacterCount++]; ///< read the character at the same position from the LookupName
    // printf("%s - %s ",NameCurrentChar,LookupCurrentChar);

    if (NameCurrentChar == '\0')
      break; ///< reached the string termination sign, stop the while loop
    if (NameCurrentChar != LookupCurrentChar)
    {
      // printf(": No match\n");
      return false; ///< Stop function and return false
    }
  }
  if (NameCurrentChar == '\0' && LookupCurrentChar == '_') ///< End of the Name, _ sign at the LookupName
  {                                                        ///< if instance name is confirmed: continue reading the remaining characters from the LookupName
    int SafetyCount = 0;
    ///< printf("Inside second check: ");
    while (1)
    {
      LookupCurrentChar = LookupName[CharacterCount++]; ///< read the next LookupName character
      ///< printf(LookupCurrentChar);
      *Command++ = LookupCurrentChar;
      if (LookupCurrentChar == '\0')
        break; ///< reached the string termination sign
      if (SafetyCount++ > MaxWordLength)
      {
        *Command = '\0';
        printf("   Too long:");
        printf(LookupName, true, 1);
        return false;
      }
    }
    printf("Match on %s, command: %s\n",Name,ShortMessage);
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
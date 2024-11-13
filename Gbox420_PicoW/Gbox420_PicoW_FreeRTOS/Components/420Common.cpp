#include "420Common.h"

Common::Common(const char *Name)
{
  this->Name = Name;
  printf("Initializing %s...", Name);
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer - Append the start of the JSON
 */
void Common::report(bool FriendlyFormat)
{
  strcat(LongMessage, "\"");
  strcat(LongMessage, Name);
  strcat(LongMessage, "\":{");
}

void Common::run1sec()
{
  if (Debug)
  {
    printf(" 1sec - %s\n", Name);
  }
}

void Common::run5sec()
{
  if (Debug)
  {
    printf(" 5sec - %s\n", Name);
  }
}

void Common::run1min()
{
  if (Debug)
  {
    printf(" 1min - %s\n", Name);
  }
}

void Common::run30min()
{
  if (Debug)
  {
    printf(" 30min - %s\n", Name);
  }
}

void Common::runAll()
{
  run1sec();
  run5sec();
  run1min();
  run30min();
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

bool Common::isThisForMe(char const *Command)
{
  return ::isThisForMe((char const *)Name, Command);
}
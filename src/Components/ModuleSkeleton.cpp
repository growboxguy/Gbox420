#include "ModuleSkeleton.h"

ModuleSkeleton::ModuleSkeleton(const __FlashStringHelper *Name, Module *Parent, Settings::ModuleSkeletonSettings *DefaultSettings) : Common(Name)
{ ///< constructor
  this->Parent = Parent;
  PersistentBool = &DefaultSettings->PersistentBool;
  PersistentInt = &DefaultSettings->PersistentInt;
  PersistentFloat = &DefaultSettings->PersistentFloat;
  RollingInt = new RollingAverage();
  RollingFloat = new RollingAverage();
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToRefreshQueue_Minute(this);
  logToSerials(F("ModuleSkeleton object created"), true, 1);
}

void ModuleSkeleton::refresh_Sec()
{
  if (*Debug)
    Common::refresh_Sec();
}

void ModuleSkeleton::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  if (RuntimeBool)
  {
    RuntimeInt++;
    RuntimeFloat += 0.1;
  }
}

void ModuleSkeleton::refresh_Minute()
{
  if (*Debug)
    Common::refresh_Minute();
}

void ModuleSkeleton::report()
{ ///< Report status to the Arduino Serial and ESP-lin console output
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("PersistentBool:"));
  strcat(LongMessage, toText_yesNo(*PersistentBool));
  strcat_P(LongMessage, (PGM_P)F(" ; PersistentInt:"));
  strcat(LongMessage, toText(*PersistentInt));
  strcat_P(LongMessage, (PGM_P)F(" ; PersistentFloat:"));
  strcat(LongMessage, toText(*PersistentFloat));
  strcat_P(LongMessage, (PGM_P)F(" ; RuntimeBool:"));
  strcat(LongMessage, toText_yesNo(RuntimeBool));
  strcat_P(LongMessage, (PGM_P)F(" ; RuntimeInt:"));
  strcat(LongMessage, toText(RuntimeInt));
  strcat_P(LongMessage, (PGM_P)F(" ; RuntimeFloat:"));
  strcat(LongMessage, toText(RuntimeFloat));
  strcat_P(LongMessage, (PGM_P)F(" ; RuntimeString:"));
  strcat(LongMessage, RuntimeString);
  strcat_P(LongMessage, (PGM_P)F(" ; RollingInt:"));
  strcat(LongMessage, RollingInt->getIntText(true));
  strcat_P(LongMessage, (PGM_P)F(" ; RollingFloat:"));
  strcat(LongMessage, RollingFloat->getFloatText(true));
  logToSerials(&LongMessage, true, 1);
}
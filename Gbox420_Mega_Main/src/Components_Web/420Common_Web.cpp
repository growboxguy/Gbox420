#include "420Common_Web.h"

Common_Web::Common_Web(const __FlashStringHelper *Name) : Common(Name)
{
  this->Name = Name;
}

void Common_Web::reportToJSON()
{
    strcat_P(LongMessage, (PGM_P)F("\""));
    strcat_P(LongMessage, (PGM_P)Name);
    strcat_P(LongMessage, (PGM_P)F("\":{"));
}
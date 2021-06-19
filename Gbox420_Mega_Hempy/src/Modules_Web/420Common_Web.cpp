#include "420Common_Web.h"

/**
* @brief Constructor: creates an instance of the class, and stores the unique Name of the instance
*/
Common_Web::Common_Web(const __FlashStringHelper *Name) : Common(Name)
{
  this->Name = Name;
}
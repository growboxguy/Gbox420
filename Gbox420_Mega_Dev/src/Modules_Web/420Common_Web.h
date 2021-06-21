#pragma once ///< header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

///< These are functions extending the functions of *_Web components, each component must include this file

#include "ELClientWebServer.h" ///< ESP-link - WebServer API
#include "../Components/420Common.h"

///< forward declaration

class Common_Web : virtual public Common
{
public:
  virtual bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data){};

private:
protected:
  Common_Web(const __FlashStringHelper *Name); ///< Constructor
};
#pragma once
///< This is a template for creating new modules

///< #include "3rdPartyModlue.h" ///< Load any required 3rd party modules here
#include "420Common_Web.h"
#include "../Modules_Web/420Module_Web.h"
#include "../Components/ModuleSkeleton.h"

///< Example module with test data - website component

class ModuleSkeleton_Web : virtual public ModuleSkeleton, virtual public Common_Web
{
public:
  ModuleSkeleton_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::ModuleSkeletonSettings *DefaultSettings); ///< constructor
  void websiteEvent_Load(__attribute__((unused)) char *Url);
  void websiteEvent_Refresh(__attribute__((unused)) char *Url);
  bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data);

private:
protected:
  Module_Web *Parent;
};
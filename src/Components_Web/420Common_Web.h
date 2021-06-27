#pragma once ///< header guard, same as the ifndef/define guard  https://en.wikipedia.org/wiki/Pragma_once

/**
  \brief Extends the Component class with fuctions required to communicate with the ESP-link website 
  \attention Each Command needs to be unique 
  \details This class contains pure virtual functions forces implementing functions required to interact with the ESP-link website
*/

#include "ELClientWebServer.h" ///< ESP-link - WebServer API
#include "../Components/420Common.h"

///< forward declaration
extern ELClientWebServer WebServer;

class Common_Web : virtual public Common
{
public:
  virtual void websiteEvent_Load(__attribute__((unused)) char *url) {}; ///< Called when an ESP-link webpage is loading. The name of the page is passed in the URL attribute (example: /Settings.html)
  virtual void websiteEvent_Refresh(__attribute__((unused)) char *url) {}; ///< Called when an ESP-link webpage is refreshing (Automatic, every 5sec)
  virtual bool commandEvent(__attribute__((unused)) char *Command, __attribute__((unused)) char *Data) {return false;}; ///< Called when an ESP-link Button is clicked (Command: Name of the button, Data: empty), or a Field is submitted (Command: Name of the field, Data: The submitted value as text) 

private:
protected:
  inline Common_Web(const __FlashStringHelper *Name):Common(Name){} ///< Constructor, implemented in the header file https://stackoverflow.com/questions/4761834/defining-constructor-in-header-file-vs-implementation-cpp-file
};
// strcat_P(LongMessage,(PGM_P)F(" ; Internal fan:"));strcat(LongMessage,fanSpeedToText(true));
// strcat_P(LongMessage,(PGM_P)F(" ; Exhaust fan:"));strcat(LongMessage,fanSpeedToText(false)); 

#pragma once

#include "420Common.h"

class GrowBox;  //forward declaration

class Fan : public Common
{
  public:
    Fan(const __FlashStringHelper * Name, GrowBox * GBox, uint8_t onOffpin, uint8_t speedPin);
    GrowBox * GBox;

    void refresh();  //Called when component should refresh its state
    void report(){};
    void websiteLoadEvent(char * url){};
    void websiteRefreshEvent(char * url);
    void websiteButtonPressEvent(char * Button){};
    void websiteFieldSubmitEvent(char * Button){};

    void setOff();
    void setLowSpeed();  
    void setHighSpeed();
  
  private:
    bool * State;
    bool * HighSpeed;     
    byte onOffPin;
    byte speedPin;
};

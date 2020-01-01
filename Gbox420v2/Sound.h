#pragma once

#include "420Common.h"

class GrowBox;  //forward declaration

class Sound : public Common
{
public:
    Sound(const __FlashStringHelper * Name, GrowBox * GBox, Settings::SoundSettings * DefaultSettings);
    void websiteEvent_Load(__attribute__((unused)) char * url);
    //void websiteEvent_Refresh(__attribute__((unused)) char * url);  //{} means this function is empty, it has no implementation in the cpp file
    void websiteEvent_Field(char * Field);
    void websiteEvent_Button(char * Button);
    void refresh_Sec();
    void setSoundOnOff(bool State);
    void playOnSound();
    void playOffSound();
    void playEE();

private:
  GrowBox * GBox;
  byte* Pin; //Pin connecting the piezo Sound positive(+) side over a 330Ω resistor)
  bool* Enabled;   //Sound ON or OFF
  bool PlayOnSound = false; //Play on beep flag - website controls it
  bool PlayOffSound = false; //Play off beep flag - website controls it
  bool PlayEE = false; //Surprise :) - website controls it
  const PROGMEM static int melody[];
  const PROGMEM static byte tempo[];
  void buzz(uint32_t frequency, uint32_t length);
  void OnSound();
  void OffSound();
  void EE();      
protected:  
};

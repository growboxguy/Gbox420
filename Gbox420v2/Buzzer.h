#ifndef Buzzer_H
#define Buzzer_H

#include "GrowBox.h"
//#include "TimerThree.h"  //Interrupt handling for webpage

class Buzzer
{
  private:
    GrowBox * GBox;
    byte Pin; //Pin connecting the piezo buzzer positive(+) side over a 330Ω resistor)
    bool* Enabled;   //Sound ON or OFF
    bool PlayOnSound = false; //Play on beep flag - website controls it
    bool PlayOffSound = false; //Play off beep flag - website controls it
    bool PlayEE = false; //Surprise :) - website controls it
    void buzz(uint32_t frequency, uint32_t length);
    void OnSound();
    void OffSound();
    void EE();
    const PROGMEM static int melody[];
    const PROGMEM static byte tempo[];
      
  protected:
  
  public:
    Buzzer(GrowBox * GBox,byte Pin, bool *Enabled);
    void refresh();
    void setSoundOnOff(bool State);
    void playOnSound();
    void playOffSound();
    void playEE();
};
#endif

#pragma once

#include "420Common.h"
#include "420Module.h"

class Sound : virtual public Common
{
public:
  Sound(const __FlashStringHelper *Name, Module *Parent, Settings::SoundSettings *DefaultSettings);  
  void refresh_Sec();
  void playOnSound();
  void playOffSound();
  void playEE();

private:
 
protected:
  Module *Parent;
  void setSoundOnOff(bool State);
  void OnSound();
  void OffSound();
  void buzz(uint32_t frequency, uint32_t length);
  void EE();
  byte *Pin;                 //Pin connecting the piezo Sound positive(+) side over a 330Ω resistor)
  bool *Enabled;             //Sound ON or OFF
  bool PlayOnSound = false;  //Play on beep flag - website controls it
  bool PlayOffSound = false; //Play off beep flag - website controls it
  bool PlayEE = false;       //Surprise :) - website controls it
  const PROGMEM static int melody[];
  const PROGMEM static byte tempo[];
};

//WEBSITE COMPONENT
/*
<form><label><input type="checkbox" name="Sound1_Enabled">Sound enabled </label><input type="submit" value="Set"></form>
<button style="height:32px;width:32px;border-radius:0;"  id="Sound1_Ee" type="button"><img style="padding:0px;height:28px;width:28px;" src="https://lh3.googleusercontent.com/GAoRKaB09BobBln1BQgRHL0qmbfyTmnMn6_6JBmCVZ3gvUuCOzhRep7HkBYL4_bWqa5Vk70W7dgq_t5vgwisNYon5yXiM0hbBoUauYdnv9ujxtkVrn5hB4SXhT2X-nWb7BS4Bo_-fL-Pj2TLdzMcbJN4597KkEvYGrqv6CtPy_wf06wc9t1WN2jL5lbhWHdvkleKIU6Hpp1AXPjOI_3c9lY0jpIdEtcjbOZ3fYIgrEvm1HO7JrAbyllYIgpSpZlMr6uUQh2h6PuA9tDSb8-V3NmMuvRtOQi5qfH6rJKrOBv5oNN4_t-C-V3BLKjXqx82oAmP65RAnH1Y3EXG8FbuEsQvqYcWiEp9iaNeuYbjokQoVM8h61Gh1cPr717j0D1GjEYhYzba32UvVZ3iXiVr1zmonOw810HP5oTWhc-o0mNdW211dhs80tvOyaggxKGX_I7r5kcak9nK7PSP3ppAi-wjrsqhW0UBiRvsyHwpHRfIu-A0lbnQ4ukwWiw28dnBPQMZUu8rLAXVNRnBjoAPU5udjLwyCUyCeU6YjUlC9TTe7QpbTZCrVueTaytD3GOqtP2dKg2wZse3CCGPrnNq2vg5DpRV4Qb0sOBv8oyJzKfdWLCMgYJYTw9rpYg6Y3zgUjJWqmLdX3upj8an069FddUa33Sgb6YhQe6NSz1sdW7cq_-8zYosD0RmeavL4t3sfpNDXw95OOmQJ0LEFKRZSa3PBApRvl2GIOpNmLI2Li_QSw-y=s671-no" /></button>							
*/
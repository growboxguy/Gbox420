#pragma once

#include "ELClientRest.h" //ESP-link - REST API
#include "../Components/Sound.h"

class GrowBox; //forward declaration

class Sound_Web : public Sound
{
public:
  Sound_Web(const __FlashStringHelper *Name, Module *Parent, Settings::Sound_WebSettings *DefaultSettings);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Field(char *Field);
  void websiteEvent_Button(char *Button); 

private:
  GrowBox *GBox; 

protected:
};

//WEBSITE COMPONENT
/*
<form><label><input type="checkbox" name="Sound_Web1_Enabled">Sound_Web enabled </label><input type="submit" value="Set"></form>
<button style="height:32px;width:32px;border-radius:0;"  id="Sound_Web1_Ee" type="button"><img style="padding:0px;height:28px;width:28px;" src="https://lh3.googleusercontent.com/GAoRKaB09BobBln1BQgRHL0qmbfyTmnMn6_6JBmCVZ3gvUuCOzhRep7HkBYL4_bWqa5Vk70W7dgq_t5vgwisNYon5yXiM0hbBoUauYdnv9ujxtkVrn5hB4SXhT2X-nWb7BS4Bo_-fL-Pj2TLdzMcbJN4597KkEvYGrqv6CtPy_wf06wc9t1WN2jL5lbhWHdvkleKIU6Hpp1AXPjOI_3c9lY0jpIdEtcjbOZ3fYIgrEvm1HO7JrAbyllYIgpSpZlMr6uUQh2h6PuA9tDSb8-V3NmMuvRtOQi5qfH6rJKrOBv5oNN4_t-C-V3BLKjXqx82oAmP65RAnH1Y3EXG8FbuEsQvqYcWiEp9iaNeuYbjokQoVM8h61Gh1cPr717j0D1GjEYhYzba32UvVZ3iXiVr1zmonOw810HP5oTWhc-o0mNdW211dhs80tvOyaggxKGX_I7r5kcak9nK7PSP3ppAi-wjrsqhW0UBiRvsyHwpHRfIu-A0lbnQ4ukwWiw28dnBPQMZUu8rLAXVNRnBjoAPU5udjLwyCUyCeU6YjUlC9TTe7QpbTZCrVueTaytD3GOqtP2dKg2wZse3CCGPrnNq2vg5DpRV4Qb0sOBv8oyJzKfdWLCMgYJYTw9rpYg6Y3zgUjJWqmLdX3upj8an069FddUa33Sgb6YhQe6NSz1sdW7cq_-8zYosD0RmeavL4t3sfpNDXw95OOmQJ0LEFKRZSa3PBApRvl2GIOpNmLI2Li_QSw-y=s671-no" /></button>							
*/
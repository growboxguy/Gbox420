#ifndef LightSensor_H
#define LightSensor_H

#include "Common.h"

class LightSensor : public Common
{
  protected:

  public:
  
  private:  
    void checkLightSensor();
    bool PreviousLightRead = true;    
    int LightReading;  //light sensor analog reading
    bool Bright;  //Ligth sensor digital feedback: True-Bright / False-Dark 
    int LightsTriggerCount = 0;  //Counters of out of range readings before triggering an alert
    bool LightOK = true; //Track component health, at startup assume every component is OK
    int MaxLightReading = 0; // stores the highest light sensor analog reading
    int MinLightReading = 1023; //stores the lowest light sensor analog reading   

    void Lights::checkLightSensor(){
      Bright = !digitalRead(LightSensorInPin);     // read the input pin: 0- light detected , 1 - no light detected. Had to invert signal from the sensor to make more sense.
      LightReading = 1023 - analogRead(LightSensorAnalogInPin);
      
      if((MySettings.LightStatus && Bright) || (!MySettings.LightStatus && !Bright)){ //All OK: lights on&bright OR lights off&dark
        if(PreviousLightRead != false){LightsTriggerCount = 0;}
        else{ if(!LightOK)LightsTriggerCount++; } 
        PreviousLightRead = true;
        
        if(!LightOK && LightsTriggerCount>=MySettings.ReadCountBeforeAlert) {
          //sendEmailAlert(F("Lights%20OK")); 
          LightOK = true;
        }
      }
      else{
        if(PreviousLightRead != true){LightsTriggerCount = 0;}
        else{ if(LightOK)LightsTriggerCount++; } 
        PreviousLightRead = false;
        
        if(LightOK && LightsTriggerCount>=MySettings.ReadCountBeforeAlert){
           LightOK = false;
           if(MySettings.LightStatus && !Bright){ //if light should be ON but no light is detected
            //sendEmailAlert(F("No%20light%20detected"));        
            addToLog(F("Lights ON, no light detected"));
           }
           if(!MySettings.LightStatus && Bright){ //if light should be OFF but light is detected
            //sendEmailAlert(F("Dark%20period%20interrupted")); 
            addToLog(F("Dark period interrupted"));
            }
         }
       }  
    }

};

#endif

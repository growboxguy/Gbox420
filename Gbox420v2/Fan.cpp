#include "Fan.h"
#include "GrowBox.h"

// strcat_P(LongMessage,(PGM_P)F(" ; Internal fan:"));strcat(LongMessage,fanSpeedToText(true));
// strcat_P(LongMessage,(PGM_P)F(" ; Exhaust fan:"));strcat(LongMessage,fanSpeedToText(false)); 

Fan::Fan(const __FlashStringHelper * Name, GrowBox * GBox, uint8_t onOffPin, uint8_t speedPin) : Common(Name){
  this -> GBox = GBox;
  this -> onOffPin = onOffPin;
  this -> speedPin = speedPin;
  logToSerials(F("Fan object created"),true);
}

void Fan::refresh(){
  Common::refresh();
 /*  if(MySettings.InternalFanOn) digitalWrite(Relay4OutPin, LOW); else digitalWrite(Relay4OutPin, HIGH);
  if(MySettings.InternalFanHigh) digitalWrite(Relay5OutPin, LOW); else digitalWrite(Relay5OutPin, HIGH);
  if(MySettings.ExhaustFanOn) digitalWrite(Relay6OutPin, LOW); else digitalWrite(Relay6OutPin, HIGH);
  if(MySettings.ExhaustFanHigh) digitalWrite(Relay7OutPin, LOW); else digitalWrite(Relay7OutPin, HIGH);
  */
}
#include "Fan.h"
#include "GrowBox.h"

// strcat_P(LongMessage,(PGM_P)F(" ; Internal fan:"));strcat(LongMessage,fanSpeedToText(true));
// strcat_P(LongMessage,(PGM_P)F(" ; Exhaust fan:"));strcat(LongMessage,fanSpeedToText(false)); 
  //if (strcmp(url,"/GrowBox.html.json")==0){
  // WebServer.setArgBoolean(F("AutoInternalFan"),GBox -> BoxSettings -> AutomaticInternalFan);
  // WebServer.setArgString(F("InternalFanSwitchTemp"), toText(GBox -> BoxSettings -> InternalFanSwitchTemp));
  // WebServer.setArgBoolean(F("AutoExhaustFan"), GBox -> BoxSettings -> AutomaticExhaustFan);
  // WebServer.setArgString(F("ExhaustFanHighHumid"), toText(GBox -> BoxSettings -> ExhaustFanHighHumid));
  // WebServer.setArgString(F("ExhaustFanLowHumid"), toText(GBox -> BoxSettings -> ExhaustFanLowHumid));
  // WebServer.setArgString(F("ExhaustFanOffHumid"), toText(GBox -> BoxSettings -> ExhaustFanOffHumid));

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
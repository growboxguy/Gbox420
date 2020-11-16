#include "Switch.h"

Switch::Switch(const __FlashStringHelper *Name, uint8_t Pin, bool NegativeLogic) : Common(Name)
{
  this->Pin = Pin;
  this->NegativeLogic = NegativeLogic;
  pinMode(Pin, OUTPUT);
  turnOff();  ///initialize in OFF state  
  logToSerials(F("Switch object created"), true, 2);
}

Switch::Switch(const __FlashStringHelper *Name) : Common(Name)
{  
}

void Switch::flip(){
  if(State) turnOff();
  else turnOn();
}

void Switch::turnOn(){
 State = true;
 
 if(NegativeLogic){
    digitalWrite(Pin, LOW);   ///with negative logic switches
  }
  else{
    digitalWrite(Pin, HIGH);    ///with positive logic switches
  }
}

void Switch::turnOff(){
 State = false;

 if(NegativeLogic){
    digitalWrite(Pin, HIGH);   ///with negative logic switches
  }
  else{
    digitalWrite(Pin, LOW);  ///with positive logic switches
  }
}

bool Switch::getState(){
  return State;
}

char * Switch::getStateText(){
  return toText_onOff(State);
}
#include "420Common.h"

Common::Common(const __FlashStringHelper * Name){
    this -> Name = Name;
    logToSerials(F("Initializing "),false,2);logToSerials(this ->Name,false,0);logToSerials(F("..."),false,0);
}

void Common::refresh(){
  logToSerials(Name,false,2);logToSerials(F(" refreshing"),true,0);  
}

char * Common::getWebsiteComponentName(const __FlashStringHelper * ComponentName){
    static char ReturnChar[MaxTextLength] = ""; 
    strcpy_P(ReturnChar,(PGM_P)Name);
    strcat_P(ReturnChar,(PGM_P)F("_"));
    strcat_P(ReturnChar,(PGM_P)ComponentName);     
    return ReturnChar;
}

bool Common::isThisMyComponent(char const * lookupName){  //this is the name of the web component that triggered the action, in the form of: InstanceName_functionName . Examles: Light1_On , Light1_OnTime, LightSensor1_ReadingRaw from GrowBox.html 
  char * ReturnChar = ShortMessage; //return text will be loaded into a global temp buffer
  byte CharacterCount = 0;
  char FlashCurrentChar;
  char RAMCurrentChar;

  const char * FlashAddressPointer = (const char PROGMEM *)Name;
  while (1) {
    FlashCurrentChar = pgm_read_byte(FlashAddressPointer++);  //read back from the memory address on character, and then increment the pointer to the next char
    RAMCurrentChar = lookupName[CharacterCount++];  //
    if (FlashCurrentChar == 0) break;  //if we have reached the string termination sign. ( null terminator is the numerical value 0, sometimes also marked as '\0')   
    if(FlashCurrentChar!=RAMCurrentChar){
      return false; //stop the while loop
    }
}
  if(FlashCurrentChar == 0 && RAMCurrentChar == '_'){  //if instance name is confirmed: continue reading the HTML component name describing its function
    int SafetyCount = 0;
    while (1) {
      RAMCurrentChar = lookupName[CharacterCount++];  //      
      Serial.print(RAMCurrentChar);
      *ReturnChar++ = RAMCurrentChar;
      if(RAMCurrentChar == 0) break;  //if we have reached the string termination sign. ( null terminator is the numerical value 0, sometimes also marked as '\0')   
      if(SafetyCount > MaxTextLength){
       return false; 
      }      
    }
    return true;    
  }
  else{  
    return false;
  }  
}
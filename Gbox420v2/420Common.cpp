#include "420Common.h"

Common::Common(const __FlashStringHelper * Name){
    this -> Name = Name;
    logToSerials(F("Initializing "),false,2);logToSerials(this ->Name,false,0);logToSerials(F("..."),false,0);
}

void Common::refresh(){
  logToSerials(Name,false);logToSerials(F(" refreshing"),true,0);  
}

char * Common::getWebsiteComponentName(const __FlashStringHelper * ComponentName){
    static char ReturnChar[MaxTextLength] = ""; 
    strcpy_P(ReturnChar,(PGM_P)Name);
    strcat_P(ReturnChar,(PGM_P)F("_"));
    strcat_P(ReturnChar,(PGM_P)ComponentName);     
    return ReturnChar;
}


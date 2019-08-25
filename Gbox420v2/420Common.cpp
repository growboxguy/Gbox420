#include "420Common.h"

Common::Common(const __FlashStringHelper * Name){
    this -> Name = Name;
    logToSerials(this ->Name,false,0);logToSerials(F(" at your service!"),true,0);
}

char * Common::getWebsiteComponentName(const __FlashStringHelper * ComponentName){
    static char ReturnChar[MaxTextLength] = ""; 
    strcpy_P(ReturnChar,(PGM_P)Name);
    strcat_P(ReturnChar,(PGM_P)F("_"));
    strcat_P(ReturnChar,(PGM_P)ComponentName);     
    return ReturnChar;
}


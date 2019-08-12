#ifndef Helpers_H
#define Helpers_H

#include "Arduino.h"  //every inheriting class have Arduino commands available
#include "Settings.h"  //for storing/reading defaults

extern Settings MySettings;

template <class logLine>
static void logToSerials (const logLine& ToPrint,bool BreakLine) { 
  if(BreakLine){Serial.println(ToPrint);Serial3.println(ToPrint);}
      else{Serial.print(ToPrint);Serial3.print(ToPrint);}
}

static void saveSettings(bool LogThis){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)&MySettings, (void*)0, sizeof(MySettings)); //update_block only writes the bytes that changed
  //if(LogThis) Common::addToLog(F("Settings saved to EEPROM"));
  }
  
static void loadSettings(){
    Settings EEPROMSettings; //temporary storage with "Settings" type
    eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));  
    if(EEPROMSettings.Version != MySettings.Version){
      logToSerials(F("Change detected, updating EEPROM..."),false);
      saveSettings(false);  //overwrites stored settings with defaults from this sketch
    }
    else {
      logToSerials(F("Same structure version detected, applying restored settings..."),false);
      MySettings = EEPROMSettings; //overwrite sketch defaults with loaded settings
    }
    logToSerials(F("done"),true);
  }

#endif

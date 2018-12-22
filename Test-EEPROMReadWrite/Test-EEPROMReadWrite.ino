//GrowBoxGuy - http://sites.google.com/site/growboxguy/arduino
//Sketch for testing: EEPROM - Writing and reading back values

//Libraries
#include "avr/eeprom.h"

//Global variables - test values to write and read back
struct TestValues //define how the stucture looks like
{byte TestInt = 420; //When StructureVersion is changed these numbers get stored in EEPROM, else EEPROM content is loaded
float TestFloat = 4.20;
bool TestBool = true;
char TestText[21]= "Gbox420";
byte StructureVersion= 5; //increment this when you update the default test values or stucture to invalidate the EEPROM stored settings, do not update this value inside the loop

};
typedef struct TestValues settings;  //create the "settings" type using the stucture
settings MySettings;  //create a variable of type "settings"  with TestValues

void setup() {
  Serial.begin(115200);
  settings EEPROMSettings; //temporary storage with same "settings" type
  Serial.print(F("Test values from sketch - Ver."));Serial.println(MySettings.StructureVersion);
  printSettings(MySettings);
  
  eeprom_read_block((void*)&EEPROMSettings, (void*)0, sizeof(EEPROMSettings));
  Serial.print(F("Restored values - Ver."));Serial.println(EEPROMSettings.StructureVersion);
  printSettings(EEPROMSettings); 
  
  if(EEPROMSettings.StructureVersion != MySettings.StructureVersion){
    Serial.println(F("Change detected, updating EEPROM"));
    saveSettings();
  }
  else {
    Serial.println(F("Same structure version detected, applying restored settings"));
    MySettings = EEPROMSettings;
  }
  Serial.println();
}

void loop(void) {
  Serial.println("Loaded to memory:");
  printSettings(MySettings);
  Serial.println(F("Changing setting in memory"));
  MySettings.TestInt = 421;
  MySettings.TestFloat = 4.21;
  MySettings.TestBool = false;
  memset(&MySettings.TestText[0], 0, sizeof(MySettings.TestText));  //clear variable
  strcat(MySettings.TestText,"GrowBoxGuy"); //copy new text into variable
  printSettings(MySettings);
  Serial.println(F("Restoring setting from EEPROM"));
  eeprom_read_block((void*)&MySettings, (void*)0, sizeof(MySettings));
  printSettings(MySettings);  
  delay(600000);  //repeat every 10 minutes. Only read happens that does not decrease the EEPROM lifetime 
}
void printSettings(settings ToPrint){
  Serial.print(F("\t"));Serial.println(ToPrint.TestInt);
  Serial.print(F("\t"));Serial.println(ToPrint.TestFloat);
  Serial.print(F("\t"));Serial.println(ToPrint.TestBool);
  Serial.print(F("\t"));Serial.println(ToPrint.TestText);
  Serial.println();
}

void saveSettings(){ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void*)&MySettings, (void*)0, sizeof(MySettings));
}

//GrowBoxGuy - http://sites.google.com/site/growboxguy/arduino
//Sketch for testing: EEPROM - Writing and reading back values

//Libraries
#include "avr/eeprom.h"

byte Version = 6;        //increment this when you update the test values or stucture to invalidate the EEPROM stored settings
struct SettingsStructure //when Version is changed these values get stored in EEPROM, else EEPROM content is loaded
{
  int TestInt = 420;
  float TestFloat = 4.20;
  bool TestBool = true;
  char TestText[21] = "Gbox420";   //max 20 chars plus null terminator
  byte StructureVersion = Version; //do not update this value inside the loop
};
typedef struct SettingsStructure settings; //create the "settings" type using the stucture
settings MySettings;                       //create a variable of type "settings"  with default values from SettingsStructure

void setup()
{
  Serial.begin(115200);
  loadSettings();
}

void loop(void)
{
  Serial.println("Loaded to memory:");
  printSettings(MySettings);
  Serial.println(F("Changing setting in memory"));
  MySettings.TestInt = 421;
  MySettings.TestFloat = 4.21;
  MySettings.TestBool = false;
  memset(&MySettings.TestText[0], 0, sizeof(MySettings.TestText)); //clear variable
  strcat(MySettings.TestText, "GrowBoxGuy");                       //copy new text into variable
  printSettings(MySettings);
  Serial.println(F("Restoring setting from EEPROM"));
  eeprom_read_block((void *)&MySettings, (void *)0, sizeof(MySettings));
  printSettings(MySettings);
  delay(600000); //repeat every 10 minutes. Only read happens that does not decrease the EEPROM lifetime
}
void printSettings(settings ToPrint)
{
  Serial.print(F("\t"));
  Serial.println(ToPrint.TestInt);
  Serial.print(F("\t"));
  Serial.println(ToPrint.TestFloat);
  Serial.print(F("\t"));
  Serial.println(ToPrint.TestBool);
  Serial.print(F("\t"));
  Serial.println(ToPrint.TestText);
  Serial.println();
}

void loadSettings()
{
  settings EEPROMSettings; //temporary storage with same "settings" type
  Serial.print(F("Test values from sketch - Ver."));
  Serial.println(MySettings.StructureVersion);
  printSettings(MySettings);

  eeprom_read_block((void *)&EEPROMSettings, (void *)0, sizeof(EEPROMSettings));
  Serial.print(F("Restored values - Ver."));
  Serial.println(EEPROMSettings.StructureVersion);
  printSettings(EEPROMSettings);

  if (EEPROMSettings.StructureVersion != MySettings.StructureVersion)
  {
    Serial.print(F("Change detected, updating EEPROM..."));
    saveSettings();
  }
  else
  {
    Serial.print(F("Same structure version detected, applying restored settings..."));
    MySettings = EEPROMSettings;
  }
  Serial.println("done");
  Serial.println();
}

void saveSettings()
{ //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
  eeprom_update_block((const void *)&MySettings, (void *)0, sizeof(MySettings));
}

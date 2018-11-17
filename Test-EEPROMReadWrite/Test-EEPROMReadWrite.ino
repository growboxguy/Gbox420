//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Writing and reading back values from SD card

//Libraries
#include "avr/eeprom.h"

//Global variables - test values to write and read back
struct Settings //define how the stucture looks like
{
int TestInt = 420;
float TestFloat = 4.20;
bool TestBool = true;
};
typedef struct Settings settings;  //create the settings type using the stucture
settings MySettings;  //create a variable of "setting" type with "Setting structure"

void setup() {
  Serial.begin(115200);
  Serial.println(MySettings.TestInt);
  Serial.println(MySettings.TestFloat);
  Serial.println(MySettings.TestBool);
  Serial.println(F("Saving setting to EEPROM"));
  eeprom_update_block((const void*)&MySettings, (void*)0, sizeof(MySettings));  //do not put this in the loop, EEPROM has a write limit of 100.000 cycles
}

void loop(void) {
  Serial.println(F("Changing setting in memory"));
  MySettings.TestInt = 421;
  MySettings.TestFloat = 4.21;
  MySettings.TestBool = false;
  Serial.println(MySettings.TestInt);
  Serial.println(MySettings.TestFloat);
  Serial.println(MySettings.TestBool);
  
  Serial.println(F("Reading setting back from EEPROM"));
  eeprom_read_block((void*)&MySettings, (void*)0, sizeof(MySettings));
  Serial.println(MySettings.TestInt);
  Serial.println(MySettings.TestFloat);
  Serial.println(MySettings.TestBool);
  delay(60000);
}







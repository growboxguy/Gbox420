#pragma once

#include "420Common.h"
#include "420Module.h"

class WaterLevelSensor : public Common
{
public:
  WaterLevelSensor(const __FlashStringHelper *Name, Module *Parent, Settings::WaterLevelSensorSettings *DefaultSettings); //constructor
  void refresh_Minute();
  void report();
  int8_t getLevel();
  char *getLevelText();
  char *getLevelGauge();

private:
  byte *Pin_1;   //Lowest water level
  byte *Pin_2;
  byte *Pin_3;
  byte *Pin_4;
  bool isAboveSensor1; //Lowest water sensor, true if level reached. Below this level the reservoir should be refilled
  bool isAboveSensor2;
  bool isAboveSensor3;
  bool isAboveSensor4; //Highest water sensor. When level is reached the reservoir is full.
  char LevelGauge[9];  //Displays water level: E[____]F, E[#___]F, E[##__]F, E[###_]F, E[####]F
  int8_t Level;        //Stores water level in a number: 0 - Empty ... 4 - Full

protected:
  Module *Parent;
};
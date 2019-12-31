#pragma once

#define PZEM004_NO_SWSERIAL  //Singals to the PZEM004Tv30 module to use HardwareSerial instead of SoftwareSerial
#include "PZEM004Tv30.h" //3rd party module for the PZEM004T V3.0 power sensor
#include "420Common.h"

class GrowBox;  //forward declaration

class PowerSensor : public Common
{  
  public:
    PowerSensor(const __FlashStringHelper * Name, GrowBox * Gbox,HardwareSerial * SerialPort);  //constructor  
    float Power; //Power sensor - W
    float Energy; //Power sensor - Wh Total consumption 
    float Voltage; //Power sensor - V
    float Current; //Power sensor - A
    float Frequency;
    float PowerFactor;
    int Alarms;  
    //void websiteEvent_Load(__attribute__((unused)) char * url); //{} means this function is empty, it has no implementation in the cpp file
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    //void websiteEvent_Button(char * Button); //{} means this function is empty, it has no implementation in the cpp file
    //void websiteEvent_Field(char * Field); //{} means this function is empty, it has no implementation in the cpp file
    void refresh();  //Called when component should refresh its state
    void report();
    bool setPowerAlarm(int watts); //trigger alarm if max power consumption is exceeded
    bool getPowerAlarm();
    bool setAddress(uint8_t addr);
    uint8_t getAddress();
    bool resetEnergy();
    char * getPowerText(bool IncludeUnits);
    char * getEnergyText(bool IncludeUnits);   
    char * getVoltageText(bool IncludeUnits);
    char * getCurrentText(bool IncludeUnits);    
    char * getFrequencyText(bool IncludeUnits);
    char * getPowerFactorText();

  private:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    PZEM004Tv30 * Sensor; //for PZEM004T V3.0 model

  protected:
};

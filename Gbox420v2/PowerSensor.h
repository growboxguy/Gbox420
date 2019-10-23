#pragma once

#define PZEM004_NO_SWSERIAL  //Singal not to load SoftwareSerial module
#include "PZEM004Tv30.h" //for PZEM004T V3.0 model
#include "420Common.h"

class GrowBox;  //forward declaration

class PowerSensor : public Common
{
  protected:
    GrowBox * GBox; //Pointer to the GrowBox object that contains the Lights object
    PZEM004Tv30 * Sensor; //for PZEM004T V3.0 model
    
  public:
    PowerSensor(const __FlashStringHelper * Name, GrowBox * Gbox,HardwareSerial * SerialPort);  //constructor    
    void refresh();  //Called when component should refresh its state
    void report();
    void websiteLoadEvent(char * url){};
    void websiteRefreshEvent(char * url);
    void websiteFieldSubmitEvent(char * Button){};
    void websiteButtonPressEvent(char * Button){};

    bool setPowerAlarm(int watts); //trigger alarm if max power consumption is exceeded
    bool getPowerAlarm();
    bool setAddress(uint8_t addr);
    uint8_t getAddress();
    bool resetEnergy();
    float getPower();    
    float getEnergy();
    float getVoltage();
    float getCurrent();
    float getFrequency();
    float getPowerFactor();
    char * getPowerText();
    char * getEnergyText();    
    char * getVoltageText();
    char * getCurrentText();    
    char * getFrequencyText();
    char * getPowerFactorText();
        
  private:  
    float Power; //Power sensor - W
    float Energy; //Power sensor - Wh Total consumption 
    float Voltage; //Power sensor - V
    float Current; //Power sensor - A
    float Frequency;
    float PowerFactor;
    int Alarms;
};

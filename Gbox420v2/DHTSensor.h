#pragma once

#include "DHT.h"  //DHT11 or DHT22 Digital Humidity and Temperature sensor
#include "420Common.h"

class GrowBox;  //forward declaration

class DHTSensor : public Common
{
  public:
    DHTSensor(const __FlashStringHelper * Name, GrowBox * GBox, uint8_t _pin, uint8_t _sensorType);
    GrowBox * GBox;
    void refresh();  //Called when component should refresh its state
    void report();
    void websiteLoadEvent();
    void websiteRefreshEvent();
    float getTemp();  
    float getHumidity();
    char* getTempText();  
    char* getHumidityText();
  
  private:
    RollingAverage * Temp;
    RollingAverage * Humidity;
    DHT *sensor; //Pointer declaration, points to null initially 
    byte pin;
};

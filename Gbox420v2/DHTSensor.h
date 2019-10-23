#pragma once

#include "DHT.h"  //DHT11 or DHT22 Digital Humidity and Temperature sensor
#include "420Common.h"

class GrowBox;  //forward declaration

class DHTSensor : public Common
{
  public:
    DHTSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::DHTSensorSettings * DefaultSettings, uint8_t sensorType);
    GrowBox * GBox;

    void refresh();  //Called when component should refresh its state
    void report();
    void websiteLoadEvent(char * url){};
    void websiteRefreshEvent(char * url);
    void websiteButtonPressEvent(char * Button){};
    void websiteFieldSubmitEvent(char * Button){};

    float getTemp();  
    float getHumidity();
    char* getTempText();  
    char* getHumidityText();
  
  private:
    RollingAverage * Temp;
    RollingAverage * Humidity;
    DHT* Sensor; //Pointer declaration, points to null initially 
};

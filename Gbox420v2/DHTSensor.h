#ifndef DHTSensor_H
#define DHTSensor_H

#include "Common.h"
#include "DHT.h"  //DHT11 or DHT22 Digital Humidity and Temperature sensor

class DHTSensor : public Common
{
  public:
    DHTSensor(uint8_t _pin, uint8_t _sensorType);
    void refresh();  //Called when component should refresh its state
    float getTemp();  
    float getHumidity();
    char* getTempText();  
    char* getHumidityText();

  
  private:
    RollingAverage *Temp;
    RollingAverage *Humidity;
    DHT *sensor; //Pointer declaration, points to null initially 
    byte pin;
};

#endif

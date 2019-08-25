#pragma once
//Defines 3 classes: Reservoir, PHSensor and WaterTempSensor

#include "420Common.h"

class GrowBox;  //forward declaration 

class Reservoir : public Common
{
  private:
  
  protected:
   
  public:
  
};

class PHSensor : public Common
{
  private:
    GrowBox * GBox;
    RollingAverage * PH;
    float * Intercept;
    float * Slope;
    byte Pin;
  
  protected:
   
  public:
    PHSensor(GrowBox * GBox, byte Pin, float * Intercept, float * Slope);
    void refresh();
    void report();
    void getPH(bool ShowRaw);
    void setSlope(float Value);
    void setIntercept(float Value);    
};

class WaterTempSensor : public Common
{
  private:
  
  protected:
   
  public:
    
};

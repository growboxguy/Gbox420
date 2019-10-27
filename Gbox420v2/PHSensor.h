#pragma once
//Defines 3 classes: Reservoir, PHSensor and WaterTempSensor

#include "420Common.h"

class GrowBox;  //forward declaration 

class PHSensor : public Common
{
  public:
    PHSensor(GrowBox * GBox, byte Pin, float * Intercept, float * Slope);
    void websiteEvent_Load(char * url);
    void websiteEvent_Refresh(char * url){};
    void websiteEvent_Button(char * Button){};
    void websiteEvent_Field(char * Field){};
    void refresh();
    void report(){}; //{} means this function is empty, it has no implementation in the cpp file
    void getPH(bool ShowRaw);
    void setSlope(float Value);
    void setIntercept(float Value);

  private:
    GrowBox * GBox;
    byte Pin;
    RollingAverage * PH;
    float * Intercept;
    float * Slope;
  
  protected:    
};
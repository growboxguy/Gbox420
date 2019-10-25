#pragma once
//Defines 3 classes: Reservoir, PHSensor and WaterTempSensor

#include "420Common.h"

class GrowBox;  //forward declaration 

class PHSensor : public Common
{
  public:
    PHSensor(GrowBox * GBox, byte Pin, float * Intercept, float * Slope);
    void websiteLoadEvent(char * url);
    void websiteRefreshEvent(char * url){};
    void websiteButtonPressEvent(char * Button){};
    void websiteFieldSubmitEvent(char * Field){};
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

class Reservoir : public Common
{
  public:
    void websiteLoadEvent(char * url){};
    void websiteRefreshEvent(char * url){};
    void websiteButtonPressEvent(char * Button){};
    void websiteFieldSubmitEvent(char * Field){};  
    void refresh(){};
    void report(){};

  private:
  
  protected:  
};


class WaterTempSensor : public Common
{
  private:
  
  protected:
   
  public:
    
};
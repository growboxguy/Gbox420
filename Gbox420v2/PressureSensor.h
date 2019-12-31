#pragma once

#include "420Common.h"

class GrowBox;  //forward declaration

class PressureSensor : public Common
{
  public:
    PressureSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::PressureSensorSettings * DefaultSettings);
    RollingAverage * Pressure;
    void websiteEvent_Load(__attribute__((unused)) char * url);
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void websiteEvent_Button(char * Button);
    void websiteEvent_Field(char * Field);
    void refresh();  //Called when component should refresh its state
    void report();
    bool PreviousPressureRead = true;
    void readPressure();
    float getPressure();
    char * getPressureText(bool IncludeUnits);   
    void readOffset();
    void setOffset(float Value);
    void setRatio(float Value);
  
  private:
    GrowBox * GBox;
    byte * Pin;    
    float * Offset;
    float * Ratio;
  
  protected:
};
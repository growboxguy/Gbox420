#pragma once

#include "../../420Common.h"

class GrowBox;  //forward declaration

class LightSensor : public Common
{
  public:
    LightSensor(const __FlashStringHelper * Name, GrowBox * GBox, Settings::LightSensorSettings * DefaultSettings);
    void websiteEvent_Refresh(__attribute__((unused)) char * url);
    void websiteEvent_Load(__attribute__((unused)) char * url); 
    void websiteEvent_Button(char * Button);
    void refresh_Minute();  //Called when component should refresh its state
    void report();
    void triggerCalibration();    //Website signals to calibrate the MAX/MIN readings the next time the object gets CPU time
    char * getCalibrationText(); //Light sensor analog feedback relative to calibration values: 0 to 100% 
    int getReading(bool ReturnAverage); //Light sensor analog feedback: 0(darkest) to 1023 (brightest)
    char * getReadingText(bool IncludePercentage, bool ReturnAverage); //returns the current light sensor reading. If IncludeMinMax is set to true it also includes the calibated minimum and maximum readings
    bool getDark(); //Light sensor digital feedback: True(Bright) or False(Dark)
    char * getDarkText(bool UseWords);  

  private:
    GrowBox * GBox;
    byte * DigitalPin; //D0 - LM393 light sensor digital in
    byte * AnalogPin; //A0 - LM393 light sensor analog in
    RollingAverage * LightReading;  //keeps an average of previous readings: Smoothens sensor readings
    void calibrate();
    bool Dark;  
    bool CalibrateRequested;
    int MaxReading; //Analog reading with maximum brightnes
    int MinReading; //Analog reading with minimum brightnes
    int DarkReading; //Analog reading with lights off

  protected:
};

//WEBSITE COMPONENT
/*
<div class="card" style="width:90%">		
  <h1>Light sensor</h1>
  <table class="tg">
    <tr>
    <td class="headerRows"><span style="font-weight:bold">Is dark</span></td>
    <td class="headerRows"><span style="font-weight:bold">Reading</span></td>
    <td class="headerRows"><span style="font-weight:bold">Dark/Min/Max</span></td>
    </tr>
    <tr>
    <td id="LightSensor1_Dark" class="dataRows"></td>
    <td id="LightSensor1_Reading" class="dataRows"></td>
    <td id="LightSensor1_Calibrated" class="dataRows"></td>
    </tr>				 
  </table> 
  <button id="LightSensor1_Calibrate" type="button" title="Measure light reading in dark, 0 and 100% brightness">Calibrate</button><br>
</div>	
*/
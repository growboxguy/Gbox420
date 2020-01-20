#pragma once

#include "../../420Common.h"
#include "Lights.h"

class GrowBox; //forward declaration

class LightSensor : public Common
{
public:
  LightSensor(const __FlashStringHelper *Name, GrowBox *GBox, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource);
  void websiteEvent_Load(__attribute__((unused)) char *url);
  void websiteEvent_Refresh(__attribute__((unused)) char *url);
  void websiteEvent_Button(char *Button);
  void refresh_Minute();
  void report();
  void triggerCalibration();  //Website signals to calibrate the Dark/Min/Max readings at the next refresh trigger
  char *getCalibrationText();  //Light sensor analog feedback relative to calibration values: 0 to 100%
  int getReading(bool ReturnAverage = true);  //Light sensor analog feedback: 0(darkest) to 1023 (brightest)
  char *getReadingText(bool IncludePercentage, bool ReturnAverage);  //returns the current light sensor reading
  bool getDark(); //Light sensor digital feedback: True(Dark) or False(Bright)  
  char *getDarkText(bool UseWords);

private:
  GrowBox *GBox;
  Lights *LightSource; //The light used during calibration
  byte *DigitalPin;    //D0 - LM393 light sensor digital in
  byte *AnalogPin;     //A0 - LM393 light sensor analog in
  RollingAverage *LightReading; //keeps an average of previous readings: Smoothens sensor readings
  void calibrate(bool AddToLog = true);
  bool Dark;
  bool CalibrateRequested = false;
  int MaxReading = 0;  //Analog reading with maximum brightness
  int MinReading = 0;  //Analog reading with minimum brightness
  int DarkReading = 0; //Analog reading with lights off

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
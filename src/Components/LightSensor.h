#pragma once

#include "420Common.h"
#include "420Module.h"
#include "Lights.h"

///LM393 Light sensor

class LightSensor : virtual public Common
{
public:
  LightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource);
  void refresh_FiveSec();
  void report();
  void triggerCalibration();          ///Website signals to calibrate the Dark/Min/Max readings at the next refresh trigger
  void getCalibrationReadings();      ///Light reading at dark,0,10,20...100%
  int getReading();                   ///Light sensor analog feedback: 0(darkest) to 1023 (brightest)
  char *getReadingText(bool UseText); ///returns the current light sensor reading
  bool getDark();                     ///Light sensor digital feedback: True(Dark) or False(Bright)
  char *getDarkText(bool UseWords);

private:
  Lights *LightSource;                     ///The light used during calibration
  uint8_t *DigitalPin;                     ///D0 - LM393 light sensor digital in
  uint8_t *AnalogPin;                      ///A0 - LM393 light sensor analog in
  int LightReading = 0;                    ///keeps an average of previous readings: Smoothens sensor readings
  static const int DelaySec = 250;         ///how many miliseconds to wait after changing brightness for the driver to adjust
  static const int ReadingArrayDepth = 12; ///How many readings to take during a calibration (Do not change)
  void calibrate(bool AddToLog = true);
  bool Dark;
  bool CalibrateRequested = false;
  int MaxReading = 0;              ///Analog reading with maximum brightness
  int MinReading = 0;              ///Analog reading with minimum brightness
  int DarkReading = 0;             ///Analog reading with lights off
  int Readings[ReadingArrayDepth]; ///Stores the calibration readings for every 10% increment. [0] is darkness

protected:
  Module *Parent;
};

///WEBSITE COMPONENT
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
    <td id="LtSen1_Dark" class="dataRows"></td>
    <td id="LtSen1_Reading" class="dataRows"></td>
    <td id="LtSen1_Calibrated" class="dataRows"></td>
    </tr>				 
  </table> 
  <button id="LtSen1_Calibrate" type="button" title="Measure light reading in dark, 0 and 100% brightness">Calibrate</button><br>
</div>	
*/
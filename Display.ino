void logToScreen(){
  Screen.fillScreen(ILI9341_BLACK);
  Screen.setCursor(0, 0);
  Screen.setTextColor(ILI9341_GREEN);Screen.setTextSize(3);  
  Screen.println("GrowBoxGuy"); 
  Screen.setTextColor(ILI9341_WHITE);Screen.setTextSize(2);  
  Screen.println(CurrentTime);
  Screen.setTextColor(ILI9341_ORANGE);Screen.setTextSize(3);
  Screen.print(BoxTempC); Screen.print(F("C,Hum:"));Screen.print(Humidity); Screen.println( F("%"));
  Screen.setTextColor(ILI9341_BLUE);Screen.setTextSize(2);
  Screen.print(Power); Screen.print(F("W, Total:"));Screen.print(Energy); Screen.println(F("kWh"));   
  Screen.setTextColor(ILI9341_RED);
  Screen.print(F("PH:")); Screen.print(PH);  Screen.print(F(",Moisture:")); Screen.print(Moisture); Screen.println(F("% "));
  Screen.setTextColor(ILI9341_YELLOW);Screen.setTextSize(2);
  if(MySettings.isLightOn) Screen.print(F("Light is on, "));else Screen.print(F("Light is off, "));  
  if(isBright) Screen.println(F("it is bright"));else Screen.println(F("it is dark"));
  Screen.print(F("Intensity:")); Screen.print(MySettings.LightBrightness);Screen.print(F(" ("));Screen.print(LightReading); Screen.print(F(" - ")); Screen.print( LightReadingPercent); Screen.println(F("%)"));
  Screen.setTextColor(ILI9341_MAGENTA);Screen.setTextSize(2);
  Screen.println(reservoirToText(true));
  Screen.setTextColor(ILI9341_CYAN);Screen.setTextSize(3);
  Screen.print(F("On:")); Screen.print(MySettings.LightOnHour);Screen.print(F(":"));Screen.print(MySettings.LightOnMinute);Screen.print(F(",Off:")); Screen.print(MySettings.LightOffHour);Screen.print(F(":"));Screen.println(MySettings.LightOffMinute);
  Screen.setTextColor(ILI9341_WHITE);Screen.setTextSize(2);
  Screen.print(F("Internal "));Screen.print(fanSpeedToText(true));
  Screen.print(F(", Exhaust "));Screen.println(fanSpeedToText(false));
  Screen.setTextColor(ILI9341_GREEN);
  Screen.print(F("Pressure:"));Screen.print(AeroPressure);Screen.print(F(" ("));Screen.print(MySettings.AeroPressureLow);Screen.print(F("/"));Screen.print(MySettings.AeroPressureHigh);Screen.println(F(")"));
  Screen.print(F("Interval:"));Screen.print(MySettings.AeroInterval);Screen.print(F(", Duration:"));Screen.println(MySettings.AeroDuration/1000);
}

//***4 digit display***
//Global variables for updateDisplay funciton
int CurrentStep = 0;
const byte degreeSign = B11100011; //Used at Temp step
void updateDisplay() { //Cycles through the different phases
    DigitDisplay.clear();   
    switch (CurrentStep) {
    case 0:
      DigitDisplay.print(F("Humi"));
      break;
    case 1:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)Humidity , 100 * (Humidity - (int)Humidity));DigitDisplay.setColonOn(0);      
      break;
    case 2:
      DigitDisplay.print(F("Temp"));
      break;
    case 3:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)BoxTempC , 100 * (BoxTempC - (int)BoxTempC));DigitDisplay.printRaw(degreeSign, 3);DigitDisplay.setColonOn(0);
      break;
    case 4:
      DigitDisplay.print(F("LGHT"));
      break;
    case 5:
      if(MySettings.isLightOn) DigitDisplay.print(F("On")); else DigitDisplay.print(F("Off"));
      break;
    case 6:
      DigitDisplay.print(F("Watt"));
      break;
    case 7:
      DigitDisplay.print((int)Power);
      break;      
    case 8:
      DigitDisplay.print(F("Sum"));
      break;
    case 9:
      DigitDisplay.print((int)Energy);
      break; 
    case 10:
      DigitDisplay.print(F("IFan"));
      break;
    case 11:
      DigitDisplay.print(fanSpeedToText(true));
      break;
    case 12:
      DigitDisplay.print(F("EFan"));
      break;
    case 13:
      DigitDisplay.print(fanSpeedToText(false));      
      break;
    case 14:
      DigitDisplay.print(F("PRES"));
      break;
    case 15:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)AeroPressure , 100 * (AeroPressure - (int)AeroPressure));DigitDisplay.setColonOn(0);
      CurrentStep = -1; //last step, reset counter
      break;  
    default:      
      CurrentStep = -1; //nothing matched - reset counter
      break;
  }
  CurrentStep++;  
  //CurrentStep = 15; //locked to Pressure for calibration
}

void setDigitDisplayBacklight(int Backlight){
  MySettings.DigitDisplayBacklight=Backlight;
  DigitDisplay.setBacklight(MySettings.DigitDisplayBacklight);
}
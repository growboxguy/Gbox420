void logToScreen(){
  Screen.fillScreen(ILI9341_BLACK);
  Screen.setCursor(0, 0);
  Screen.setTextColor(ILI9341_GREEN);Screen.setTextSize(3);  
  Screen.println(F("GrowBoxGuy")); 
  Screen.setTextColor(ILI9341_WHITE);Screen.setTextSize(2);  
  Screen.println(getFormattedTime());
  Screen.setTextColor(ILI9341_ORANGE);Screen.setTextSize(3);
  Screen.print(BoxTempC); Screen.print(F("C,Hum:"));Screen.print(Humidity); Screen.println( F("%"));
  Screen.setTextColor(ILI9341_BLUE);
  Screen.print(Power); Screen.print(F("W,"));Screen.print(Energy); Screen.println(F("kWh"));   
  Screen.setTextColor(ILI9341_RED);Screen.setTextSize(2);
  //Screen.setTextColor(ILI9341_YELLOW);Screen.setTextSize(2);
  Screen.print(F("Light:")); Screen.print(lightStatusToText()); Screen.print(F(",Bright:")); Screen.println(isBrightToText());
  Screen.print(F("Intensity:")); Screen.print(MySettings.LightBrightness);Screen.print(F(" ("));Screen.print(LightReading); Screen.print(F(" - ")); Screen.print( LightReadingPercent); Screen.println(F("%)"));
  Screen.print(F("On:")); Screen.print(timetoText(MySettings.LightOnHour, MySettings.LightOnMinute));Screen.print(F(",Off:")); Screen.println(timetoText(MySettings.LightOffHour,MySettings.LightOffMinute));
  Screen.setTextColor(ILI9341_MAGENTA);Screen.setTextSize(2);
  Screen.print(F("Reservoir:"));Screen.print(reservoirText);Screen.print(F(",PH:"));Screen.println(PH);
  //Screen.setTextColor(ILI9341_CYAN);Screen.setTextSize(3);
  Screen.setTextColor(ILI9341_WHITE);Screen.setTextSize(2);
  Screen.print(F("Internal:"));Screen.print(internalFanSpeedToText());Screen.print(F(",Exhaust:"));Screen.println(exhaustFanSpeedToText());
  Screen.setTextColor(ILI9341_GREEN);
  Screen.print(F("Pressure:"));Screen.print(AeroPressure);Screen.print(F(" ("));Screen.print(MySettings.AeroPressureLow);Screen.print(F("/"));Screen.print(MySettings.AeroPressureHigh);Screen.println(F(")"));
  Screen.print(F("Interval:"));Screen.print(MySettings.AeroInterval);Screen.print(F(",Duration:"));Screen.println(MySettings.AeroDuration);
  Screen.print(F("Pump:"));Screen.print(pumpStateToText());Screen.print(F("("));Screen.print(pumpStatusToText());Screen.println(F(")"));
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
      DigitDisplay.print(lightStatusToText());
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
      DigitDisplay.print(internalFanSpeedToText());
      break;
    case 12:
      DigitDisplay.print(F("EFan"));
      break;
    case 13:
      DigitDisplay.print(exhaustFanSpeedToText());      
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

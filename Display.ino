void logToScreen(){
  Screen.fillScreen(ILI9341_BLACK);  //wipe previous screen
  Screen.setCursor(0, 0);
  Screen.setTextColor(ILI9341_GREEN);Screen.setTextSize(3); //waste of screen space customization 
  Screen.println(F("GrowBoxGuy")); //waste of screen space customization
  Screen.setTextColor(ILI9341_WHITE);Screen.setTextSize(2);  
  Screen.println(getFormattedTime());
  Screen.setTextColor(ILI9341_ORANGE);
  Screen.print(F("External:"));Screen.print(ExternalTemp); if(MySettings.MetricSystemEnabled)Screen.print(F("C,"));else Screen.print(F("F,"));Screen.print(ExternalHumidity); Screen.println(F("%"));
  Screen.print(F("Internal:"));Screen.print(InternalTemp); if(MySettings.MetricSystemEnabled)Screen.print(F("C,"));else Screen.print(F("F,"));Screen.print(InternalHumidity); Screen.println(F("%"));
  Screen.setTextColor(ILI9341_BLUE);
  Screen.print(F("Power:"));Screen.print(Power); Screen.print(F("W,"));Screen.print(Energy); Screen.println(F("kWh"));   
  Screen.setTextColor(ILI9341_RED);
  Screen.print(F("Light:")); Screen.print(stateToText(MySettings.LightStatus)); Screen.print(F(",Bright:")); Screen.println(BrightToText());
  Screen.print(F("Intensity:")); Screen.print(MySettings.LightBrightness);Screen.print(F("% - "));Screen.println(LightReading);
  Screen.print(F("On:")); Screen.print(timetoText(MySettings.LightOnHour, MySettings.LightOnMinute));Screen.print(F(",Off:")); Screen.println(timetoText(MySettings.LightOffHour,MySettings.LightOffMinute));
  Screen.setTextColor(ILI9341_MAGENTA);
  Screen.print(F("Reservoir:"));Screen.println(ReservoirText);
  Screen.print(F(",PH:"));Screen.print(PH); Screen.print(F(",Temp:"));Screen.print(ReservoirTemp);if(MySettings.MetricSystemEnabled)Screen.println(F("C"));else Screen.println(F("F"));  
  Screen.setTextColor(ILI9341_WHITE);
  Screen.print(F("Internal:"));Screen.print(fanSpeedToText(true));Screen.print(F(",Exhaust:"));Screen.println(fanSpeedToText(false));
  Screen.setTextColor(ILI9341_GREEN);
  Screen.print(F("Pressure:"));Screen.print(AeroPressure);Screen.print(F(" ("));Screen.print(MySettings.AeroPressureLow);Screen.print(F("/"));Screen.print(MySettings.AeroPressureHigh);Screen.println(F(")"));
  Screen.print(F("Interval:"));Screen.print(MySettings.AeroInterval);Screen.print(F(",Duration:"));Screen.println(MySettings.AeroDuration);
  Screen.print(F("Pump:"));Screen.print(pumpStateToText());
  //Screen.setTextColor(ILI9341_CYAN);
  //Screen.setTextColor(ILI9341_YELLOW);
}

//***4 digit display***
int CurrentStep = 0;
const byte degreeSign = B11100011; //Used at Temp step
void updateDisplay() { //Cycles through the different values
    DigitDisplay.clear();
    if(MySettings.DigitDisplayValue == 0) CurrentStep++;  //cycle the values
    else CurrentStep= MySettings.DigitDisplayValue;  //lock to a single value
       
    switch (CurrentStep) {
    case 1:
      DigitDisplay.print(F("ITmp"));
      break;
    case 2:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)InternalTemp , 100 * (InternalTemp - (int)InternalTemp));DigitDisplay.printRaw(degreeSign, 3);DigitDisplay.setColonOn(0);
      break;
    case 3:      
      DigitDisplay.print(F("IHum"));
      break;
    case 4:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)InternalHumidity , 100 * (InternalHumidity - (int)InternalHumidity));DigitDisplay.setColonOn(0);      
      break;
    case 5:
      DigitDisplay.print(F("ITmp"));
      break;
    case 6:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)InternalTemp , 100 * (InternalTemp - (int)InternalTemp));DigitDisplay.printRaw(degreeSign, 3);DigitDisplay.setColonOn(0);
      break;
    case 7:      
      DigitDisplay.print(F("IHum"));
      break;
    case 8:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)InternalHumidity , 100 * (InternalHumidity - (int)InternalHumidity));DigitDisplay.setColonOn(0);      
      break;    
    case 9:
      DigitDisplay.print(F("PRES"));
      break;
    case 10:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)AeroPressure , 100 * (AeroPressure - (int)AeroPressure));DigitDisplay.setColonOn(0);
      break;
    case 11:
      DigitDisplay.print(F("PH"));
      break;
    case 12:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)PH , 100 * (PH - (int)PH));DigitDisplay.setColonOn(0);
      break; 
    case 13:
      DigitDisplay.print(F("Watt"));
      break;
    case 14:
      DigitDisplay.print((int)Power);
      break;
    case 15:
      DigitDisplay.print(F("Sum"));
      break;
    case 16:
      DigitDisplay.print((int)Energy);
      break;     
    case 17:
      DigitDisplay.print(F("LGHT"));
      break;
    case 18:
      DigitDisplay.print(stateToText(MySettings.LightStatus));
      break;
    case 19:
      DigitDisplay.print(F("IFan"));
      break;
    case 20:
      DigitDisplay.print(fanSpeedToText(true));
      break;
    case 21:
      DigitDisplay.print(F("EFan"));
      break;
    case 22:
      DigitDisplay.print(fanSpeedToText(false));
      CurrentStep = 0; //last step, reset counter      
      break;    
    default:      
      CurrentStep = 0; //nothing matched - reset counter
      break;
  }
}

void setDigitDisplayBacklight(int Backlight){
  MySettings.DigitDisplayBacklight=Backlight;
  DigitDisplay.setBacklight(MySettings.DigitDisplayBacklight);
  addToLog(F("Digit display brightness set"));
}

void setDigitDisplayLoop(){
  MySettings.DigitDisplayValue = 0;
  addToLog(F("Display set to loop"));
}

void setDigitDisplayLockTemp(){
  MySettings.DigitDisplayValue = 2;
  addToLog(F("Display locked to Temp"));
}

void setDigitDisplayLockPressure(){
  MySettings.DigitDisplayValue = 10;
  addToLog(F("Display locked to Pressure"));
}

void setDigitDisplayLockPH(){
  MySettings.DigitDisplayValue = 12;
  addToLog(F("Display locked to PH"));
}

void setDigitDisplayLockWattage(){
  MySettings.DigitDisplayValue = 14;
  addToLog(F("Display locked to Wattage"));
}

void setDigitDisplayValue(int Value){
  MySettings.DigitDisplayValue=Value;
  if(Value<0)addToLog(F("Digit display cycle ON"));
  else addToLog(F("Digit display locked"));
}

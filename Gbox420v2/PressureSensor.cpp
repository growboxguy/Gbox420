// void readAeroPressure(){
//   float  Reading=0;
//   for(byte i=0;i<50;i++) { 
//    Reading+=analogRead(PressureSensorInPin);
//    delay(20);
//   }
//   Reading = Reading /50;
//   float Voltage = ((float)Reading) * 5 / 1024 ;
//   if(MySettings.MetricSystemEnabled) AeroPressure = MySettings.PressureSensorRatio*(Voltage-MySettings.PressureSensorOffset); // unit: bar / 100kPa
//   else AeroPressure = MySettings.PressureSensorRatio*(Voltage-MySettings.PressureSensorOffset) * 14.5038;  //unit: PSI
// }


// void calibratePressureSensor(){  //Should only be called when there is 0 pressure
//   float sum = 0;
//   for(byte i = 0; i<50;i++){
//   sum += analogRead(PressureSensorInPin) ; 
//   delay(10);
//   }  
//   float AeroOffsetRecommendation = (sum/50)*5/1024; //Reads voltage at 0 pressure
//   strncpy_P(LogMessage,(PGM_P)F("0 pressure AeroOffset: "),MaxTextLength);
//   strcat(LogMessage,toText(AeroOffsetRecommendation));
//   addToLog(LogMessage);
// }

// void setPressureSensorOffset(float Value){
//   MySettings.PressureSensorOffset = Value;
//   addToLog(F("Pressure offset updated"));  
// }

// void setPressureSensorRatio(float Value){
//   MySettings.PressureSensorRatio = Value;
//   addToLog(F("Pressure/voltage ratio updated"));  
// }

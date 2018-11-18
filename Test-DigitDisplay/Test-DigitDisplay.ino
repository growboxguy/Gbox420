//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: 4 digit display

//Libraries
  #include "SevenSegmentTM1637.h" // 4 digit LED display
  #include "SevenSegmentExtended.h" // 4 digit LED display

//Pins
  const byte DigitDisplayCLKOutPin = 31; //CLK - 4 digit LED display
  const byte DigitDisplayDI0OutPin = 30; //DIO - 4 digit LED display

//Global variables - fake data to display
  byte DigitDisplayBacklight = 75; //4 digit display - backlight strenght (0-100)
  float BoxTempC = 17.10;
  float Humidity = 4.20; 
  bool isLightOn = true;
  char Message[] = "GrowBoxGuy";

//Component initialization
  SevenSegmentExtended DigitDisplay(DigitDisplayCLKOutPin, DigitDisplayDI0OutPin); //4 digit LED Display

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
  DigitDisplay.begin(); //start 4 digit LED display
  DigitDisplay.setBacklight(DigitDisplayBacklight); //set 4 digit LED display backlight intensity
}

void loop() {  // put your main code here, to run repeatedly:
Serial.println(F("Updating display")); 
updateDisplay();
Serial.print("Humidity: "); Serial.println(Humidity);
Serial.print("Humidity casted to int: ");Serial.println((int)(Humidity));
Serial.print("Modulo 100 divison result: ");Serial.println(((int)round((100.0 * Humidity)))% 100); //modulo operation finds the remainder after division of one number by another 
Serial.print("BoxTempC: "); Serial.println(BoxTempC);
Serial.print("BoxTempC casted to int: ");Serial.println((int)(BoxTempC));
Serial.print("BoxTempC 100 divison result: ");Serial.println(((int)round((100.0 * BoxTempC)))% 100); //modulo operation finds the remainder after division of one number by another 

delay(5000); //update display every 5 seconds
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
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)Humidity , ((int)round((100.0 * Humidity)))% 100);DigitDisplay.setColonOn(0);      
      break;
    case 2:
      DigitDisplay.print(F("Temp"));
      break;
    case 3:
      DigitDisplay.setColonOn(1);DigitDisplay.printDualCounter((int)BoxTempC , ((int)round((100.0 * BoxTempC)))% 100);DigitDisplay.printRaw(degreeSign, 3);DigitDisplay.setColonOn(0);
      break;
    case 4:
      DigitDisplay.print(F("LGHT"));
      break;
    case 5:
      if(isLightOn) DigitDisplay.print(F("On")); else DigitDisplay.print(F("Off"));
      break;
    case 6:
      DigitDisplay.print(Message);
      CurrentStep = -1; //last step, reset counter
      break;    
    default:      
      CurrentStep = -1; //nothing matched - reset counter
      break;
  }
  CurrentStep++;  
}


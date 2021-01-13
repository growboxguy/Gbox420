//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing PWM Output/controlling of AC devices using Robodyn AC Light Dimmin Module with Zero-cross detection
//Library: https://github.com/RobotDynOfficial/RBDDimmer
//PS: Library acts funky below 25% output, best not to dim under 25%

#include "RBDdimmer.h"

//Pins
//const byte ZC_Pin = 2;   //Zero Cross detection pin - This pin is FIXED to D2 digital pin due to Interroupt handling (Mega2560 and Nano)
const byte PWM_Pin = 13;  //Mega2560 PWM signal that sets the Output (D2-D13 supported)
//const byte PWM_Pin = 6;  //Nano PWM signal that sets the Output (D3,D5,D6,D9-D12 supported)
const byte MinOutput = 30;
const byte MaxOutput = 100;
int Output = MaxOutput;  //Start with full power
bool isGettingHigh = true; //Direction to increase or decrease Output

dimmerLamp ACDevice(PWM_Pin);   //Any device that can be controlled adjusted using AC PWM signals (AC Motors, Incandescent light bulb)

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing AC PWM Output/controlling"));
  Serial.println();

  ACDevice.begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(Mode: NORMAL_MODE/TOGGLE_MODE, State:ON/OFF) 
  delay(5000); //Wait 5 seconds
  Serial.println(F("Turning on device..."));
  ACDevice.setPower(Output);
  ACDevice.setState(ON); //Turn on device

  //Mode: NORMAL_MODE to make dimmer work in defined value from 0 to 100 (%) (integer)
  //Mode: TOGGLE_MODE smooth change of Output value up or down in a defined range. This solutions implies change of Output values by means of hardware timer, without using the cycle code.
  //State: ON - turns timer ON, allows to use dimmer.
  //State: OFF - turns timer parameters OFF, prevents the use of dimmer.
}

void loop()
{
 while (MinOutput <= Output && Output <= MaxOutput)
  {
    ACDevice.setPower(Output); 
    
    if (Output % 10 == 0)   //modulo division, https://www.arduino.cc/reference/en/language/structure/arithmetic-operators/modulo/
    {
      Serial.print(Output);
      Serial.println("%");
    }
    if (isGettingHigh)
      Output++;
    else
      Output--;
    delay(1000); //Wait 1seconds between 1% change
  }

  //When the while loop above breaks the Output is out of the 0 to 100 bounds
  if (isGettingHigh)
  {
    Serial.println("MAX Output reached");
    Output = MaxOutput; //Set Output back within bounds
  }
  else
  {
    Serial.println("MIN Output reached");
    Output = MinOutput;  //Set Output back within bounds
  }
  isGettingHigh = !isGettingHigh; // flip the direction
  delay(5000);                    //Wait 5 seconds
}

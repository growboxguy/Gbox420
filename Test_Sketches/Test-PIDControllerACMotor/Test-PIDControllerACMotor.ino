/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include <PID_v1.h>
#include "analogComp.h" //< Internal comparator library
#include "movingAvg.h"  ///< Moving average calculation for Speed adjuster 10kOhm Potentiometer
#include "RBDdimmer.h"  //< For the AC PWM controller

#define SpeedPotPin A0
#define SpeedPWMPin 10
#define OnOffRelay A4

const uint8_t TachoPulsesPerRevolution = 16;
uint16_t RPMLimitLow = 2000;
uint16_t RPMLimitHigh = 10000;
uint16_t SpeedLimitLow = 28;
uint16_t SpeedLimitHigh = 60;
uint32_t RPMLastCalculation = millis(); ///< Timestamp of the last RPM calculation
static long TachoPulseCounter;          //Count the total number of interrupts
movingAvg *AverageSpeedReading;
dimmerLamp *PWMController; //AC dimmer

//Define Variables we'll be connecting to
double Target, Input, Output;

//Specify the links and initial tuning parameters
double Kp = 0.1, Ki = 0.1, Kd = 0;
PID myPID(&Input, &Output, &Target, Kp, Ki, Kd, P_ON_M, DIRECT);

void setup()
{
  pinMode(SpeedPotPin, INPUT);
  pinMode(OnOffRelay, OUTPUT);
  AverageSpeedReading = new movingAvg(10); //Create an average of the last 10 speed potentiometer readings
  AverageSpeedReading->begin();
  PWMController = new dimmerLamp(SpeedPWMPin);
  PWMController->begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(Mode: NORMAL_MODE/TOGGLE_MODE, State:ON/OFF)
  PWMController->setPower(0);
  TachoPulseCounter = 0;
  analogComparator.setOn(INTERNAL_REFERENCE, AIN1);       // AINO (D6) , the external signal is connected to ANI1 (D7)
  analogComparator.enableInterrupt(tachoTrigger, RISING); // Call the trigger function when an intertupt is raised by the comparator (When ANT1 (D7) goes over AIN0)
  myPID.SetMode(AUTOMATIC);                               //turn the PID on
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  digitalWrite(OnOffRelay, LOW);
  PWMController->setState(ON);
}

void loop()
{
  Target = readSpeedTarget();
  Input = readRPM();
  myPID.Compute();
  PWMController->setPower(map(Output,0,255,SpeedLimitLow,SpeedLimitHigh));
  Serial.print(F("Target: "));
  Serial.println(Target);
  Serial.print(F("Feedback: "));
  Serial.println(Input);
  Serial.print(F("Output: "));
  Serial.println(Output);
  delay(200);

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == 'q')
      Kp += 0.1;
    else if (temp == 'a')
      Kp -= 0.1;
    else if (temp == 'w')
      Ki += 0.1;
    else if (temp == 's')
      Ki -= 0.1;
    else if (temp == 'e')
      Kd += 0.1;
    else if (temp == 'd')
      Kd -= 0.1;

    Serial.print(F("Kp: "));
    Serial.print(Kp);
    Serial.print(F(", Ki: "));
    Serial.print(Ki);
    Serial.print(F(", Kd: "));
    Serial.println(Kd);
    myPID.SetTunings(Kp, Ki, Kd);
  }
}

uint16_t readSpeedTarget()
{
  AverageSpeedReading->reading(map(analogRead(SpeedPotPin), 0, 1023, RPMLimitLow, RPMLimitHigh)); //take a reading and map it between 0 - 100%
  return AverageSpeedReading->getAvg();
}

float readRPM()
{
  uint32_t ElapsedTime = millis() - RPMLastCalculation;
  float RPM = (float)TachoPulseCounter / ElapsedTime / TachoPulsesPerRevolution * 60000;
  TachoPulseCounter = 0;
  RPMLastCalculation = millis();
  return RPM;
}

void tachoTrigger()
{
  TachoPulseCounter++;
}

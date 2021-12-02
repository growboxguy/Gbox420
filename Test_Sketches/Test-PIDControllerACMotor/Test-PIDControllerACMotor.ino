/********************************************************
 * PID Basic Example
 * Reading analog inpput 0 to control an AC motor
 ********************************************************/

#include <PID_v1.h>
#include "analogComp.h" //< Internal comparator library
#include "movingAvg.h"  ///< Moving average calculation for Speed adjuster 10kOhm Potentiometer
#include "RBDdimmer.h"  //< For the AC PWM controller

const uint8_t RPMRPMTargetPin = A0;
const uint8_t OnOffRelay = A4;
const uint8_t SpeedPWMPin = 10;
const uint8_t TachoPulsesPerRevolution = 16;

uint16_t RPMLimitLow = 2000;
uint16_t RPMLimitHigh = 10000;
uint16_t ACDimmerLimitMin = 28;
uint16_t ACDimmerLimitMax = 60;

movingAvg *AverageRPMRPMTarget;
dimmerLamp *ACDimmer;              //AC dimmer
uint32_t RPMLastCalculation = millis(); ///< Timestamp of the last RPM calculation
static long TachoPulseCounter;          //Count the total number of tacho pulses
double RPMTarget, RPMCurrent, Speed;        //PID control related variables
double Kp = 0.1, Ki = 0.1, Kd = 0;       //PID control: Specify the links and initial tuning parameters
PID PidController(&RPMCurrent, &Speed, &RPMTarget, Kp, Ki, Kd, P_ON_M, DIRECT);

void setup()
{
  pinMode(RPMRPMTargetPin, INPUT);
  pinMode(OnOffRelay, OUTPUT);
  AverageRPMRPMTarget = new movingAvg(10); //Create an average of the last 10 speed potentiometer readings
  AverageRPMRPMTarget->begin();
  ACDimmer = new dimmerLamp(SpeedPWMPin);
  ACDimmer->begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(Mode: NORMAL_MODE/TOGGLE_MODE, State:ON/OFF)
  ACDimmer->setPower(0);
  TachoPulseCounter = 0;
  analogComparator.setOn(INTERNAL_REFERENCE, AIN1);       // AINO (D6) , the external signal is connected to ANI1 (D7)
  analogComparator.enableInterrupt(tachoTrigger, RISING); // Call the trigger function when an intertupt is raised by the comparator (When ANT1 (D7) goes over AIN0)
  PidController.SetMode(AUTOMATIC);                               //turn the PID on
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  digitalWrite(OnOffRelay, LOW);
  ACDimmer->setState(ON);
}

void loop()
{
  RPMTarget = readRPMTarget();
  RPMCurrent = readRPM();
  PidController.Compute();
  ACDimmer->setPower(map(Speed, 0, 255, ACDimmerLimitMin, ACDimmerLimitMax));
  Serial.print(F("RPMTarget: "));
  Serial.println(RPMTarget);
  Serial.print(F("RPMCurrent: "));
  Serial.println(RPMCurrent);
  Serial.print(F("Speed: "));
  Serial.println(Speed);
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
    PidController.SetTunings(Kp, Ki, Kd);
  }
}

uint16_t readRPMTarget()
{
  AverageRPMRPMTarget->reading(map(analogRead(RPMRPMTargetPin), 0, 1023, RPMLimitLow, RPMLimitHigh)); //take a reading and map it between 0 - 100%
  return AverageRPMRPMTarget->getAvg();
}

double readRPM()
{
  uint32_t ElapsedTime = millis() - RPMLastCalculation;
  double RPM = (double)TachoPulseCounter / ElapsedTime / TachoPulsesPerRevolution * 60000;
  TachoPulseCounter = 0;
  RPMLastCalculation = millis();
  return RPM;
}

void tachoTrigger()
{
  TachoPulseCounter++;
}

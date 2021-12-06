/********************************************************
 * PID Basic Example
 * Reading analog inpput 0 to control an AC motor
 ********************************************************/

#include <PID_v1.h>
#include "analogComp.h" //< Internal comparator library
#include "movingAvg.h"  ///< Moving average calculation for Speed adjuster 10kOhm Potentiometer
#include "RBDdimmer.h"  //< For the AC PWM controller

const uint8_t RPMTargetPin = A0;
const uint8_t OnOffRelay = A4;
const uint8_t ACDimmerPWMPin = 10;
const uint8_t TachoPulsesPerRevolution = 16;
bool PIDEnabled = false;

uint16_t RPMLimitMin = 2000;
uint16_t RPMLimitMax = 10000;
double ACDimmerLimitMin = 28;
double ACDimmerLimitMax = 60;

movingAvg *AverageRPMTarget;
dimmerLamp *ACDimmer; //AC dimmer
uint32_t PIDTimer = millis();
uint32_t OneSecondTimer = millis();
uint32_t LastRPMCalculation = millis(); ///< Timestamp of the last RPM calculation
static long TachoPulseCounter;          //Count the total number of tacho pulses
double RPMTarget = 0, RPMCurrent = 0, Speed = 28;    //PID control related variables
double Kp = 0.1, Ki = 0.05, Kd = 0;   //PID control: Specify the links and initial tuning parameters
PID PidController(&RPMCurrent, &Speed, &RPMTarget, Kp, Ki, Kd, P_ON_M, DIRECT);

void setup()
{
  pinMode(RPMTargetPin, INPUT);
  pinMode(OnOffRelay, OUTPUT);
  AverageRPMTarget = new movingAvg(10); //Create an average of the last 10 speed potentiometer readings
  AverageRPMTarget->begin();  
  TachoPulseCounter = 0;
  analogComparator.setOn(INTERNAL_REFERENCE, AIN1);       // AINO (D6) , the external signal is connected to ANI1 (D7)
  analogComparator.enableInterrupt(tachoTrigger, RISING); // Call the trigger function when an intertupt is raised by the comparator (When ANT1 (D7) goes over AIN0)
  PidController.SetOutputLimits(ACDimmerLimitMin, ACDimmerLimitMax);
  PidController.SetMode(AUTOMATIC);                       //turn the PID on
  Serial.begin(115200);
  Serial.println(F("Starting..."));
  digitalWrite(OnOffRelay, LOW);
  ACDimmer = new dimmerLamp(ACDimmerPWMPin);
  ACDimmer->begin(NORMAL_MODE, OFF); //dimmer initialisation: name.begin(Mode: NORMAL_MODE/TOGGLE_MODE, State:ON/OFF)
  ACDimmer->setPower(Speed);
  ACDimmer->setState(ON);
  updateRPMTarget();
}

void loop()
{
  //Adjust tuning parameters if there is a user input on Serial
  if (Serial.available())
  {
    while (Serial.available())
    {
      char temp = Serial.read();
      if (temp == 'q')
        Kp += 0.005;
      else if (temp == 'a' && Kp > 0)
        Kp -= 0.005;
      else if (temp == 'w')
        Ki += 0.005;
      else if (temp == 's' && Ki > 0)
        Ki -= 0.005;
      else if (temp == 'e')
        Kd += 0.005;
      else if (temp == 'd' && Kd > 0)
        Kd -= 0.005;
      else if (temp == 'i' && Kd > 0)
      {
        PIDEnabled = true;
        AverageRPMTarget->reset();
      }
      else if (temp == 'o' && Kd > 0)
      {
        PIDEnabled = false;
        AverageRPMTarget->reset();
      }
    }
    PidController.SetTunings(Kp, Ki, Kd);

    Serial.print(F("Kp: "));
    Serial.print(Kp);
    Serial.print(F(", Ki: "));
    Serial.print(Ki);
    Serial.print(F(", Kd: "));
    Serial.println(Kd);
  }

  if (millis() - PIDTimer > 200)
  {
    updateRPMCurrent();
    if (PIDEnabled)
    {
      PidController.Compute();
      ACDimmer->setPower(Speed);
    }
    else
    {
      updateSpeedTarget();      
    }
  }

  if (millis() - OneSecondTimer > 1000)
  {
    OneSecondTimer = millis();
    updateRPMTarget();
    Serial.print(F("Target: "));
    Serial.print(RPMTarget);
    Serial.print(F(", Current: "));
    Serial.print(RPMCurrent);
    Serial.print(F(", Speed: "));
    Serial.println(Speed);
  }
}

void updateSpeedTarget()  //when PID mode is disabled use the potentiometer to adjust motor speed by %
{
  AverageRPMTarget->reading(map(analogRead(RPMTargetPin), 0, 1023, ACDimmerLimitMin, ACDimmerLimitMax)); //take a reading and map it between 0 - 100%
  if(Speed != AverageRPMTarget->getAvg())
  {
    Speed = AverageRPMTarget->getAvg();
    ACDimmer->setPower(Speed);
  }
}

void updateRPMTarget()
{
  AverageRPMTarget->reading(map(analogRead(RPMTargetPin), 0, 1023, RPMLimitMin, RPMLimitMax)); //take a reading and map it between 0 - 100%
  RPMTarget = AverageRPMTarget->getAvg();
}

void updateRPMCurrent()
{
  uint32_t ElapsedTime = millis() - LastRPMCalculation;
  RPMCurrent = (double)TachoPulseCounter / ElapsedTime / TachoPulsesPerRevolution * 60000;
  TachoPulseCounter = 0;
  LastRPMCalculation = millis();
}

void tachoTrigger()
{
  TachoPulseCounter++;
}

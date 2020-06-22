/********************************************************
 * PID Basic Example
 * Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include <PID_v1.h>

#define PIN_INPUTTarget A1
#define PIN_INPUTFeedback A2
#define PIN_OUTPUTPwm 9

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=0.2, Ki=1, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd,P_ON_M, DIRECT);

void setup()
{
  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  Serial.begin(115200);
  Serial.println(F("Starting..."));
}

void loop()
{
  Setpoint = analogRead(PIN_INPUTTarget);
  Input = 1024 - analogRead(PIN_INPUTFeedback);
  myPID.Compute();
  analogWrite(PIN_OUTPUTPwm, Output);
  Serial.print(F("Target: "));
  Serial.println(Setpoint);
  Serial.print(F("Feedback: "));
  Serial.println(Input);
  Serial.print(F("PWM: "));
  Serial.println(Output);
  delay(200);

if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == 'q')
      Kp += 0.1;
    else if(temp == 'a')
      Kp -= 0.1;
    else if(temp == 'w')
      Ki += 0.1;
    else if(temp == 's')
      Ki -= 0.1;
    else if(temp == 'e')
      Kd += 0.1;
     else if(temp == 'd')
      Kd -= 0.1;
  }
  Serial.print(F("Kp: "));
  Serial.print(Kp);
  Serial.print(F(", Ki: "));
  Serial.print(Ki);
  Serial.print(F(", Kd: "));
  Serial.println(Kd);
  myPID.SetTunings(Kp, Ki, Kd);
}

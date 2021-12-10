/********************************************************
 PID Basic Example
 Reading RPM to control an AC motor using a TRIAC dimmer with zero cross detection

 Notes on the Universal AC Motor used in this test:
 - 320W max output
 - 220-240V - 50Hz AC
 - 2000 - 12500 RPM range
 - Tacho meter for measuring motor RPM -> 16 pulses/rotation, 26V AC signal, Needs an external comparator with Hysteresis (LM393 for example)

Notes on TRIAC controlling the AC waves:  https://sound-au.com/lamps/dimmers.html
- Using leading edge dimming logic -> beginning of the AC signal is cut off after the zero clossing (ideal for inductive loads like a motor)
- Max TRIAC gate turn on delay after a zero crossing: 
    50Hz AC : 10ms
    60Hz AC : 8.3335ms
- After a slight delay (couple ms) of pulling the TRIAC gate HIGH the signal can be switched LOW -> TRIAC will stay on until the zero crossing point

Notes on Timer1 used to control the TRIAC gate:
- 16bit timer: Max value 65535
- 16MHz frequency: 65535/16000000 = 0.0040959375 -> 4.096ms max delay without prescaler
- TCCR1B Register: Timer1 Prescaler, accepts the following values: 0x00 - Stop timer, 0x01 - No prescale (max ~4ms before overflow), 0x02: /8 prescale (max ~32ms), 0x03: /64 prescale, 0x04: /256 prescale,0x05: /1024 prescale  https://maxembedded.com/2011/06/avr-timers-timer1/ 
- TCNT1 register: 16bit timer/counter
- Overflows call an interrupt service routine: ISR(TIMER1_OVF_vect)
- Built in comperator to trigger an interrupt ISR(TIMER1_COMPA_vect) after X number of timer ticks
- AC signal period: https://en.wikipedia.org/wiki/Utility_frequency#/media/File:50Hz60Hz.svg
    50Hz : 20ms (Zero crossing every 10ms)
    60Hz : 16.667ms (Zero crossing every 8.3335ms)

 ********************************************************/

#include <PID_v1.h>
#include "movingAvg.h" ///< Moving average calculation for RPM adjuster 10kOhm Potentiometer

const uint8_t ZeroCrossingPin = 2;           ///< FIXED pin for incoming Zero Crossing interrupt
const uint8_t ComperatorPin = 3;             ///< FIXED connected to the Output1 pin of an LM393 comperator. LM393 IN1- pin: Motor Tacho cable1, GND PIN: Motor tacho cable2 + Arduino GND, and 5V to IN1+
const uint8_t TargetRPMPin = A0;             ///< Center leg of a 10kOhm potentiometer connected between GND and 5V. Adjusts the target motor RPM
const uint8_t OnOffButton = 4;               ///< Pin that needs to be pulled LOW to turn on the motor (Motor stops on releasing the button)
const uint8_t OnOffRelay = A4;               ///< Relay controlling AC power going into the TRIAC dimmer circuit
const uint8_t TriacPin = 10;                 ///< Signal controlling the TRIAC's gate. When pulled HIGH the TRIAC turns on (and stays on until at least the next zero crossing even if the pin is set to LOW)
const uint8_t TachoPulsesPerRevolution = 16; ///<Tacho meter for measuring motor RPM -> Number of pulses during a single rotation
bool PIDEnabled = true;                      ///< Enable/disable motor speed stabilization under variable load based on RPM feedback
bool MotorState = false;                     ///< true: turn on motor, false_ turn off motor

const uint16_t RPMLimitMin = 1500;  ///< Lowest target RPM
const uint16_t RPMLimitMax = 10000; ///< Highest target RPM
//const double SpeedLimitMin = 28;
//const double SpeedLimitMax = 60;
const int Prescale = 0x02;              ///< /8 prescale: 2Mhz  : Max 32.7675ms. 1 tick every 0.5μs on 50Hz
const double TriacTickDelayMin = 0;     ///< Shortest allowed delay before turning on the TRIAC after a zero crossing: High power output
const double TriacTickDelayMax = 15000; ///< Longest allowed delay before turning on the TRIAC after a zero crossing: Low power output. 20000 ticks -> 10ms
const int TriacOpenTicks = 40;          ///< Keep the HIGH signal on the TRIAC gate for 40 timer ticks -> 20μs

movingAvg *TargetRPM;                   ///< Target RPM adjusted by 10kOhm Potentiometer (TargetRPMPin)
uint32_t PIDTimer = millis();           ////< Timer for tracking PID re-calculation
uint32_t OneSecondTimer = millis();     ////< Timer for reporting to serial console
uint32_t LastRPMCalculation = millis(); ///< Timestamp of the last RPM calculation
volatile static long TachoPulseCounter; //Count the total number of tacho pulses, volatile: modified during an interupt (tachoTriggerInterrupt)
double PID_TargetRPM = RPMLimitMin;     ///< RPM the motor should reach
double PID_CurrentRPM = 0;              ///< actual RPM the motor
double PID_Output = TriacTickDelayMin;
double Delay = map(PID_Output, TriacTickDelayMin, TriacTickDelayMax, TriacTickDelayMax, TriacTickDelayMin);
double Kp = 0.1, Ki = 0.05, Kd = 0; //PID control: Specify the links and initial tuning parameters
PID PidController(&PID_CurrentRPM, &PID_Output, &PID_TargetRPM, Kp, Ki, Kd, P_ON_M, DIRECT);

void setup()
{
  pinMode(TargetRPMPin, INPUT);
  pinMode(OnOffButton, INPUT_PULLUP);
  pinMode(OnOffRelay, OUTPUT);
  pinMode(TriacPin, OUTPUT);
  digitalWrite(OnOffRelay, HIGH); ///< Negative logic! - Turns relay OFF
  digitalWrite(TriacPin, LOW);    ///< Turns TRIAC OFF
  TargetRPM = new movingAvg(10);  //Create an average of the last 10 speed potentiometer readings
  TargetRPM->begin();
  TargetRPM->reading(RPMLimitMin); ///< Start from the slowest setting
  updateTargetRPM();
  TachoPulseCounter = 0;
  PidController.SetOutputLimits(TriacTickDelayMin, TriacTickDelayMax);
  PidController.SetMode(AUTOMATIC); //turn the PID on
  Serial.begin(115200);

  // set up Timer1
  OCR1A = 100;   // initialize the comparator
  TIMSK1 = 0x03; // enable comparator A and overflow interrupts
  TCCR1A = 0x00; // timer control registers set for normal operation, timer disabled
  TCCR1B = 0x00; // timer control registers set for normal operation, timer disabled
  attachInterrupt(0, zeroCrossingInterrupt, RISING);
  attachInterrupt(1, tachoTriggerInterrupt, RISING);

  Serial.println(F("Ready... start motor by pressing the OnOffButton"));
}

void loop()
{
  checkSerialInput();

  if (millis() - PIDTimer > 200) //Adjust PID output every 200ms
  {
    calculateRPM();
    if (PIDEnabled && MotorState)
    {
      PidController.Compute();
      Delay = map(PID_Output, TriacTickDelayMin, TriacTickDelayMax, TriacTickDelayMax, TriacTickDelayMin);
    }
    PIDTimer = millis();
  }

  if (millis() - OneSecondTimer > 1000) //Report to serial / update target RPM every second
  {
    OneSecondTimer = millis();
    updateTargetRPM();
    checkOnOffButton();
    Serial.print(F("Target: "));
    Serial.print(PID_TargetRPM);
    Serial.print(F(", Current: "));
    Serial.print(PID_CurrentRPM);
    Serial.print(F(", Delay: "));
    Serial.println(PID_Output);
  }
}

/*
Checks the serial input used to adjust the PID parameters
*/
void checkSerialInput()
{
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
      else if (temp == 'i')
      {
        PIDEnabled = true;
        TargetRPM->reset();
        TargetRPM->reading(RPMLimitMin);
        Serial.println(F("PID enabled"));
      }
      else if (temp == 'o')
      {
        PIDEnabled = false;
        TargetRPM->reset();
        TargetRPM->reading(RPMLimitMin);
        Serial.println(F("PID disabled"));
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
}

void checkOnOffButton()
{
  bool ButtonState = !digitalRead(OnOffButton); ///< Button pin pulled LOW when pressed down
  if (ButtonState != MotorState)                //If there was a change
  {
    if (ButtonState) ///Motor should be on
    {
      Serial.println(F("Starting motor"));
      digitalWrite(OnOffRelay, LOW);
      delay(300); //Wait for the relay to turn on
      MotorState = true;
    }
    else ///Motor should be off
    {
      Serial.println(F("Stoppting motor"));
      MotorState = false;
      delay(40); //Wait 2 AC waves
      digitalWrite(OnOffRelay, HIGH);
    }
  }
}

void updateTargetRPM()
{
  TargetRPM->reading(map(analogRead(TargetRPMPin), 0, 1023, RPMLimitMin, RPMLimitMax));
  PID_TargetRPM = TargetRPM->getAvg();
}

void calculateRPM()
{
  uint32_t ElapsedTime = millis() - LastRPMCalculation;
  PID_CurrentRPM = (double)TachoPulseCounter / ElapsedTime / TachoPulsesPerRevolution * 60000;
  TachoPulseCounter = 0;
  LastRPMCalculation = millis();
}

// Interrupt Service Routines
void zeroCrossingInterrupt()
{                     //AC signal crossed zero: start the delay before turning on the TRIAC
  TCCR1B = Prescale;  // prescale the
  TCNT1 = 0;          // reset timer - count from zero
  OCR1A = Delay; // set the compare register: triggers TIMER1_COMPA_vect when the tick counter reaches the delay calculated by the PID controller
}

ISR(TIMER1_COMPA_vect)
{                 // comparator match: TRIAC delay reached after a zero crossing
  if (MotorState) ///< If the motor should be running
  {
    digitalWrite(TriacPin, HIGH);   // turn on TRIAC gate
    TCNT1 = 65536 - TriacOpenTicks; // trigger pulse width
  }
}

ISR(TIMER1_OVF_vect)
{                              // timer1 overflow
  digitalWrite(TriacPin, LOW); // turn off TRIAC gate
  TCCR1B = 0x00;               // disable timer stops unintended triggers
}

void tachoTriggerInterrupt()
{
  TachoPulseCounter++;
}

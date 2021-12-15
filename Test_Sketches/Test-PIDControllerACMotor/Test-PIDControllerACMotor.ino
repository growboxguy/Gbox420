/*! \file 
 *  \brief     AC motor controller for Arduino Nano
 *  \details   Forward-Backward Direction control
 *  \details   Speed control: TRIAC based leading edge trimmer with zero crossing detection 
 *  \details   PID controller for keeping the same motor RPM under varying load
 *  \author    GrowBoxGuy  - https://sites.google.com/site/growboxguy/
 *  \version   4.20
 */

/********************************************************
 Reading RPM to PID control an AC motor using a TRIAC dimmer with zero cross detection

 Notes on the Universal AC Motor used in this test:
 - 320W max output
 - 220-240V - 50Hz AC
 - 1500 - 12500 RPM range
 - Tacho meter for measuring motor RPM -> 16 pulses/rotation, 26V AC signal, Needs an external comparator with Hysteresis (LM393 for example)

Notes on TRIAC controlling the AC waves:  https://sound-au.com/lamps/dimmers.html
- Using leading edge dimming logic -> beginning of the AC signal is cut off after the zero clossing (ideal for inductive loads like a motor)
- Zero crossing frequency: 
    50Hz AC : 10ms
    60Hz AC : 8.3335ms
- After a slight delay (couple ms) of pulling the TRIAC gate HIGH the signal can be switched LOW -> TRIAC will stay on until the zero crossing point

Notes on Timer1 used to control the TRIAC gate:
- 16bit timer: Max value 65535
- 16MHz frequency: 65535/16000000 = 0.0040959375 -> 4.096ms max delay without prescaler
- TCCR1B Register: Timer1 Prescaler accepts the following values: 0 - Stop timer, 1 - No prescale (max ~4ms before overflow), 2 - /8 prescale (max ~32ms), 3 - /64 prescale, 4 - /256 prescale, 5 - /1024 prescale  https://maxembedded.com/2011/06/avr-timers-timer1/ 
- TCNT1 register: 16bit timer/counter
- Overflows call an interrupt service routine: ISR(TIMER1_OVF_vect)
- Built in comperator to trigger an interrupt ISR(TIMER1_COMPA_vect) after X number of timer ticks
- AC signal period: https://en.wikipedia.org/wiki/Utility_frequency#/media/File:50Hz60Hz.svg
    50Hz : 20ms (Zero crossing every 10ms)
    60Hz : 16.667ms (Zero crossing every 8.3335ms)

 ********************************************************/

#include <PID_v1.h> ///< PID controller

//Physical pins
const uint8_t ZeroCrossingPin = 2;    ///< FIXED pin for incoming Zero Crossing interrupt
const uint8_t ComperatorPin = 3;      ///< FIXED connected to the Output1 pin of an LM393 comperator. LM393 IN1- pin: Motor Tacho cable1, GND PIN: Motor tacho cable2 + Arduino GND, and 5V to IN1+
const uint8_t BackwardPin = 4;        ///< Pin that needs to be pulled LOW to turn on the motor (Motor stops on releasing the button)
const uint8_t ForwardPin = 5;         ///< Pin that needs to be pulled LOW to turn on the motor (Motor stops on pressing the button a second time)
const uint8_t TriacPin = 7;           ///< Signal controlling the TRIAC's gate. When pulled HIGH the TRIAC turns on (and stays on until at least the next zero crossing even if the pin is set to LOW)
const uint8_t TargetRPMPin = A0;      ///< Center leg of a 10kΩ potentiometer connected between GND and 5V. Adjusts the target motor RPM
const uint8_t OnOffRelayPin = A1;     ///< Relay module - IN1 port: controlling AC power going into the TRIAC dimmer circuit - Negative logic: LOW turns relay on
const uint8_t BrushRelayPin = A2;     ///< Relay module - IN2 port: Motor brush relay pin - Direction control - Negative logic: LOW turns relay on
const uint8_t Coil1RelayPin = A3;     ///< Relay module - IN3 port: Motor coil pole 1 relay pin - Direction control - Negative logic: LOW turns relay on
const uint8_t Coil2RelayPin = A4;     ///< Relay module - IN4 port: Motor coil pole 2 relay pin - Direction control - Negative logic: LOW turns relay on

bool FriendlyMessages = true;                ///< Toggle between formatted (idal for reading) or raw (ideal for data logging) serial console outputs
bool MotorState = false;                     ///< true: motor running, false: motor off
bool PIDEnabled = true;                      ///< Enable/disable motor speed stabilization under variable load based on RPM feedback
const double PIDTuningSteps = 0.05;          ///< Increment/decrement PID tuning parameters with this step based on serial input
double Kp = 1;                               ///< Proportional term - PID controller tuning parameter
double Ki = 1;                               ///< Integral term - PID controller tuning parameter
double Kd = 0.01;                            ///< Derivative term - PID controller tuning parameter
const uint8_t TachoPulsesPerRevolution = 16; ///< Tacho meter for measuring motor RPM -> Number of pulses during a single rotation
const uint16_t RPMLimitMin = 1500;           ///< Lowest RPM target - Range of the TargetRPMPin 10kΩ potentiometer
const uint16_t RPMLimitMax = 10000;          ///< Highest RPM target - Range of the TargetRPMPin 10kΩ potentiometer
const int Prescale = 0x02;                   ///< Divides the 16MHz timer by /8 -> 2Mhz : 1 tick every 0.5μs  (16bit counter overflows every 32.7675ms)
const float TicksPerMicroSec = 2.0;          ///< Based on the Prescale, how many ticks are in 1μs
const double TriacDelayMin = 0;              ///< Minimum number of ticks after a zero crossing before turning on the TRIAC: The lower the delay the High the power output (0 turns on the TRIAC right at the zero crossing) - within this limit the PID controller tries to find an ideal delay to reach the target RPM
const double TriacDelayMax = 16000;          ///< Maximum number of ticks after a zero crossing before turning on the TRIAC: Low power output. 16000 ticks -> 8ms . (If set too high motor will not get enough energy to start ) - within this limit the PID controller tries to find an ideal delay to reach the target RPM
const int TriacGateCloseDelay = 20;          ///< Keep the HIGH signal on the TRIAC gate for 20 timer ticks -> 10μs. This gives time for the TRIAC to open. It will keeep conducting until the next zero crossing even after pulling the gate pin LOW

uint32_t PIDTimer = millis();                                                                      ///< Timer for tracking PID re-calculation
uint32_t OneSecondTimer = millis();                                                                ///< Timer for reporting to serial console
uint32_t LastRPMCalculation = millis();                                                            ///< Timestamp of the last RPM calculation
uint32_t LastDebounce = millis();                                                                  ///< Timestamp of the start of the last button press debounce
volatile static long TachoPulseCounter;                                                            ///< Count the total number of tacho pulses, volatile: modified during an interupt (tachoTriggerInterrupt)
double PID_TargetRPM = RPMLimitMin;                                                                ///< RPM the motor should reach
double PID_CurrentRPM = 0;                                                                         ///< Actual RPM of the motor
double PID_OutputDelay = TriacDelayMax;                                                            ///< Output delay after a zero crossing. Each PID calculation updates this value between TriacDelayMin - TriacDelayMax.
PID PidController(&PID_CurrentRPM, &PID_OutputDelay, &PID_TargetRPM, Kp, Ki, Kd, P_ON_E, REVERSE); ///< Initialize PID controller
uint16_t RelayDelay = 300;                                                                         ///< Time in milliseconds needed by the relays to change state
uint8_t DebounceDelay = 50;                                                                        ///< Number of miliseconds to wait for the signal to stabilize after a button press
bool ForwardButtonState = false;                                                                   ///< Tracks current state of the Forward button - Used during debouncing
bool PreviousForwardButtonState = false;                                                           ///< Tracks previous state of the Forward button - Used during debouncing
bool BackwardButtonState = false;                                                                  ///< Tracks current state of the Backward button - Used during debouncing
bool PreviousBackwardButtonState = false;                                                          ///< Tracks previous state of the Backward button - Used during debouncing

void setup()
{
  Serial.begin(115200);
  pinMode(TargetRPMPin, INPUT);
  pinMode(BackwardPin, INPUT_PULLUP); ///< Connect the pin to HIGH over the internal 20kΩ pull-up resistor
  pinMode(ForwardPin, INPUT_PULLUP);  ///< Connect the pin to HIGH over the internal 20kΩ pull-up resistor
  pinMode(OnOffRelayPin, OUTPUT);
  digitalWrite(OnOffRelayPin, HIGH); ///< Negative logic! - Turns relay OFF
  pinMode(BrushRelayPin, OUTPUT);
  digitalWrite(BrushRelayPin, HIGH); ///< Negative logic! - Turns relay OFF
  pinMode(Coil1RelayPin, OUTPUT);
  digitalWrite(Coil1RelayPin, HIGH); ///< Negative logic! - Turns relay OFF
  pinMode(Coil2RelayPin, OUTPUT);
  digitalWrite(Coil2RelayPin, HIGH); ///< Negative logic! - Turns relay OFF
  pinMode(TriacPin, OUTPUT);
  digitalWrite(TriacPin, LOW); ///< Turns TRIAC OFF
  updateTargetRPM();
  TachoPulseCounter = 0;
  PidController.SetOutputLimits(TriacDelayMin, TriacDelayMax);
  PidController.SetMode(AUTOMATIC); //turn the PID on
  OCR1A = 100;                      // Timer1 setup - Initialize the comparator
  TIMSK1 = 0x03;                    // Timer1 setup - Enable comparator A and overflow interrupts
  TCCR1A = 0x00;                    // Timer1 setup - Timer control registers set for normal operation, timer disabled
  TCCR1B = 0x00;                    // Timer1 setup - Timer control registers set for normal operation, timer disabled
  attachInterrupt(digitalPinToInterrupt(ZeroCrossingPin), zeroCrossingInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ComperatorPin), tachoTriggerInterrupt, RISING);
  Serial.println(F("Ready... start motor by pressing the Forward or Backward button"));
}

void loop()
{
  checkSerialInput(); ///< Accept serial inputs for adjusting the PID tuning parameters

  if (millis() - PIDTimer > 200) //Every 200ms: Check button inputs, Adjust PID output
  {
    backwardButton_Momentary();
    forwardButton_Momentary();
    //backwardButton_Latching();
    //forwardButton_Latching();
    calculateRPM();
    if (PIDEnabled && MotorState)
    {
      PidController.Compute();
    }
    PIDTimer = millis();
  }

  if (millis() - OneSecondTimer > 1000) //Every sec: Update the target RPM, Report to serial console
  {
    OneSecondTimer = millis();
    updateTargetRPM();
    if (FriendlyMessages)
    {
      Serial.print(F("Target: "));
    }
    Serial.print(PID_TargetRPM);
    if (FriendlyMessages)
    {
      Serial.print(F("rpm"));
    }
    Serial.print(F(","));
    if (FriendlyMessages)
    {
      Serial.print(F(" Current: "));
    }
    Serial.print(PID_CurrentRPM);
    if (FriendlyMessages)
    {
      Serial.print(F("rpm"));
    }
    Serial.print(F(","));
    if (FriendlyMessages)
    {
      Serial.print(F(" Delay: "));
    }
    Serial.print(PID_OutputDelay / TicksPerMicroSec);
    if (FriendlyMessages)
    {
      Serial.print(F("ms"));
    }
    Serial.println();
  }
}

// Interrupt Service Routines
void zeroCrossingInterrupt() //zero cross detection circuit creates an interrupt every time the AC signal crosses 0V (50Hz: every 10ms, 60Hz: every 8.3335ms)
{                            //AC signal crossed zero: start the delay before turning on the TRIAC
  if (MotorState)            ///< If the motor should be running
  {
    TCCR1B = Prescale;       // prescale the
    TCNT1 = 0;               // reset timer count from zero
    OCR1A = PID_OutputDelay; // set the compare register: triggers ISR(TIMER1_COMPA_vect) when the tick counter reaches the TRIAC delay calculated by the PID controller
  }
}

ISR(TIMER1_COMPA_vect)
{                 // comparator match: TRIAC delay reached after a zero crossing
  if (MotorState) ///< If the motor should be running
  {
    digitalWrite(TriacPin, HIGH);        // turn on TRIAC gate
    TCNT1 = 65536 - TriacGateCloseDelay; // set the timer1 to overflow after TriacGateCloseDelay's worth of timer ticks. Calls ISR(TIMER1_OVF_vect) at the overflow
  }
}

ISR(TIMER1_OVF_vect)
{                              // timer1 overflow
  digitalWrite(TriacPin, LOW); // turn off TRIAC gate
  TCCR1B = 0x00;               // disable timer stops unintended triggers
}

void tachoTriggerInterrupt() ///< Increment counter at every tacho pulse
{
  TachoPulseCounter++;
}

void updateTargetRPM() ///< Reads the 10kΩ potentiometer on TargetRPMPin pin to adjust the desired motor RPM
{
  PID_TargetRPM = roundToTenth(map(analogRead(TargetRPMPin), 0, 1023, RPMLimitMin, RPMLimitMax));
}

int roundToTenth(int Number)
{
  int SmallerMultiple = (Number / 10) * 10;
  int LargerMultiple = SmallerMultiple + 10;
  return (Number - SmallerMultiple > LargerMultiple - Number) ? LargerMultiple : SmallerMultiple;
}

void calculateRPM() ///< Calculate the actual motor RPM based on the number of tachometer pulses received since the last calculation
{
  uint32_t ElapsedTime = millis() - LastRPMCalculation;
  PID_CurrentRPM = (double)TachoPulseCounter / ElapsedTime / TachoPulsesPerRevolution * 60000;
  TachoPulseCounter = 0;
  LastRPMCalculation = millis();
}

void turnMotorOnOff(bool State, bool Direction)
{
  if (State != MotorState)
  {
    if (MotorState) ///If motor is on
    {
      stopMotor();
    }
    else ///If motor is not on
    {
      startMotor(Direction);
    }
  }
}

void startMotor(bool Forward)
{
  if (!MotorState)
  {
    Serial.print(F("Starting motor "));
    if (Forward)
    {
      Serial.println(F("forward"));
      digitalWrite(BrushRelayPin, HIGH);
      digitalWrite(Coil1RelayPin, HIGH);
      digitalWrite(Coil2RelayPin, HIGH);
    }
    else
    {
      Serial.println(F("backward"));
      digitalWrite(BrushRelayPin, LOW);
      digitalWrite(Coil1RelayPin, LOW);
      digitalWrite(Coil2RelayPin, LOW);
    }
    delay(RelayDelay); //Wait for the directon relays to turn on
    digitalWrite(OnOffRelayPin, LOW);
    delay(RelayDelay); //Wait for the on/off relay to turn on
    MotorState = true;
  }
}

void stopMotor()
{
  if (MotorState)
  {
    Serial.println(F("Stoppting motor"));
  }
  MotorState = false;
  delay(RelayDelay); ///< This is a bit overkill here, TRIAC closes faster than a relay
  digitalWrite(OnOffRelayPin, HIGH);
  digitalWrite(BrushRelayPin, HIGH);
  digitalWrite(Coil1RelayPin, HIGH);
  digitalWrite(Coil2RelayPin, HIGH);
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
      if (temp == 'q') //Increase Proportional term  - PID tuning
        Kp += PIDTuningSteps;
      else if (temp == 'a' && Kp > 0) //Decrease Proportional term - PID tuning
      {
        Kp -= PIDTuningSteps;
        if (Kp < 0)
          Kp = 0;
      }
      else if (temp == 'w') //Increase Integral term - PID tuning
        Ki += PIDTuningSteps;
      else if (temp == 's' && Ki > 0) //Decrease Integral term - PID tuning
      {
        Ki -= PIDTuningSteps;
        if (Ki < 0)
          Ki = 0;
      }
      else if (temp == 'e') //Increase Derivative term - PID tuning
        Kd += PIDTuningSteps;
      else if (temp == 'd' && Kd > 0) //Decrease Derivative term - PID tuning
      {
        Kd -= PIDTuningSteps;
        if (Kd < 0)
          Kd = 0;
      }
      else if (temp == 'p') //Turn PID control on
      {
        PIDEnabled = !PIDEnabled;
        if (PIDEnabled)
          Serial.println(F("PID enabled"));
        else
          Serial.println(F("PID disabled"));
      }
      else if (temp == 'o') //Turn PID control off
      {
        PIDEnabled = false;
      }
      else if (temp == 'f') //Turn motor on forward
      {
        turnMotorOnOff(!MotorState, true);
      }
      else if (temp == 'b') //Turn motor on backwards
      {
        turnMotorOnOff(!MotorState, false);
      }
      else if (temp == 'x') //Turn motor off
      {
        turnMotorOnOff(false, false);
      }
      else if (temp == 'c') //Console output formatting
      {
        FriendlyMessages = !FriendlyMessages;
      }
    }
    PidController.SetTunings(Kp, Ki, Kd);
    if (FriendlyMessages)
    {
      Serial.print(F("Kp: "));
    }
    Serial.print(Kp);
    Serial.print(F(","));
    if (FriendlyMessages)
    {
      Serial.print(F(" Ki: "));
    }
    Serial.print(Ki);
    Serial.print(F(","));
    if (FriendlyMessages)
    {
      Serial.print(F(" Kd: "));
    }
    Serial.println(Kd);
  }
}

/*
Forward button with Momentary logic : Motor runs while the button is pressed, stops on button release
*/
void forwardButton_Momentary()
{
  bool NewButtonState = !digitalRead(ForwardPin);   ///< Button pin pulled LOW when pressed down. true: button is pressed down
  if (NewButtonState != PreviousForwardButtonState) //If there was a change
  {
    LastDebounce = millis();
  }
  if (NewButtonState != ForwardButtonState && millis() - LastDebounce > DebounceDelay)
  {
    ForwardButtonState = NewButtonState;
    turnMotorOnOff(ForwardButtonState, true);
  }
  PreviousForwardButtonState = NewButtonState;
}

/*
Forward button with Latching logic : Motor runs once the button is pressed, stops after a second press
*/
void forwardButton_Latching()
{
  bool NewButtonState = !digitalRead(ForwardPin);   ///< Button pin pulled LOW when pressed down. true: button is pressed down
  if (NewButtonState != PreviousForwardButtonState) //If there was a change
  {
    LastDebounce = millis();
  }
  if (NewButtonState != ForwardButtonState && millis() - LastDebounce > DebounceDelay)
  {
    ForwardButtonState = NewButtonState;
    if (ForwardButtonState) //Only toggle on button press down (ignores button release)
    {
      turnMotorOnOff(!MotorState, true);
    }
  }
  PreviousForwardButtonState = NewButtonState;
}

/*
Backward button with Momentary logic : Motor runs while the button is pressed, stops on button release
*/
void backwardButton_Momentary()
{
  bool NewButtonState = !digitalRead(BackwardPin);   ///< Button pin pulled LOW when pressed down. true: button is pressed down
  if (NewButtonState != PreviousBackwardButtonState) //If there was a change
  {
    LastDebounce = millis();
  }
  if (NewButtonState != BackwardButtonState && millis() - LastDebounce > DebounceDelay)
  {
    BackwardButtonState = NewButtonState;
    turnMotorOnOff(BackwardButtonState, false);
  }
  PreviousBackwardButtonState = NewButtonState;
}

/*
Backward button with Latching logic : Motor runs once the button is pressed, stops after a second press
*/
void backwardButton_Latching()
{
  bool NewButtonState = !digitalRead(BackwardPin);   ///< Button pin pulled LOW when pressed down. true: button is pressed down
  if (NewButtonState != PreviousBackwardButtonState) //If there was a change
  {
    LastDebounce = millis();
  }
  if (NewButtonState != BackwardButtonState && millis() - LastDebounce > DebounceDelay)
  {
    BackwardButtonState = NewButtonState;
    if (BackwardButtonState) //Only toggle on button press down (ignores button release)
    {
      turnMotorOnOff(!MotorState, false);
    }
  }
  PreviousBackwardButtonState = NewButtonState;
}
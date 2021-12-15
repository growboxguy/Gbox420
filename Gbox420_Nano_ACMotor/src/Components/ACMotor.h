#pragma once
#include "420Common.h"
#include "420Module.h"
#include "Sound.h"
#include "Switch.h"
#include "PID_v1.h"    ///< PID controller
#include "movingAvg.h" ///< Moving average calculation for Speed adjuster 10kΩ potentiometer

class ACMotor : virtual public Common
{
public:
  ACMotor(const __FlashStringHelper *Name, Module *Parent, Settings::ACMotorSettings *MotorSettings, Settings::RelaySettings *RelaySettings);
  void processTimeCriticalStuff(); ///< Process things that cannot wait or need precise timing
  void refresh_Sec();
  void report(bool FriendlyFormat = false);
  void updateState(ACMotorStates NewState);
  ACMotorStates getState();
  char *getStateText(bool FriendlyFormat = false); ///< Retuns the state of the AC Motor
  void stop();                                     ///< Stop the AC Motor  - Takes time to apply
  void stopRequest();                              ///< Signals to Stop the AC Motor at the next refresh - Runs fast
  void forward();
  void forwardRequest();
  void backward();
  void backwardRequest();
  void calculateRPM();
  void tachoTrigger();
  double getRPM();
  char *getRPMText(bool FriendlyFormat = false);
  void updateTargetRPM(); ///< Read 10kΩ potentiometer
  char *getPID_TargetRPMText(bool FriendlyFormat = false);
  void zeroCrossingInterrupt();

private:
  static volatile long TachoPulseCounter; ///< Count the total number of interrupts
  double PID_CurrentRPM = 0;              ///< Calculated current RPM
  double PID_TargetRPM = 0;               ///< Target RPM - Adjusted by 10k potentiometer
  double PID_Output = 0;                  ///< Inverted! The PID output shows the Active time of the TRIAC
  double Delay = 0;                       ///< Delay after a zero crossing, before turning on the TRIAC. Calculated from PID_Output
  uint32_t LastRPMCalculation = millis(); ///< Timestamp of the last RPM calculation
  uint32_t StateTimer = millis();         ///< Used to measure time spent in a state
  uint32_t LastDebounce = millis();       ///< Timestamp of the start of the last button press debounce
  bool StopRequested = false;             ///< Signals to stop the motor
  bool ForwardRequested = false;          ///< Signals to start the motor in forward direction
  bool BackwardRequested = false;         ///< Signals to start the motor in backward direction
  bool ForwardButtonPressed = false;
  bool BackwardButtonPressed = false;

protected:
  Module *Parent = NULL;
  ACMotorStates State = ACMotorStates::IDLE;
  PID *PidController = NULL;
  bool *PIDEnabled = false;                ///< Enable/disable motor speed stabilization under variable load based on RPM feedback
  movingAvg *TargetRPM = NULL;
  Switch *OnOffSwitch = NULL;               ///< Power intake relay pin - ON/OFF control
  Switch *BrushSwitch = NULL;               ///< Motor brush relay pin - Direction control
  Switch *Coil1Switch = NULL;               ///< Motor coil pole 1 relay pin - Direction control
  Switch *Coil2Switch = NULL;               ///< Motor coil pole 2 relay pin - Direction control
  uint8_t *TargetRPMPin = NULL;             ///< Analog pin connected to the center pin of a 10kΩ potentiometer. Left leg: GND and Right leg: +5V
  uint8_t *ZeroCrossingPin = NULL;          ///< FIXED to Port2: AC dimmer - Zero Corssing pin for interrupt handling
  uint8_t *ComparatorPin = NULL;            ///< FIXED to Port3: External comparator interupt pin for measuring tacho pulses -> needed for RPM counting
  uint8_t *ForwardPin = NULL;               ///< Button pin
  uint8_t *BackwardPin = NULL;              ///< Button pin
  uint8_t *TriacPin = NULL;                 ///< AC dimmer - PWM pin
  double *TriacDelayMin = NULL;             ///< Shortest delay after a zero crossing before turning on the TRIAC: The lower the delay the High the power output
  double *TriacDelayMax = NULL;             ///< Longest delay after a zero crossing before turning on the TRIAC: Low power output. 15000 ticks -> 7.5ms
  uint8_t *TriacGateCloseDelay = NULL;      ///< Keep the HIGH signal on the TRIAC gate for 20 timer ticks -> 10μs
  uint8_t *TachoPulsesPerRevolution = NULL; ///< Shortest delay after a zero crossing before turning on the TRIAC: The lower the delay the High the power output
  uint16_t *RPMLimitMin = NULL;             ///< Target speed when TargetRPMPin potentiometer is at the lowest position
  uint16_t *RPMLimitMax = NULL;             ///< Target speed when TargetRPMPin potentiometer is at the highest position
  double *Kp = NULL;                        ///< PID tuning parameter - proportional gain
  double *Ki = NULL;                        ///< PID tuning parameter - integral gain
  double *Kd = NULL;                        ///< PID tuning parameter - derivative gain
  uint8_t *Prescale = NULL;                 ///< Timer1 Prescaler that divides the 16MHz timer (TCCR1B Register). Accepts the following values: 0 - Stop timer, 1 - No prescale (max ~4ms before overflow), 2 - /8 prescale (max ~32ms), 3 - /64 prescale, 4 - /256 prescale, 5 - /1024 prescale  https://maxembedded.com/2011/06/avr-timers-timer1/
  uint16_t *SpinOffTime = NULL;             ///< (sec) How long it takes for the motor to stop after cutting the power
  uint8_t *DebounceDelay = NULL;            ///< Number of miliseconds to wait for the signal to stabilize after a button press
};
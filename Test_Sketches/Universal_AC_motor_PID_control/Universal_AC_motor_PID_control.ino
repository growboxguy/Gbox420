/// https://sound-au.com/lamps/dimmers.html - Leading edge dimmers (ideal for inductive loads like a motor)


#include <avr/io.h>
#include <avr/interrupt.h>
#include <PID_v1.h>

#define TACHO 3       // tacho signals input pin
#define DETECT 2      // zero cross detect pin
#define GATE 10       // TRIAC gate pin
#define BUTTON 5      // rottary encoder button pin
#define RELAY A4       // relay pin
#define PULSE 2       // number of triac trigger pulse width counts. One count is 16 microseconds
#define TACHOPULSES 8 // number of pulses per revolution

unsigned int RPM;           // real rpm variable
unsigned int TachoCounter;  // tacho pulses count variable
unsigned int lastcount = 0; // additional tacho pulses count variable
unsigned long lastcounttime = 0;
unsigned long lastflash;
unsigned long lastpiddelay = 0;
unsigned long previousMillis = 0;
unsigned long lastDebounceTime = 0;

const int sampleRate = 200;       // Determines how often the PID algorithm evaluates. The default is 200mS. https://playground.arduino.cc/Code/PIDLibrarySetSampleTime/
const int rpmcorrection = 86;     // had to add this parameter to have real RPM equal to desired RPM
const int refreshInterval = 1000; // rpm refresh interval in milliseconds
const int protection = 2000;      // protection will switch on when real rpm exceeds desired by value
const int debounceDelay = 50;     // the debounce time; increase if the output flickers
const int minoutputlimit = 80;    // limit of PID output
const int maxoutputlimit = 540;   // limit of PID output
const int mindimminglimit = 80;   // the shortest delay before triac fires
const int maxdimminglimit = 625;  // for 60Hz will be 520
const int RPMMin = 2000;        // min RPM of the range 2
const int RMPMax = 5000;        // max RPM of the range 2
const int risetime = 100;         // RPM rise time delay in microseconds (risetime x RPM)

int dimming = 540; // this should be the same as maxoutputlimit
int RPMCounter;     // desired RPM counter for range 2
int RPMDesired;
int tempcounter = 100;

byte relayState = HIGH;       // the current state of the relay pin
byte buttonState;            // the current reading from the input pin
byte lastButtonState = HIGH; // the previous reading from the input pin

bool loopflag = false;  // flag for soft start
bool startflag = false; // flag for motor start delay
bool runflag = false;   // flag for motor running state

double Setpoint, Input, Output;       // define PID variables
double sKp = 0.1, sKi = 0.2, sKd = 0; // PID tuning parameters for starting motor
double rKp = 0.25, rKi = 1, rKd = 0;  // PID tuning parameters for runnig motor

PID myPID(&Input, &Output, &Setpoint, sKp, sKi, sKd, DIRECT); // define PID variables and parameters

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing PID controller"));
  // set up pins
  pinMode(BUTTON, INPUT_PULLUP);          // set the button pin
  pinMode(RELAY, OUTPUT);          // set the relay  pin
  pinMode(DETECT, INPUT);          // set the zero cross detect pin
  pinMode(GATE, OUTPUT);           // set the TRIAC gate control pin
  pinMode(TACHO, INPUT);           // set the tacho pulses detect pin
  digitalWrite(RELAY, relayState); // initialize relay output
  RPMCounter = RPMMin; // assign start value for range 2
  Input = 200;          // asiign initial value for PID
  Setpoint = 200;       // asiign initial value for PID

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(minoutputlimit, maxoutputlimit);
  myPID.SetSampleTime(sampleRate); // Sets the sample rate

  // set up Timer1
  OCR1A = 100;   // initialize the comparator
  TIMSK1 = 0x03; // enable comparator A and overflow interrupts
  TCCR1A = 0x00; // timer control registers set for
  TCCR1B = 0x00; // normal operation, timer disabled

  // set up zero crossing interrupt IRQ0 on pin 2.
  // set up tacho sensor interrupt IRQ1 on pin3
  attachInterrupt(0, zeroCrossingInterrupt, RISING);
  attachInterrupt(1, tacho, FALLING);
}

// Interrupt Service Routines
void zeroCrossingInterrupt()
{                  // zero cross detect
  TCCR1B = 0x04;   // start timer with divide by 256 input
  TCNT1 = 0;       // reset timer - count from zero
  OCR1A = dimming; // set the compare register brightness desired.
}

ISR(TIMER1_COMPA_vect)
{ // comparator match
  if (startflag == true)
  {                           // flag for start up delay
    digitalWrite(GATE, HIGH); // set TRIAC gate to high
    TCNT1 = 65536 - PULSE;    // trigger pulse width
  }
}

ISR(TIMER1_OVF_vect)
{                          // timer1 overflow
  digitalWrite(GATE, LOW); // turn off TRIAC gate
  TCCR1B = 0x00;           // disable timer stops unintended triggers
}

// RPM counting routine
void tacho()
{
  TachoCounter++;
  unsigned long tachotime = micros() - lastflash;
  float time_in_sec = ((float)tachotime + rpmcorrection) / 1000000;
  float prerpm = 60 / time_in_sec;
  RPM = prerpm / TACHOPULSES;
  lastflash = micros();
}

void loop()
{

 
  // check the RPM range switch state changes
  if (range != lastRangeState)
  {
    if (range == 0)
    {
      //range0();
      Serial.print(F("Stopping motor..."));      
      runflag = false;
      startflag = false; // flag to turn off triac before relay turns off
      delay(300);        // delay to prevent sparks on relay contacts
      digitalWrite(RELAY, HIGH);
      relayState = HIGH;
      Serial.println(F("done"));
    }    
    if (relayState == HIGH && range != 0)
    {
     Serial.println(F("Ready to start motor"));
    }
  }
  lastRangeState = range;

  // check the start / stop button state
  if (range != 0)
  {
    int reading = digitalRead(BUTTON); // read the state of the switch into a local variable:
    if (reading != lastButtonState)
    {                              // If the switch changed, due to noise or pressing
      lastDebounceTime = millis(); // reset the debouncing timer
    }
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
      if (reading != buttonState)
      { // if the button state has changed:
        buttonState = reading;
        if (buttonState == LOW)
        { // only toggle the relay if the new button state is LOW
          relayState = !relayState;
          if (relayState == LOW)
          {
            loopflag = true;
            digitalWrite(RELAY, LOW); // set the Relay:
            delay(300);                      // delay to prevent sparks on relay contacts
            startflag = true;                // flag to start motor                      
            Serial.println(F("Starting motor"));
          }
          if (relayState == HIGH)
          {
            Setpoint = 200;
            Input = 200;
            runflag = false;
            startflag = false;
            delay(300);                      // delay to prevent sparks on relay contacts
            digitalWrite(RELAY, HIGH); // set the Relay: 
            Serial.println(F("Ready to start motor"));
          }
        }
      }
    }
    lastButtonState = reading; // save the reading. Next time through the loop, it'll be the lastButtonState:
  }


  //soft start
  if (loopflag == true)
  {
    myPID.SetTunings(sKp, sKi, sKd); // Set the PID gain constants and start
    int i = (RPMDesired - tempcounter);
    for (int j = 1; j <= i; j++)
    {
      Input = RPM;
      Setpoint = tempcounter;
      myPID.Compute();
      dimming = map(Output, minoutputlimit, maxoutputlimit, maxoutputlimit, minoutputlimit); // inverse the output
      dimming = constrain(dimming, mindimminglimit, maxdimminglimit);                        // check that dimming is in 20-625 range
      tempcounter++;
      delayMicroseconds(risetime);
    }
    if (tempcounter >= RPMDesired)
    {
      lastcounttime = millis();
      lastpiddelay = millis();
      loopflag = false;
      runflag = true;
      tempcounter = 100;
    }
  }

  // normal motor running state
  if (relayState == LOW && loopflag == false)
  {
    unsigned long piddelay = millis();

    if ((piddelay - lastpiddelay) > 1000)
    {                                  // delay to switch PID values. Prevents hard start
      myPID.SetTunings(rKp, rKi, rKd); // Set the PID gain constants and start
      lastpiddelay = millis();
    }

    Input = RPM;
    Setpoint = RPMDesired;
    myPID.Compute();
    dimming = map(Output, minoutputlimit, maxoutputlimit, maxoutputlimit, minoutputlimit); // reverse the output
    dimming = constrain(dimming, mindimminglimit, maxdimminglimit);                        // check that dimming is in 20-625 range
  }
  // diagnose a fault and turn on protection

  unsigned long counttime = millis();
  if (counttime - lastcounttime >= 1000)
  {
    /*
    if (TachoCounter == 0 && relayState == LOW && runflag == true)
    {
      startflag = false; // flag to turn off triac before relay turns off
      delay(300);        // delay to prevent sparks on relay contacts
      digitalWrite(RELAY, HIGH);
      relayState = LOW;
      Serial.println(F("MOTOR STUCK!"));
    }
    */
    lastcount = TachoCounter;
    TachoCounter = 0;
    lastcounttime = millis();
  }

  //reset rpm after motor stops
  if (TachoCounter == 0 && relayState == HIGH)
  {
    RPM = 0;
  }

/*
  // protection against high rpm. i e triac damage
  if (relayState == LOW && RPM > RPMDesired + protection)
  {
    startflag = false; // flag to turn off triac before relay turns off
    delay(300);        // delay to prevent sparks on relay contacts
    digitalWrite(RELAY, HIGH);
    Serial.println(F("TRIAC DAMAGE!"));
  }
  */

  // real RPM display
  if (relayState == LOW && range != 0)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= refreshInterval)
    {
      previousMillis = currentMillis;
      Serial.print(F("Real RPM: "));
      Serial.println(RPM);
    }
  }
}
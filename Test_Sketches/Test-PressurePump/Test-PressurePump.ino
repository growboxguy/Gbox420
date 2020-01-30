//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Aeroponics tote with directly connected pressure pump (No pressure tank)

//Pins
const byte Relay1OutPin = 52; //Power relay Port 1 - Aeroponics solenoid
const byte Relay2OutPin = 53; //Power relay Port 2 - Aeroponics high pressure pump

//Global variables
byte AeroInterval = 1;              //Aeroponics - Spray every minutes
byte AeroDuration = 10;             //Aeroponics - Spray time in seconds
uint32_t AeroPumpTimeout = 5;       // Aeroponics - Self priming time in seconds
uint32_t AeroSprayTimer = millis(); //Aeroponics - Spray cycle timer - https://www.arduino.cc/reference/en/language/functions/time/millis/
uint32_t AeroPumpTimer = millis();  //Aeroponics - Pump cycle timer
bool AeroSolenoidOn = false;        //Aeroponics - Spray state, set to true to spay at power on
bool AeroPumpOn = false;            //Aeroponics - High pressure pump state

void setup()
{                        // put your setup code here, to run once:
  Serial.begin(115200);  //2560mega console output
  Serial3.begin(115200); //ESP console output

  pinMode(Relay1OutPin, OUTPUT);
  pinMode(Relay2OutPin, OUTPUT);
  digitalWrite(Relay1OutPin, HIGH); //default OFF (Uses negative logic - HIGH turns relay off, LOW on)
  digitalWrite(Relay2OutPin, HIGH);

  logToSerials(F("Starting aero cycle. Spraying every "), false);
  logToSerials(AeroInterval, false);
  logToSerials(F(" minutes, for  "), false);
  logToSerials(AeroDuration, false);
  logToSerials(F(" seconds."), true);
}

void loop()
{ // put your main code here, to run repeatedly:
  checkAero();
  delay(500); //delay 15 seconds
}

void checkAero()
{
  checkAeroSprayTimer();
}

void checkAeroSprayTimer()
{
  if (AeroPumpOn)
  { //if pump is on
    if (!AeroSolenoidOn && millis() - AeroSprayTimer >= AeroDuration * 1000)
    { //bypass valve is closed and time to stop spraying (AeroDuration in Seconds)
      AeroPumpOn = false;
      checkRelays();
      logToSerials(F("Stopping spray"), true);
      AeroSprayTimer = millis();
    }
    else
    {
      if (AeroSolenoidOn && millis() - AeroPumpTimer >= AeroPumpTimeout * 1000)
      {                         //self priming timeout reached, time to start spraying
        AeroSolenoidOn = false; //Close bypass valve
        checkRelays();
        logToSerials(F("Closing bypass, starting spray"), true);
        AeroSprayTimer = millis();
      }
    }
  }
  else
  { //pump is off
    if (millis() - AeroSprayTimer >= AeroInterval * 60000)
    { //if time to start spraying (AeroInterval in Minutes)
      AeroPumpOn = true;
      AeroSolenoidOn = true; //Open bypass valve
      logToSerials(F("Bypass valve opened, priming pump"), true);
      AeroPumpTimer = millis();
      checkRelays();
    }
  }
}

void checkRelays()
{
  if (AeroSolenoidOn)
    digitalWrite(Relay1OutPin, LOW);
  else
    digitalWrite(Relay1OutPin, HIGH); //True turns relay ON , False turns relay OFF  (LOW signal activates Relay)
  if (AeroPumpOn)
    digitalWrite(Relay2OutPin, LOW);
  else
    digitalWrite(Relay2OutPin, HIGH);
}

template <class logLine> //fuction template: logToSerials can take any parameter type (int,float,bool,char..) https://en.cppreference.com/w/cpp/language/function_template
void logToSerials(logLine ToPrint, bool breakLine)
{
  if (breakLine)
  {
    Serial.println(ToPrint);
    Serial3.println(ToPrint);
  }
  else
  {
    Serial.print(ToPrint);
    Serial3.print(ToPrint);
  }
}

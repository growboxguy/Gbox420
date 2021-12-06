//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing external comperator interrupts

const byte InterruptPin = 3;   ///< Nanno supports external interrupts on port D2 and D3  (https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)
const byte TachoPulsesPerRevolution = 1; //Number of pulses sent for each full rotation (used to calculate the Motor RPM)
volatile boolean Triggered = false;   //used to check if the interrupt has been raised
volatile uint16_t TriggerCounter = 0; //Count the total number of interrupts
uint16_t LastTriggerCounter = 0; //for calculating pulses / second
uint32_t LastCalculationTimer = millis(); //for calculating pulses / second

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Sketch for testing: external comperator interupts"));
  pinMode(InterruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(InterruptPin), trigger, RISING);
}

void loop()
{
  delay(1000); //every seconds
  checkTrigger();  
}

void checkTrigger()
{
  uint32_t ElapsedTime = millis() - LastCalculationTimer;
  if (Triggered)
  {
    Triggered = false;
    Serial.print(F("Comparator triggered. Counter: "));
    Serial.print(TriggerCounter);
    double TriggersPerSec = (double)(TriggerCounter-LastTriggerCounter) / ElapsedTime * 1000;
    if(TriggersPerSec > 1)
    {     
    Serial.print(F(" ["));
    Serial.print(TriggersPerSec);
    Serial.print(F(" trigger/sec, "));
    Serial.print(TriggersPerSec / TachoPulsesPerRevolution * 60);
    Serial.print(F(" RPM]"));
    }
    Serial.println();
  }
  LastTriggerCounter = TriggerCounter;
  LastCalculationTimer = millis();
}

void trigger()
{
  Triggered = true;
  TriggerCounter++;
}

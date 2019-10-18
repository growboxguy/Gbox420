//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Flow sensor for detecting liquid throughput

//Pins
const byte FlowMeterInPin = 2;     //Signal(yellow) - water flow sensor

//Global constants
const byte SampleSize = 10;

//Global variables
volatile unsigned int FlowPulseCount = 0;
byte SampleCounter = 0;
unsigned int FlowTotalPulses = 0;

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(FlowMeterInPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FlowMeterInPin), flowPulseCounter, FALLING);  //Mega2560 support interrupts on port 2, 3, 18, 19, 20, 21
  Serial.println("Measure a fix amount (e.g.:1L) of liquid through the sensor, sketch will measure the pulse count");
  Serial.println("Press 'Enter' to store the measured value");
}

void loop() {  // put your main code here, to run repeatedly:
  Serial.println(FlowPulseCount); //Displays how many pulses were received
  delay(2000); //does not block interrupts
  if(Serial.available()){
        while(Serial.read() > -1); //dump the input
        FlowTotalPulses += FlowPulseCount;
        Serial.print("Test ");Serial.print(++SampleCounter);Serial.print("/");Serial.print(SampleSize);Serial.print(" - ");Serial.println(FlowPulseCount);
        FlowPulseCount = 0;
        if(SampleCounter == SampleSize){
          float Average = (float)FlowTotalPulses/SampleSize;
          Serial.print("Tests completed, total pulses: ");Serial.print(FlowTotalPulses);Serial.print(", average: ");Serial.println(Average);
          FlowTotalPulses=0;
          SampleCounter=0;
        }
  }
}

void flowPulseCounter()
{
  FlowPulseCount++;
}

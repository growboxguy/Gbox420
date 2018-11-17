//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Moisture sensor and calibrating min/max readings

//Pins
int AMoistureSensorInPin = A8; 
int DMoistureSensorInPin = 45; 

//Global variables
bool isWet = false; //Moisture content reaches pre-set threshold
int Moisture ; //Reading of moisture content
int MoisturePercentage ; //Moisture content relative to max,min values measued
int MaxMoisture = 0; // Stores the highest moisture reading
int MinMoisture = 1023; //Stores the lowest moisture reading

void setup() {
  Serial.begin(115200);
  }

void loop() {
  Moisture= analogRead(AMoistureSensorInPin);
  isWet = !digitalRead(DMoistureSensorInPin);
  if(Moisture > MaxMoisture) MaxMoisture = Moisture;
  if(Moisture < MinMoisture) MinMoisture = Moisture;  
  MoisturePercentage = map(Moisture,MaxMoisture,MinMoisture,100,0);
  
  Serial.print("Moisture : ");
  Serial.print(Moisture);
  Serial.print(" (");
  Serial.print(MoisturePercentage);
  Serial.print("%),");
  Serial.print("\tMin: ");
  Serial.print(MinMoisture);
  Serial.print(",\tMax: ");
  Serial.print(MaxMoisture);
  Serial.print(F(",\tThreshold: ")); if(isWet) Serial.println(F("Reached"));else Serial.println(F("Not reached"));
  
  delay(2000);
  }

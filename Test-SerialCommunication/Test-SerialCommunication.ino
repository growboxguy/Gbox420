//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Arduino and ESP-link firmware communication

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200);    //2560mega console output
  Serial3.begin(115200);  //wifi console output
}

void loop() {  // put your main code here, to run repeatedly:
  Serial.println("Marko");
  Serial3.println("Polo");
  delay(5000); 
}

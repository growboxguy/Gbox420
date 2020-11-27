//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: 1 channel relay module
//Note: The single channel relays tend to use positive logic, where: HIGH = relay ON, LOW = relay OFF

//Pins
const byte Relay1OutPin = 5; //Power relay Port 1

void setup()
{ // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println(F("Sketch for testing: 1 channel relay module"));

    pinMode(Relay1OutPin, OUTPUT);
    digitalWrite(Relay1OutPin, LOW); //Start in off state
}

void loop()
{ // put your main code here, to run repeatedly:
    Serial.println(F("  Turning Relay1 ON"));
    digitalWrite(Relay1OutPin, HIGH);
    delay(10000);
    Serial.println(F("  Turning Relay2 OFF"));
    digitalWrite(Relay1OutPin, LOW);
    Serial.println(F("The Cycle is complete, restarting in 10 sec..."));
    delay(10000);
}

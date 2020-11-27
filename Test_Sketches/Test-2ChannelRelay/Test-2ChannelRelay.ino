//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: 2 channel relay module
//Note: Multi channel relays tend to use negative logic, where: HIGH = relay OFF, LOW = relay ON

//Pins
const byte Relay1OutPin = 4; //Power relay Port 1
const byte Relay2OutPin = 3; //Power relay Port 2

void setup()
{ // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println(F("Sketch for testing: 2 channel relay module"));

    pinMode(Relay1OutPin, OUTPUT);
    pinMode(Relay2OutPin, OUTPUT);

    turnAllRelaysOff(); //Relays start in an undefined state, need to specify startup state

    Serial.println(F("Turning all relays on..."));
    Serial.println(F("  Turning Relay1 ON"));
    digitalWrite(Relay1OutPin, LOW);
    delay(10000);
    Serial.println(F("  Turning Relay2 ON"));
    digitalWrite(Relay2OutPin, LOW);
    Serial.println(F("All relays are active, starting cycle test in 10 sec..."));
    delay(10000);
    turnAllRelaysOff();
}

void loop()
{ // put your main code here, to run repeatedly:
    Serial.println(F("  Turning Relay1 ON"));
    digitalWrite(Relay1OutPin, LOW);
    delay(10000);
    Serial.println(F("  Turning Relay2 ON"));
    digitalWrite(Relay1OutPin, HIGH);
    digitalWrite(Relay2OutPin, LOW);
    delay(10000);
    digitalWrite(Relay2OutPin, HIGH);
    Serial.println(F("The Cycle is complete, restarting in 10 sec..."));
    delay(10000);
}

void turnAllRelaysOff()
{
    Serial.println(F("Turning all relays off..."));
    digitalWrite(Relay1OutPin, HIGH);
    digitalWrite(Relay2OutPin, HIGH);
}

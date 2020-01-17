//This is a test for matching the HTML object name to an instance of a class

const __FlashStringHelper * Name;  //stores the name of the instance, like: Light1, Aeroponics_Tank1, InternalDHTSensor
static const byte MaxTextLength = 32;  //Default char* buffer size: 31 characters + null terminator, used for logging and converting to text
char ShortMessage[MaxTextLength]; //buffer for storing a text in the Gbox420 sketch


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Name = F("Light1");
  char const * lookupName = "Light1_TimerEnabled";
  isThisMyComponent(lookupName,ShortMessage) ? Serial.println(F("Match!!!")) : Serial.println(F("No match"));
  Serial.print(F("Content of ShotMessage buffer: ")); Serial.println(ShortMessage);
  Serial.println();
  char const * lookupName1 = "Aeroponics_Tank1_Pressure";
  isThisMyComponent(lookupName1,ShortMessage) ? Serial.println(F("Match!!!")) : Serial.println(F("No match"));
  Serial.print(F("Content of ShotMessage buffer: ")); Serial.println(ShortMessage);
  Serial.println();
  char const * lookupName2 = "Light2_On";
  isThisMyComponent(lookupName2,ShortMessage) ? Serial.println(F("Match!!!")) : Serial.println(F("No match"));
  Serial.print(F("Content of ShotMessage buffer: ")); Serial.println(ShortMessage);
  Serial.println();
  char const * lookupName3 = "Light1_OnTime";
  isThisMyComponent(lookupName3,ShortMessage) ? Serial.println(F("Match!!!")) : Serial.println(F("No match")); 
  Serial.print(F("Content of ShotMessage buffer: ")); Serial.println(ShortMessage); 
}

void loop() {
  // put your main code here, to run repeatedly:
}

bool isThisMyComponent(char const * lookupName, char * outFunctionName){  //this is the name of the web component that triggered the action, in the form of: InstanceName_functionName . Examles: Light1_On , Light1_OnTime, LightSensor1_ReadingRaw from GrowBox.html 
  Serial.print("Component :");
  Serial.println(lookupName);
  Serial.print("Object :");
  Serial.println(Name); 

  byte CharacterCount = 0;
  char FlashCurrentChar;
  char RAMCurrentChar;

  const char * FlashAddressPointer = (const char PROGMEM *)Name;
  while (1) {
    FlashCurrentChar = pgm_read_byte(FlashAddressPointer++);  //read back from the memory address on character, and then increment the pointer to the next char
    RAMCurrentChar = lookupName[CharacterCount++];  //
    if (FlashCurrentChar == 0) break;  //if we have reached the string termination sign. ( null terminator is the numerical value 0, sometimes also marked as '\0')   
    Serial.print(FlashCurrentChar);
    Serial.print(" ");
    Serial.print(RAMCurrentChar);
    Serial.print(" - ");    
    if(FlashCurrentChar==RAMCurrentChar){
      Serial.println(F("Match"));
    }
    else{
      Serial.println(F("Different"));
      return false; //stop the while loop
    }
}
  if(FlashCurrentChar == 0 && RAMCurrentChar == '_'){  //if instance name is confirmed: continue reading the HTML component name describing its function
    Serial.println(F("Object name matched, extracting HTML object function name"));
    int SafetyCount = 0;
    while (1) {
      RAMCurrentChar = lookupName[CharacterCount++];  //      
      Serial.print(RAMCurrentChar);
      *outFunctionName++ = RAMCurrentChar;
      if(RAMCurrentChar == 0) break;  //if we have reached the string termination sign. ( null terminator is the numerical value 0, sometimes also marked as '\0')   
      if(SafetyCount > MaxTextLength){
       Serial.println(F("Exceeding length limit!"));
       return false; 
      }      
    }
    Serial.println();
    return true;    
  }
  else{  
    Serial.println(F("Object name did not match"));
    return false;
  }  
}

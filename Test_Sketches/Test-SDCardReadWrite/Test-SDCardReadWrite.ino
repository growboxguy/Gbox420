//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: Writing and reading back values from SD card

//Libraries
#include "SPI.h"
#include "SD.h"

//Pins
const byte SDCardMISOOutPin = 50; //SD Card - MISO
const byte SDCardMOSIOutPin = 51; //SD Card - MOSI
const byte SDCardSCKOutPin = 52; //SD Card - SCK
const byte SDCardCSOutPin = 53; //SD Card - CS

//Global variables - test values to write and read back
File TestFile;
char* TestText = "DIY GrowBox";
int TestInt = 420;
float TestFloat = 4.20;
bool TestBool = true;

void setup() {
  Serial.begin(115200);
  pinMode(SDCardMISOOutPin, INPUT); //SD library does this too, not needed
  pinMode(SDCardMOSIOutPin, OUTPUT); //SD library does this too, not needed
  pinMode(SDCardSCKOutPin, OUTPUT); //SD library does this too, not needed
  pinMode(SDCardCSOutPin, OUTPUT);
   
  unsigned long ResponseTimer = millis();  //store the number of milliseconds since the program started, https://www.arduino.cc/reference/en/language/functions/time/millis/
  while (!SD.begin(SDCardCSOutPin)) {
    Serial.print(".");
    delay(500);
    if(millis() - ResponseTimer >= 25000){
      Serial.println(F("Error: SD card timeout"));
      break; }
  }
  SaveToSD();
  ReadFromSD();
}

void loop(void) {  
}

char * floatToChar(float Number){
static char ReturnFloatChar[8] = "";
dtostrf(Number, 4, 2, ReturnFloatChar); //4 digits + decimal sign + 2 decimals + null terminator, 9999.99 max that can be passed
return ReturnFloatChar;
}

void SaveToSD(){
  Serial.println(F("Saving test.txt to SD Card"));
   if (SD.exists("test.txt")) {
    Serial.println(F("Removing old test.txt"));
    SD.remove("test.txt");
  }
  TestFile = SD.open("test.txt", FILE_WRITE);
  if (TestFile) {
    TestFile.print("TestText=");TestFile.println(TestText); 
    TestFile.print("TestInt=");TestFile.println(TestInt); 
    TestFile.print("TestBool=");TestFile.println(TestBool);
    TestFile.print("TestFloat=");TestFile.println(TestFloat);  
    TestFile.close();
    Serial.println(F("test.txt saved"));
  } else {
    Serial.println(F("Error writing test.txt"));
  }
}

void ReadFromSD(){
  Serial.println(F("Loading from test.txt"));
  TestFile = SD.open("test.txt");
  if (TestFile) {
    Serial.println(F("Raw test.txt:"));    
    while (TestFile.available()) {Serial.write(TestFile.read());   } // read to end
    TestFile.close();

  Serial.println();
  Serial.println(F("Processed values:")); 
  char buffer[40]; // May need to be a bit bigger if you have long variables
  byte index = 0;
  TestFile = SD.open("test.txt");
 
  while (TestFile.available()) {
        char c = TestFile.read();
         if(c == '\n') // Test for <cr> and <lf>
         {
             parseAndSave(buffer);
             index = 0;
         }
         else
         {
             buffer[index++] = c;             
         }
      buffer[index] = '\0'; // Keep buffer NULL terminated      
    }
   TestFile.close();
  } else {
    Serial.println(F("Error opening test.txt"));
  }
    
}

void parseAndSave(char *buff)
{
   char *name = strtok(buff, "="); //split buffer at =
   if(name)
   {
    char *value = strtok(NULL, "="); //NULL signals to get the next section
    if(value)
    {
      Serial.print(name);Serial.print("=");
      if(strcmp(name,"TestText") == 0){TestText = value;Serial.println(TestText);} 
      else if(strcmp(name,"TestInt") == 0){TestInt = atoi(value);Serial.println(TestInt);} 
      else if(strcmp(name, "TestBool") == 0){TestBool = atoi(value); Serial.println(TestBool);} //bool is based on a uint8
      else if(strcmp(name, "TestFloat") == 0){TestFloat = atof(value);Serial.println(TestFloat);};     
    }        
   }  
}

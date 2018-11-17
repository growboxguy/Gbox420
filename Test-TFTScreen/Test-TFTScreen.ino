//GrowBoxGuy - http://sites.google.com/site/growboxguy/
//Sketch for testing: TFT screen

//Libraries
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

//Pins
const byte ScreenReset = 37; //RESET(3.3V) - Screen Screen
const byte ScreenSCK = 38;  //SCK(3.3V) - Screen Screen
const byte ScreenMOSI = 39; //SDO/MOSI(3.3V) - Screen Screen
const byte ScreenMISO = 40; //SDI/MISO(3.3V), not used - Screen Screen 
const byte ScreenCS = 41;  //CS(3.3V) - Screen Screen
const byte ScreenDC = 42; //DC/RS(3.3V) - Screen Screen

//Component initialization
Adafruit_ILI9341 Screen = Adafruit_ILI9341(ScreenCS, ScreenDC, ScreenMOSI, ScreenSCK, ScreenReset, ScreenMISO);

void setup() {  // put your setup code here, to run once:
  Serial.begin(115200); 
  Screen.begin();
  
  // read diagnostics (optional but can help debug problems)
  Serial.println("Screen self diagnostics:");
  uint8_t x = Screen.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = Screen.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = Screen.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = Screen.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = Screen.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
}

void loop() {  // put your main code here, to run repeatedly:
//for(uint8_t rotation=0; rotation<4; rotation++) {
    //Serial.print("Screen rotation: "); Serial.println(rotation);
    Screen.setRotation(3);
    testText();
    delay(50000);
 // }
}

void testText() {
  Screen.fillScreen(ILI9341_BLACK);
  Screen.setCursor(0, 0);
  Screen.setTextColor(ILI9341_GREEN);
  Screen.setTextSize(4);
  Screen.println("GrowBox Stats");
  Screen.setTextColor(ILI9341_WHITE);  
  Screen.setTextSize(1);
  Screen.println("sites.google.com/site/growboxguy"); 
  Screen.setTextColor(ILI9341_RED); 
  Screen.setTextSize(2);
  Screen.println("DIY Grow Box guide");
  Screen.setTextSize(1);
  Screen.setTextColor(ILI9341_YELLOW);
  Screen.println("Indoor gardening with Arduino");
}

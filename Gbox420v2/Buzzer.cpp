#include "Buzzer.h" 

Buzzer::Buzzer(byte Pin, bool *Enabled){
  this -> Pin = Pin;
  this -> Enabled = Enabled;
  pinMode(Pin, OUTPUT);
  pinMode(13, OUTPUT); //onboard LED
  if(MySettings.DebugEnabled){logToSerials(F("Buzzer object created"),true);}
}

void Buzzer::refresh(){
  if(*Enabled){  
    if (PlayOnSound)  {PlayOnSound = false;OnSound();}
    if (PlayOffSound)  {PlayOffSound = false;OffSound();} 
  }
  if (PlayEE)  {PlayEE = false;EE();} 
}

void Buzzer::playOnSound(){
  PlayOnSound = true;
}

void Buzzer::playOffSound(){
  PlayOffSound = true;
} 

void Buzzer::OnSound(){
  if(Enabled){ 
  tone(Pin,500);
  delay(100);
  noTone(Pin);
  tone(Pin,1000);
  delay(100);
  noTone(Pin);
  }
}

void Buzzer::OffSound(){
  if(*Enabled){ 
  tone(Pin,1000);
  delay(100);
  noTone(Pin);
  tone(Pin,500);
  delay(100);
  noTone(Pin);
  }
}

void Buzzer::setSoundOnOff(bool State){
  Enabled = &State;
  if(*Enabled){ 
    addToLog(F("Sound enabled"));
    PlayOnSound=true;}
  else {
    addToLog(F("Sound disabled"));
    PlayOffSound=true;
    }
}


//EE Section

void Buzzer::playEE(){
  PlayEE = true;
}

const PROGMEM static int Buzzer::melody[] = {   //https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
  2637, 2637, 0, 2637,
  0, 2093, 2637, 0,
  3136, 0, 0,  0,
  1568, 0, 0, 0,

  2093, 0, 0, 1568,
  0, 0, 1319, 0,
  0, 1760, 0, 1976,
  0, 1865, 1760, 0,

  1568, 2637, 3136,
  3520, 0, 2794, 3136,
  0, 2637, 0, 2093,
  2349, 1976, 0, 0,

  2093, 0, 0, 1568,
  0, 0, 1319, 0,
  0, 1760, 0, 1976,
  0, 1865, 1760, 0,

  1568, 2637, 3136,
  3520, 0, 2794, 3136,
  0, 2637, 0, 2093,
  2349, 1976, 0, 0,
  
  0, 0, 0,
  
  262, 523, 220, 440,
  233, 466, 0,
  0,
  262, 523, 220, 440,
  233, 466, 0,
  0,
  175, 349, 147, 294,
  156, 311, 0,
  0,
  175, 349, 147, 294,
  156, 311, 0,
  0, 311, 277, 294,
  277, 311,
  311, 208,
  196, 277,
  262, 370, 349, 165, 466, 440,
  415, 311, 247,
  233, 220, 208
};

const PROGMEM static byte Buzzer::tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
    
  30, 30, 30,
  
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10  
};

void Buzzer::EE() { 
  addToLog(F("♬Easter egg♬"));
  //int size = sizeof(melody) / sizeof(int);  //this is equal 134: No sense calculating it every time, if you change the melody could come handy.
  for (int thisNote = 0; thisNote < 134; thisNote++) {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / (byte)pgm_read_word(&tempo[thisNote]);  //tempo is stored in PROGMEM (Flash), cannot read from it as RAM array (temp[thisNote] would not work) //https://forum.arduino.cc/index.php?topic=106603.0
    buzz((int)pgm_read_word(&melody[thisNote]), noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(noteDuration);      
    // stop the tone playing:
    buzz(0, noteDuration); 
    wdt_reset(); //Reset Watchdog timeout to avoid Arduino reseting while playing the song   
  }
}

void Buzzer::buzz( uint32_t frequency, uint32_t length) {
  digitalWrite(13, HIGH);
  uint32_t delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  uint32_t numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (uint32_t i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(Pin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(Pin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }  
  digitalWrite(13, LOW);
}

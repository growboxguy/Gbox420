
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico_tone.hpp"


int main() {
	
// Inspired thanks to Michal Choma, Slovakya; and Jana Karel Švaříčková, Slovakya (arduino version). 
	
int melody[] = {
	// notes of the melody followed by the duration.
	// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
	// !!negative numbers are used to represent dotted notes,
	// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!

  // Mafia theme
  // Score available at https://musescore.com/user/2139361/scores/783461
  
  NOTE_D4, 4, NOTE_F4, 4, NOTE_E4, 4, NOTE_D4, 4, //1
  NOTE_D4, 4, NOTE_A3, 4, NOTE_G3, 4, NOTE_C4, 4, //2
  NOTE_D4, 4, NOTE_F4, 4, NOTE_E4, 4, NOTE_G4, 4, //3
  NOTE_A4, 1, //4
  NOTE_A4, 4, NOTE_C5, 4, NOTE_G4, 2, //5
  NOTE_G4, -4, NOTE_E4, 8, NOTE_D4, 4, NOTE_C4, 8, NOTE_E4, 16, NOTE_F4, 16, //6
  NOTE_E4, 4, NOTE_C4, 4, NOTE_D4, 2, //7
  
  NOTE_D4, -4, NOTE_C4, 8, NOTE_A3, 4, NOTE_G3, 4, //8
  NOTE_A5, 4, NOTE_C6, 4, NOTE_G5, 2,  //9
  NOTE_G5, -4, NOTE_E5, 8, NOTE_D5, 4, NOTE_C5, 8, NOTE_E5, 16, NOTE_F5, 16, //10
  NOTE_E5, 4, NOTE_C5, 4, NOTE_D5, 2, //11
  NOTE_D5, -4, NOTE_C5, 8, NOTE_A4, 4, NOTE_G4, 4, //12
  NOTE_D6, 2, NOTE_D6, 8, NOTE_F5, 8, NOTE_A5, 8, NOTE_D6, 8, //13

  NOTE_G5, 2, NOTE_C6, 8, NOTE_AS5, 4, NOTE_A5, 16, NOTE_AS5, 16, //14
  NOTE_A5, 2, NOTE_E5, 8, NOTE_F5, 8, NOTE_E5, 8, NOTE_C5, 8, //15
  NOTE_D5, 2, NOTE_E5, 8, NOTE_F5, 8, NOTE_E5, 8, NOTE_C5, 8, //16
  NOTE_D6, 2, NOTE_D5, 8, NOTE_F4, 8, NOTE_A4, 8, NOTE_D5, 8, //17
  NOTE_E5, 8, NOTE_F5, 8, NOTE_C5, -2, //18

  NOTE_D5, 8, NOTE_DS5, 8, NOTE_AS4, -4, NOTE_G4, 8, NOTE_F4, 8, NOTE_DS4, 8, //19
  NOTE_AS4, 8, NOTE_B4, 8, NOTE_DS4, -2, //20
  NOTE_G4, -8, NOTE_GS4, 16, NOTE_GS4, -4, NOTE_G4, -8, NOTE_GS4, 8, //21 -1/16
  NOTE_AS4, -8, NOTE_B4, 16, NOTE_B4, 2, NOTE_C5, 4, //22
  NOTE_G4, -8, NOTE_F4, 16, NOTE_F4, -4, NOTE_G4, -8, NOTE_GS4, 8, //23 -1/16

  NOTE_E4, -2, NOTE_AS4, 8, //24 -1/8
  NOTE_B4, 8, NOTE_AS4, 16, NOTE_B4, 8, NOTE_AS4, 16, NOTE_B4, 2, //25 -1/8
  NOTE_AS4, -8, NOTE_B4, 2, NOTE_AS4, -8, NOTE_B4, 8, //26
  NOTE_CS5, -8, NOTE_D5, 2, NOTE_DS5, 4, //27 -1/16
  NOTE_AS4, -8, NOTE_GS4, 2, NOTE_AS4, -8, NOTE_B4, 8, //28
  NOTE_G4, -2, NOTE_CS5, 8, //29 -1/8

  NOTE_D5, 8, NOTE_CS5, 16, NOTE_D5, 8, NOTE_CS5, 16, NOTE_D5, 2, //30 -1/8
  NOTE_CS5, -16, NOTE_E5, -16, NOTE_DS5, -8, NOTE_C6, 32, NOTE_A5, 32, NOTE_B5, 16, NOTE_B5, -16, NOTE_D6, -16, NOTE_CS6, 8, NOTE_A5, -8, //31
  NOTE_CS5, -16, NOTE_E5, -16, NOTE_DS5, -8, NOTE_C6, 32, NOTE_A5, 32, NOTE_B5, 16, NOTE_B5, -16, NOTE_D6, -16, NOTE_CS6, 8, NOTE_A5, -8, //32
  
  NOTE_CS5, -16, NOTE_E5, -16, NOTE_DS5, -8, NOTE_C6, 32, NOTE_A5, 32, NOTE_B5, 16, NOTE_B5, -16, NOTE_D6, -16, NOTE_CS6, 8, NOTE_A5, -8, //33
  NOTE_CS5, -16, NOTE_E5, -16, NOTE_DS5, -8, NOTE_C6, 32, NOTE_A5, 32, NOTE_B5, 16, NOTE_B5, -16, NOTE_D6, -16, NOTE_CS6, 8, NOTE_A5, -8, //34

  NOTE_D5, -4, NOTE_C5, 8, NOTE_D5, 8, NOTE_F4, 8, NOTE_A4, 8, NOTE_D5, 8, //35
  NOTE_G4, 2, NOTE_C5, 8, NOTE_AS4, 4, NOTE_A4, 16, NOTE_AS4, 16, //36

  NOTE_A4, 4, NOTE_AS4, 4, NOTE_E4, 8, NOTE_F4, 8, NOTE_E4, 8, NOTE_C4, 8, //37
  NOTE_D4, 2, NOTE_E5, 8, NOTE_F5, 8, NOTE_E5, 8, NOTE_C5, 8, //38

  NOTE_D5, -4, NOTE_C5, 8, NOTE_D5, 8, NOTE_F4, 8, NOTE_A4, 8, NOTE_D5, 8,  //39
  NOTE_E5, 8, NOTE_F5, 8, NOTE_C5, 2, //40

  NOTE_D5, 8, NOTE_DS5, 8, NOTE_AS4, -4, NOTE_G4, 8, NOTE_F4, 8, NOTE_DS4, 8, //41
  NOTE_AS4, 4, NOTE_B4, 4, NOTE_DS4, 2, //42
  NOTE_C5, -4, NOTE_AS4, 8, NOTE_GS4, 4, NOTE_G4, 4, //43
  
  NOTE_C5, -4, NOTE_AS4, 8, NOTE_F4, -4, NOTE_DS4, 8, //44
  NOTE_AS4, -4, NOTE_GS4, 8, NOTE_G4, 4, NOTE_GS4, 4, //45
  NOTE_F4, 1, //46
  NOTE_C5, -4, NOTE_AS4, 8, NOTE_GS4, 4, NOTE_G4, 4, //47
  NOTE_C5, -4, NOTE_AS4, 8, NOTE_F4, -4, NOTE_DS4, 8, //48
  NOTE_AS4, -4, NOTE_GS4, 8, NOTE_G4, 4, NOTE_GS4, 4, //49
  NOTE_F4, 1, //50
};

	Tone myPlayer(15,50,0,20,0,20,0,10);
	myPlayer.init(TONE_NON_BLOCKING) ;

	
	while (true) {

	myPlayer.play_melody(T_ADAGIO,sizeof(melody)/(sizeof(melody[0])*2),melody);

	sleep_ms(4000);

	};
    return 0;
}

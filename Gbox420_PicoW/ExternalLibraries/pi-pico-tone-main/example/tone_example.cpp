
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico_tone.hpp"


int main() {
	
	int melody[] = {NOTE_C4,4,  NOTE_G3,8,  NOTE_G3,8,  NOTE_A3,4,  NOTE_G3,4, 0,4,  NOTE_B3,4,  NOTE_C4,4};

	Tone myPlayer(15,50,0,20,0,20,0,10);
	Tone myPlayer2(14) ;
	myPlayer.init(TONE_NON_BLOCKING) ;
	myPlayer2.init(TONE_NON_BLOCKING) ;
	
	while (true) {

	myPlayer.play_melody(T_PRESTO,8,melody);
	myPlayer2.tone(NOTE_FS4, 2.0);
	sleep_ms(4000);

	};
    return 0;
}

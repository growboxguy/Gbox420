#include <stdio.h>
#include <tusb.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico_tone.hpp"

unsigned char buffer[85];
uint boventonen[7];
		
unsigned char * readLine() {
    unsigned char u , *p ;
    for (p=buffer, u = getchar() ; u != '\r' && p - buffer <80 ; u = getchar()) putchar(*p++=u);
    *p = 0 ;
	printf("\n") ;
    return buffer;
}


int main() {
	
	char * str ;
	
	
	stdio_init_all();
    while (!tud_cdc_connected()) {sleep_ms(100);}

			
	Tone myPlayer(15,50,0,20,0,20,0,10);
	printf("Init Machine & dma channel!\n");
	int res = myPlayer.init(TONE_NON_BLOCKING, false) ;
	printf("Result of initializing tone = 0x%X\n", res) ;

	while (true) {
	
	printf("Defining color of sound!!\n");
	printf("Percentage of root note \% :") ;
	str = (char *)readLine();
	boventonen[0] = atoi(str);
	for (int i = 1; i<7 ; i++) {
		printf("Harmonic %d \% :",i+1);
		str = (char *) readLine() ;
		boventonen[i] = atoi(str);
	};
	
	myPlayer.reconfigure_harmonics(boventonen[0],boventonen[1],boventonen[2],boventonen[3],boventonen[4],boventonen[5],boventonen[6]);
	
	printf("Frequency (Hz): ");
	str = (char *) readLine() ;
	int frequency = atoi(str);
	printf("Duration (in ms): ");
	str = (char *) readLine() ;
	float duration = (atoi(str) / 1000.0) ;


	bool again = true ;
	
	while (again) {

	myPlayer.tone(frequency,duration);
	sleep_ms(5000);
	printf("Play again (y/n)?");
	str = (char *)readLine() ;
	if (str[0] != 'y') again = false ;
	};

}
    return 0;

}

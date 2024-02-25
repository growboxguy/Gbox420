// GrowBoxGuy - http://sites.google.com/site/growboxguy/
// Sketch for testing: Piezo buzzer connected to a Raspberry Pi using a 220ohm resistor

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Pins
const int BuzzerOutPin = 2;    ///< Port connecting the + side of the buzzer, over a minimum 200ohm resistor

// Global constants
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

static inline void pwm_calcDivTop(pwm_config *c, int frequency, int sysClock)
{
  uint count = sysClock * 16 / frequency;
  uint div = count / 60000; // to be lower than 65535*15/16 (rounding error)
  if (div < 16)
    div = 16;
  c->div = div;
  c->top = count / div;
}

void beep(int note, int duration)
{
  pwm_config cfg = pwm_get_default_config();
  int slice_num = pwm_gpio_to_slice_num(BuzzerOutPin);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
 
    pwm_set_enabled(slice_num, false);
    // get new frequency
    pwm_calcDivTop(&cfg, note, 125000000);
    pwm_init(slice_num, &cfg, true);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, cfg.top / 2);
    pwm_set_enabled(slice_num, true);
    sleep_ms(duration);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
    sleep_ms(50);
}

void firstSection()
{
  beep(a, 500);
  beep(a, 500);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  sleep_ms(500);

  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);

  sleep_ms(500);
}

void secondSection()
{
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);
  beep(fSH, 250);

  sleep_ms(325);

  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);
  beep(cSH, 175);
  beep(cH, 125);
  beep(b, 125);
  beep(cH, 250);

  sleep_ms(350);
}


int main()
{
  gpio_set_function(BuzzerOutPin, GPIO_FUNC_PWM);

  while (true)
  {
    stdio_init_all();   
    printf("Playing song...\n");
    // Play first section
    firstSection();

    // Play second section
    secondSection();

    // Variant 1
    beep(f, 250);
    beep(gS, 500);
    beep(f, 350);
    beep(a, 125);
    beep(cH, 500);
    beep(a, 375);
    beep(cH, 125);
    beep(eH, 650);

    sleep_ms(500);

    // Repeat second section
    secondSection();

    // Variant 2
    beep(f, 250);
    beep(gS, 500);
    beep(f, 375);
    beep(cH, 125);
    beep(a, 500);
    beep(f, 375);
    beep(cH, 125);
    beep(a, 650);

    printf("Wait a minute before replaying the song...\n");
    sleep_ms(60000);
  }
}
#include "Sound.h"

Sound::Sound(const char *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Parent->SoundFeedback = this; ///< Pointer for child objects to use sound feedback
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  //Tone1 = new TonePlayer(TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno/Nano), D11 (Mega)
  gpio_set_function(*Pin, GPIO_FUNC_PWM);
  cfg = pwm_get_default_config();
  slice_num = pwm_gpio_to_slice_num(*Pin);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
  pwm_set_enabled(slice_num, false);
  Parent->addToRefreshQueue(this);
  printf("   Sound ready\n");
  checkEvents();
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void Sound::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat); //< Load the objects name to the LongMessage buffer a the beginning of a JSON :  "Name":{
  strcat(LongMessage, "\"S\":\"");
  strcat(LongMessage, getEnabledStateText(FriendlyFormat));
  strcat(LongMessage, "\"}"); ///< closing the curly bracket at the end of the JSON
}

void Sound::refresh()
{
  //Common::refresh();
  checkEvents();
}

void Sound::checkEvents()
{
  if (*Enabled)
  {
    if (PlayOffSound)
      OffSound();
    if (PlayOnSound)
      OnSound();
  }
  PlayOnSound = false;
  PlayOffSound = false;
}

void Sound::playOnOffSound(bool State)
{
  if (State)
    PlayOnSound = true;
  else
    PlayOffSound = true;
}

void Sound::playOnSound()
{
  PlayOnSound = true;
}

void Sound::playOffSound()
{
  PlayOffSound = true;
}

void Sound::setSoundOnOff(bool State)
{
  *Enabled = State;
  playOnOffSound(*Enabled);
  Parent->addToLog(getName(getEnabledStateText(true)));
}

void inline Sound::pwm_calcDivTop(pwm_config *c, int frequency, int sysClock)
{
  uint count = 2000000000 / frequency;  ///2000000000: system clock(125000000) x 16 
  uint div = count / 60000; // to be lower than 65535*15/16 (rounding error)
  if (div < 16)
    div = 16;
  c->div = div;
  c->top = count / div;
}


void Sound::beep(int note, int duration)
{
  pwm_calcDivTop(&cfg, note, 125000000);
  pwm_init(slice_num, &cfg, true);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, cfg.top / 2);
  pwm_set_enabled(slice_num, true);
  sleep_ms(duration);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
  sleep_ms(50);
}

void Sound::OnSound()
{
  if (*Enabled)
  {
    beep(440, 150);
    sleep_ms(10);
    beep(880, 150);
  }
}

void Sound::OffSound()
{
  if (*Enabled)
  {
    beep(880, 150);
    sleep_ms(10);
    beep(440, 150);
  }
}

bool Sound::getEnabledState()
{
  return *Enabled;
}

char *Sound::getEnabledStateText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_enabledDisabled(*Enabled);
  }
  else
  {
    return toText(*Enabled);
  }
}
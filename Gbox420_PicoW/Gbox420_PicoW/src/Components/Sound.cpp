#include "Sound.h"

Sound::Sound(const char *Name, Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  Parent->SoundFeedback = this; ///< Pointer for child objects to use sound feedback
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  //Tone1 = new TonePlayer(TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno/Nano), D11 (Mega)
  gpio_set_function(BuzzerOutPin, GPIO_FUNC_PWM);
  cfg = pwm_get_default_config();
  slice_num = pwm_gpio_to_slice_num(BuzzerOutPin);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
  pwm_set_enabled(slice_num, false);
  Parent->addToRefreshQueue_Sec(this);
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

void Sound::refresh_Sec()
{
  Common::refresh_Sec();
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
    beep(f, 350);
    delay(100);
    beep(a, 650);
  }
}

void Sound::OffSound()
{
  if (*Enabled)
  {
    beep(f, 650);
    delay(100);
    beep(a, 350);
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
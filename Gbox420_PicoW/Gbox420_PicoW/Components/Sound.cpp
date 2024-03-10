#include "Sound.h"

Sound::Sound(Module *Parent, Settings::SoundSettings *DefaultSettings) : Common(DefaultSettings->Name)
{
  this->Parent = Parent;
  Parent->SoundFeedback = this; ///< Pointer for child objects to use sound feedback
  Pin = &DefaultSettings->Pin;
  Enabled = &DefaultSettings->Enabled;
  // Tone1 = new TonePlayer(TCCR1A, TCCR1B, OCR1AH, OCR1AL, TCNT1H, TCNT1L);  // pin D9 (Uno/Nano), D11 (Mega)
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

bool Sound::commandEvent(char *Command, char *Data)
{ ///< When the website field is submitted
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp(ShortMessage, "E") == 0)
    {
      // setSoundOnOff(WebServer.getArgBoolean());
    }
    else if (strcmp(ShortMessage, "Ee") == 0)
    {
      playEE();
    }
    return true;
  }
}

void Sound::refresh()
{
  // Common::refresh();
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
    if (PlayEE)
      EE();
  }
  PlayOnSound = false;
  PlayOffSound = false;
  PlayEE = false;
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

void Sound::playEE()
{
  PlayEE = true;
}

void Sound::setSoundOnOff(bool State)
{
  *Enabled = State;
  playOnOffSound(*Enabled);
  Parent->addToLog(getName(getEnabledStateText(true)));
}

void inline Sound::pwm_calcDivTop(pwm_config *c, int frequency, int sysClock)
{
  uint count = 2000000000 / frequency; /// 2000000000: system clock(125000000) x 16
  uint div = count / 60000;            // to be lower than 65535*15/16 (rounding error)
  if (div < 16)
    div = 16;
  c->div = div;
  c->top = count / div;
}

void Sound::beep(const int note, const int duration)
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

void Sound::EE()
{
  printf("Easter egg\n");

  for (int thisNote = 0; thisNote < 134; thisNote++)
  {
    beep(melody[thisNote], tempo[thisNote]);
    //  watchdog_update(); ///< Reset Watchdog timeout to avoid Arduino reseting while playing the song
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

const int Sound::melody[] = { ///< https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
    2637, 2637, 0, 2637,
    0, 2093, 2637, 0,
    3136, 0, 0, 0,
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
    233, 220, 208};

const uint16_t Sound::tempo[] = {
    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,

    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,

    90, 90, 90,
    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,

    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,

    90, 90, 90,
    120, 120, 120, 120,
    120, 120, 120, 120,
    120, 120, 120, 120,

    300, 300, 300,

    120, 120, 120, 120,
    120, 120, 60, 30,
    120, 120, 120, 120,
    120, 120, 60, 30,
    120, 120, 120, 120,
    120, 120, 60, 30,
    120, 120, 120, 120,
    120, 120, 60,
    60, 180, 180, 180,
    60, 60,
    60, 60,
    60, 60,
    180, 180, 180, 180, 180, 180,
    100, 100, 100,
    100, 100, 100};
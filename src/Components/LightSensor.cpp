#include "LightSensor.h"
#include "Lights.h"
#include "Sound.h"

LightSensor::LightSensor(const __FlashStringHelper *Name, Module *Parent, Settings::LightSensorSettings *DefaultSettings, Lights *LightSource)
    : Common(Name),
      Parent(Parent),                          // Dereferencing Parent to use reference
      LightSource(LightSource),                // Dereferencing LightSource to use reference
      DigitalPin(DefaultSettings->DigitalPin), // Using DefaultSettings pointer but passing values as references
      AnalogPin(DefaultSettings->AnalogPin)
{
  pinMode(DigitalPin, INPUT);
  pinMode(AnalogPin, INPUT);
  // calibrate(false);  // Uncomment to calibrate when needed
  Parent->addToReportQueue(this);          // Using pointer notation here for consistency
  Parent->addToRefreshQueue_FiveSec(this); // Using pointer notation here for consistency
  logToSerials(F("LightSensor ready"), true, 3);
}

void LightSensor::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  if (CalibrateRequested)
  {
    calibrate();
  } ///< If calibration was requested
  Dark = digitalRead(DigitalPin); ///< True: No light detected
  LightReading = (1023 - analogRead(AnalogPin));
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void LightSensor::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"R\":\""));
  strcat(LongMessage, getReadingText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, getDarkText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void LightSensor::triggerCalibration()
{ ///< website signals to calibrate light sensor MAX and MIN readings the next time a refresh runs
  CalibrateRequested = true;
  Parent->addToLog(getName(F("calibrating")));
  Parent->getSoundObject()->playOnSound();
}

void LightSensor::calibrate(bool AddToLog)
{ ///< Takes ~2sec total, could trigger a watchdog reset!
  CalibrateRequested = false;
  bool LastStatus = LightSource->getStatus();
  uint8_t LastBrightness = LightSource->getBrightness();
  LightSource->setLightOnOff(false, false);   ///< turn off light, without adding a log entry
  delay(DelaySec);                            ///< wait for light output change
  Readings[0] = 1023 - analogRead(AnalogPin); ///< store the reading in darkness to the first element of the Readings[10] array
  LightSource->setLightOnOff(true, false);    ///< turn on light, without adding a log entry
  for (uint8_t ReadingCounter = 0; ReadingCounter < (ReadingArrayDepth - 1);)
  {                                                                  ///< This probably looks dodgy as the 3rd parameter of the for cycle is empty. ReadingCounter is incremented in the code
    LightSource->setBrightness(ReadingCounter++ * 10, false, false); ///< Increment ReadingCounter AFTER reading its value
    wdt_reset();                                                     ///< Reset watchdog timer before waiting
    delay(DelaySec);                                                 ///< wait for light output change
    Readings[ReadingCounter] = 1023 - analogRead(AnalogPin);
  }
  LightSource->setBrightness(LastBrightness, false, false); ///< restore original brightness, without adding a log entry
  LightSource->setLightOnOff(LastStatus, false);            ///< restore original state, without adding a log entry
  getCalibrationReadings();
  if (AddToLog)
  {
    Parent->addToLog(getName(F("calibrated")));
    logToSerials(&LongMessage, true, 4);
  }
}

void LightSensor::getCalibrationReadings()
{                                                ///< Returns a pointer to a char array
  memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("{\"Readings\":["));
  for (uint8_t ReadingCounter = 0; ReadingCounter < ReadingArrayDepth; ReadingCounter++)
  {
    strcat(LongMessage, toText(Readings[ReadingCounter]));
    strcat_P(LongMessage, (PGM_P)F(","));
  }
  LongMessage[strlen(LongMessage) - 1] = ']'; ///< replace the last , to the closing of the array
  strcat_P(LongMessage, (PGM_P)F("}"));       ///< close JSON object
}

int LightSensor::getReading() ///< Gets the average light sensor reading, if passed false as a parameter it returns the latest reading, not the average
{
  return LightReading;
}

char *LightSensor::getReadingText(bool FriendlyFormat)
{
  itoa(LightReading, ShortMessage, 10);
  if (FriendlyFormat)
  {
    if (Dark)
    {
      strcat_P(ShortMessage, (PGM_P)F("-NIGHT"));
    }
    else
    {
      strcat_P(ShortMessage, (PGM_P)F("-DAY"));
    }
  }
  return ShortMessage;
}

bool LightSensor::getDark()
{
  return Dark;
}

char *LightSensor::getDarkText(bool UseWords)
{
  if (UseWords)
    return toText_yesNo(Dark);
  else
    return toText(Dark);
}
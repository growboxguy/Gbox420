#include "DistanceSensor.h"

/*! 
 *  @brief  Class for the Ultrasonic distance sensor - HC-SR04 or JSN-SR04T
 */
DistanceSensor::DistanceSensor(const __FlashStringHelper *Name, Module *Parent, Settings::DistanceSensorSettings *DefaultSettings) : Common(Name)
{
  this->Parent = Parent;
  EchoPin = &DefaultSettings->EchoPin;
  TriggerPin = &DefaultSettings->TriggerPin;
  pinMode(*TriggerPin, OUTPUT);
  pinMode(*EchoPin, INPUT);
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_FiveSec(this);
  logToSerials(F("Distance Sensor object created"), true, 3);
}

void DistanceSensor::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();
  readSensor();
}

void DistanceSensor::readSensor()
{
  digitalWrite(*TriggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(*TriggerPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(*TriggerPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long Duration = pulseIn(*EchoPin, HIGH);
  if (*Metric)
  {
    Distance = Duration * 0.017;
  }
  else
  {
    Distance = Duration * 0.00665;
  }
}

void DistanceSensor::report()
{
  Common::report();
  memset(&LongMessage[0], 0, MaxLongTextLength); ///< clear variable
  strcat_P(LongMessage, (PGM_P)F("Distance:"));
  strcat(LongMessage, getDistanceText(true));
  logToSerials(&LongMessage, true, 1);
}

float DistanceSensor::getDistance()
{
  return Distance;
}

char *DistanceSensor::getDistanceText(bool IncludeUnits)
{
  if (IncludeUnits)
    return toText_distance(Distance);
  else
    return toText(Distance);
}
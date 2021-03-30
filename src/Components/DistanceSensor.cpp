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
  logToSerials(F("Distance Sensor ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void DistanceSensor::report(bool FriendlyFormat)
{
  Common::report(FriendlyFormat);
  strcat_P(LongMessage, (PGM_P)F("\"D\":\""));
  strcat(LongMessage, getDistanceText(FriendlyFormat));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void DistanceSensor::refresh_FiveSec()
{
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

float DistanceSensor::getDistance()
{
  return Distance;
}

char *DistanceSensor::getDistanceText(bool FriendlyFormat)
{
  if (FriendlyFormat)
    return toText_distance(Distance);
  else
    return toText(Distance);
}
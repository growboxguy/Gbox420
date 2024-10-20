#pragma once

///< Core module - Contains a single Sound component only. In case you would like to create your own module this is a good starting point

#include "420Module.h"
#include "MQTTClient.h"
// #include "ELClientRest.h" ///< ESP-link - REST API

// forward declaration of classes
class Sound;
class MqttClient;

// extern ELClientRest PushingBoxRestAPI;

class GboxModule : public Module
{
public:
  GboxModule(Settings::GboxModuleSettings *DefaultSettings, Settings *GboxSettings);
  Sound *Sound1 = nullptr; ///< Pointer to a Piezo speaker - sound feedback
  bool commandEvent(char *Command, char *Data);
  void report(bool FriendlyFormat = false);
  /*
  void run1sec();
  void run5sec();
  void run1min();
  */

private:
protected:
  void setDebug(bool State);          ///< Turn debug messages printed to stdout ON or OFF
  void toggleDebug();                 ///< Invert the state of printing debug messages
  void setMetric(bool MetricEnabled); ///< Metric or Imperial mode
  char *getDebugText(bool FriendlyFormat = false);
  char *getMetricText(bool FriendlyFormat = false);
};
#include <functional>
#include "GboxModule.h"
#include "Sound.h"

/**
 * @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings, creates components that the instance controls, and subscribes to events
 */
GboxModule::GboxModule(Settings::GboxModuleSettings *DefaultSettings, Settings *GboxSettings) : Common(DefaultSettings->Name), Module(DefaultSettings->Name)
{
  Sound1 = new Sound(this, &GboxSettings->Sound1); // Passing DefaultSettings members as references: Changes get written back to DefaultSettings and saved to EEPROM. (uint8_t *)(((uint8_t *)&DefaultSettings) + offsetof(Settings, VARIABLENAME))
  this->DefaultSound = Sound1;

  addToReportQueue(this); //< Attach to the report event: When triggered the module reports to the Serial Console or to MQTT
  addToCommandQueue(this);
  addToLog("GboxModule ready", 0);
}

/**
 * @brief Report current state in a JSON format to the LongMessage buffer
 */
void GboxModule::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat(LongMessage, "\"M\":\"");
  strcat(LongMessage, getMetricText(FriendlyFormat));
  strcat(LongMessage, "\",\"D\":\"");
  strcat(LongMessage, getDebugText(FriendlyFormat));
  strcat(LongMessage, "\"}"); ///< closing the curly bracket at the end of the JSON  
}

/**
 * @brief Process commands received from MQTT subscription. Command format: Name_Command, Example: Sound1_Ee
 *
 * @return true : The Name matched with the object's name and the Command was processed
 * @return false : The command was not intended for the object
 */
bool GboxModule::commandEvent(char *Command, char *Data)
{
  if (!isThisForMe(Command)) // Compares the incoming Command against the object's Name. If the command starts with the objects name followed by a _  the command is extracted to ShotMessage buffer
  {
    return false;
  }
  else
  {
    if (strcmp(ShortMessage, "D") == 0)
    {
      toggleDebug();
    }
    else if (strcmp(ShortMessage, "Ee") == 0)
    {
      getSoundObject()->playEE();
    }
    else if (strcmp(ShortMessage, "M") == 0)
    {
      setMetric(!*Metric);
    }
    return true; // Match found
  }
}

///< Settings

char *GboxModule::getDebugText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(*Debug);
  }
  else
  {
    return toText(*Debug);
  }
}

char *GboxModule::getMetricText(bool FriendlyFormat)
{
  if (FriendlyFormat)
  {
    return toText_onOff(*Metric);
  }
  else
  {
    return toText(*Metric);
  }
}

///< Settings
void GboxModule::setDebug(bool DebugEnabled)
{
  *Debug = DebugEnabled;
  if (*Debug)
  {
    addToLog("Debug ON");
  }
  else
  {
    addToLog("Debug OFF");
  }
  getSoundObject()->playOnOffSound(*Debug);
}

void GboxModule::toggleDebug()
{
  setDebug(!*Debug);
}

void GboxModule::setMetric(bool MetricEnabled)
{
  if (MetricEnabled != *Metric)
  { ///< if there was a change
    *Metric = MetricEnabled;
    RefreshRequested = true;
  }
  if (*Metric)
    addToLog("Using Metric units");
  else
    addToLog("Using Imperial units");
  getSoundObject()->playOnSound();
}

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
bool GboxModule::commandEvent(char *Name_Command, char *Data)
{
  if (!isThisForMe(Name_Command)) // Compares the incoming Cop
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
    return true; // Match found
  }
}
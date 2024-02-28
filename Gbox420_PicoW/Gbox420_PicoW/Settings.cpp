#include "Settings.h"

/**
  \brief Store settings in EEPROM - Only updates changed bits
  \attention Use cautiously, EEPROM has a write limit of 100.000 cycles 
*/

void saveSettings(Settings *ToSave)
{
  ;
  //eeprom_update_block((const void *)ToSave, (void *)0, sizeof(Settings)); // update_block only writes the bytes that changed
}

/**
  \brief Load settings from EEPROM
  \param ResetEEPROM - Force loading the defaults from the sketch and overwriting the EEPROM with it
  \return Reference to Settings object
*/
Settings *loadSettings(bool ResetEEPROM) ///< if the function contains arguments with default values, they must be declared strictly before they are called, otherwise there is a compilation error: '<function name> was not declared in this scope. https://forum.arduino.cc/index.php?topic=606678.0
{
  printf("Loading settings");
  Settings *DefaultSettings = new Settings();                              // This is where settings are stored, first it takes the sketch default settings defined in Settings.h
  /*
  Settings EEPROMSettings;                                                 // temporary storage with "Settings" type
  eeprom_read_block((void *)&EEPROMSettings, (void *)0, sizeof(Settings)); // Load EEPROM stored settings into EEPROMSettings
  if (DefaultSettings->CompatibilityVersion != EEPROMSettings.CompatibilityVersion || ResetEEPROM)
  { // Making sure the EEPROM loaded settings are compatible with the sketch
    printf(" Updating EEPROM");
    saveSettings(DefaultSettings); // overwrites EEPROM stored settings with defaults from this sketch
  }
  else
  {
    printf(" Applying EEPROM settings");
    // DefaultSettings = EEPROMSettings; // overwrite sketch defaults with loaded settings
    memcpy(DefaultSettings, &EEPROMSettings, sizeof(Settings));
  }
  */
  printf("   Done. Version: ");
  printf("%hhu\n",DefaultSettings->CompatibilityVersion);
  return DefaultSettings;
}

/**
  \brief Load sketch default settings into EEPROM 
  \attention Restarts the Arduino sketch!
*/
void restoreDefaults()
{
  printf("Forcing settings update at next restart\n");
  loadSettings(true);
  printf(" Reseting the sketch\n");
  //__asm__ __volatile__("jmp 0x0000");  //TODO: Force a watchdog reboot here
}

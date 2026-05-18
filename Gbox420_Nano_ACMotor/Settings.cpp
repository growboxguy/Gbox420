#include "Arduino.h"
#include "Settings.h"
#include "SerialLog.h"

/**
  \brief Store settings in EEPROM - Only updates changed bits
  \attention Use cautiously, EEPROM has a write limit of 100.000 cycles 
*/
void saveSettings(Settings *ToSave)
{
  eeprom_update_block((const void *)ToSave, (void *)0, sizeof(Settings)); // update_block only writes the bytes that changed
}

/**
  \brief Load settings from EEPROM
  \param ResetEEPROM - Force loading the defaults from the sketch and overwriting the EEPROM with it
  \return Reference to Settings object
*/
Settings *loadSettings(bool ResetEEPROM) 
{
  Settings *DefaultSettings = new Settings(); // Allocates sketch defaults on the Heap

  // Create a pointer on the Heap instead of a massive local variable on the Stack
  Settings *EEPROMSettings = new Settings(); 
  
  // Load EEPROM stored settings directly into our heap pointer
  eeprom_read_block((void *)EEPROMSettings, (void *)0, sizeof(Settings)); 
  
  if (DefaultSettings->CompatibilityVersion != EEPROMSettings->CompatibilityVersion || ResetEEPROM)
  { 
    logToSerials(F("Updating EEPROM"), false, 0);
    saveSettings(DefaultSettings); // overwrites EEPROM stored settings with defaults from this sketch
  }
  else
  {
    logToSerials(F("Loading EEPROM"), false, 0);
    // Safely copy the values from our temporary heap block to our default settings block
    memcpy(DefaultSettings, EEPROMSettings, sizeof(Settings));
  }

  // CRITICAL: Free up the temporary heap memory so we don't leak RAM!
  delete EEPROMSettings;

  logToSerials(F("Version"), false, 3);
  logToSerials(DefaultSettings->CompatibilityVersion, true, 1);
  return DefaultSettings;
}

/**
  \brief Load sketch default settings into EEPROM 
  \attention Restarts the Arduino sketch!
*/
void restoreDefaults()
{
  loadSettings(true);
  __asm__ __volatile__("jmp 0x0000"); //Restart sketch
}
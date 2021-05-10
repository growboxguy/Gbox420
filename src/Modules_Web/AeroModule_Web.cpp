#include "AeroModule_Web.h"

struct AeroModuleCommand AeroModuleCommand1ToSend = {AeroMessages::AeroModuleCommand1};      ///< Command to send will be stored here
struct AeroModuleResponse AeroModuleResponse1Received = {AeroMessages::AeroModuleResponse1}; ///< Default startup values
struct AeroCommand_P1 AeroCommand1ToSend = {AeroMessages::AeroCommand1};                     ///< Command to send will be stored here
struct AeroResponse_P1 AeroResponse1Received = {AeroMessages::AeroResponse1};                ///< Default startup values
struct AeroCommand_P2 AeroCommand2ToSend = {AeroMessages::AeroCommand2};                     ///< Command to send will be stored here
struct AeroResponse_P2 AeroResponse2Received = {AeroMessages::AeroResponse2};                ///< Default startup values
struct AeroCommonTemplate AeroResetToSend = {AeroMessages::AeroReset};                       ///< Special command to fetch the next Response from the Receiver

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings and subscribes to events
*/
AeroModule_Web::AeroModule_Web(const __FlashStringHelper *Name, Module_Web *Parent, Settings::AeroModuleSettings *DefaultSettings) : Common_Web(Name), Common(Name)
{ ///< Constructor
  this->Parent = Parent;
  this->DefaultSettings = DefaultSettings;
  updateCommands();
  memcpy_P(this->WirelessChannel, (PGM_P)Name, sizeof(this->WirelessChannel));
  Parent->addToReportQueue(this);
  Parent->addToRefreshQueue_Sec(this);
  Parent->addToRefreshQueue_FiveSec(this);
  Parent->addToRefreshQueue_Minute(this);
  Parent->addToWebsiteQueue_Load(this);
  Parent->addToWebsiteQueue_Refresh(this);
  Parent->addToCommandQueue(this);
  logToSerials(F("AeroModule_Web ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void AeroModule_Web::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_onlineStatus(OnlineStatus) : toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"P\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_pressure(AeroResponse1Received.Pressure) : toText(AeroResponse1Received.Pressure));
  strcat_P(LongMessage, (PGM_P)F("\",\"W\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_weight(AeroResponse1Received.Weight) : toText(AeroResponse1Received.Weight));
  strcat_P(LongMessage, (PGM_P)F("\",\"Ma\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_pressure(AeroCommand1ToSend.MaxPressure) : toText(AeroCommand1ToSend.MaxPressure));
  if (AeroResponse1Received.PressureTankPresent)
  {
    strcat_P(LongMessage, (PGM_P)F("\",\"Mi\":\""));
    strcat(LongMessage, FriendlyFormat ? toText_pressure(AeroCommand1ToSend.MinPressure) : toText(AeroCommand1ToSend.MinPressure));
    strcat_P(LongMessage, (PGM_P)F("\",\"AS\":\""));
    strcat(LongMessage, FriendlyFormat ? toText_aeroTankState((AeroTankStates)AeroResponse1Received.AeroState) : toText(AeroResponse1Received.AeroState));
  }
  else
  {
    strcat_P(LongMessage, (PGM_P)F("\",\"AS\":\""));
    strcat(LongMessage, FriendlyFormat ? toText_aeroNoTankState((AeroNoTankStates)AeroResponse1Received.AeroState) : toText(AeroResponse1Received.AeroState));
  }
  strcat_P(LongMessage, (PGM_P)F("\",\"LS\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_pressure(AeroResponse1Received.LastSprayPressure) : toText(AeroResponse1Received.LastSprayPressure));
  strcat_P(LongMessage, (PGM_P)F("\",\"PSt\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_pressurePumpState(AeroResponse1Received.PumpState) : toText((int)AeroResponse1Received.PumpState));
  strcat_P(LongMessage, (PGM_P)F("\",\"PS\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_percentage(AeroCommand2ToSend.PumpSpeed) : toText(AeroCommand2ToSend.PumpSpeed));
  strcat_P(LongMessage, (PGM_P)F("\",\"PT\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(AeroCommand2ToSend.PumpTimeOut) : toText(AeroCommand2ToSend.PumpTimeOut));
  strcat_P(LongMessage, (PGM_P)F("\",\"PP\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(AeroCommand2ToSend.PumpPrimingTime) : toText(AeroCommand2ToSend.PumpPrimingTime));
  strcat_P(LongMessage, (PGM_P)F("\",\"SE\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_enabledDisabled(AeroResponse1Received.SprayEnabled) : toText(AeroResponse1Received.SprayEnabled));
  strcat_P(LongMessage, (PGM_P)F("\",\"D\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(AeroCommand1ToSend.Duration) : toText(AeroCommand1ToSend.Duration));
  strcat_P(LongMessage, (PGM_P)F("\",\"DI\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(AeroCommand1ToSend.DayInterval) : toText(AeroCommand1ToSend.DayInterval));
  strcat_P(LongMessage, (PGM_P)F("\",\"NI\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_second(AeroCommand1ToSend.NightInterval) : toText(AeroCommand1ToSend.NightInterval));
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

/**
* @brief Set values on the ESP-link website when it loads
*/
void AeroModule_Web::websiteEvent_Load(char *url)
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgBoolean(getComponentName(F("Tank")), AeroResponse1Received.PressureTankPresent);
    WebServer.setArgString(getComponentName(F("Dur")), toText(AeroCommand1ToSend.Duration));
    WebServer.setArgInt(getComponentName(F("DInt")), AeroCommand1ToSend.DayInterval);
    WebServer.setArgInt(getComponentName(F("NInt")), AeroCommand1ToSend.NightInterval);
    WebServer.setArgString(getComponentName(F("PMx")), toText(AeroCommand1ToSend.MaxPressure));
    WebServer.setArgString(getComponentName(F("PMn")), toText(AeroCommand1ToSend.MinPressure));
    WebServer.setArgInt(getComponentName(F("PS")), AeroCommand2ToSend.PumpSpeed);
    WebServer.setArgInt(getComponentName(F("PT")), AeroCommand2ToSend.PumpTimeOut);
    WebServer.setArgInt(getComponentName(F("PPT")), AeroCommand2ToSend.PumpPrimingTime);
  }
}

/**
* @brief Set values on the ESP-link website when it refreshes
*/
void AeroModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///< called when website is refreshed.
{
  if (strncmp(url, "/G", 2) == 0)
  {
    WebServer.setArgString(getComponentName(F("S")), toText_onlineStatus(OnlineStatus));
    if (AeroResponse1Received.PressureTankPresent)
    {
      WebServer.setArgString(getComponentName(F("AS")), toText_aeroTankState((AeroTankStates)AeroResponse1Received.AeroState));
    }
    else
    {
      WebServer.setArgString(getComponentName(F("AS")), toText_aeroNoTankState((AeroNoTankStates)AeroResponse1Received.AeroState));
    }

    WebServer.setArgString(getComponentName(F("P")), toText_pressurePumpState(AeroResponse1Received.PumpState));
    WebServer.setArgString(getComponentName(F("Pr")), toText_pressure(AeroResponse1Received.Pressure));
    WebServer.setArgString(getComponentName(F("LSP")), toText_pressure(AeroResponse1Received.LastSprayPressure));
    WebServer.setArgString(getComponentName(F("W")), toText_weight(AeroResponse1Received.Weight));
  }
}

/**
* @brief Process commands received from MQTT subscription or from the ESP-link website
*/
void AeroModule_Web::commandEvent(char *Command, char *Data)
{ ///< When a button is pressed on the website
  if (!isThisMyComponent(Command))
  {
    return;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("SprayEn")) == 0)
    {
      AeroCommand1ToSend.SprayEnabled = true;
      Parent->addToLog(F("Spray enabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayDis")) == 0)
    {
      AeroCommand1ToSend.SprayDisabled = true;
      Parent->addToLog(F("Spray disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayNow")) == 0)
    {
      AeroCommand1ToSend.SprayNow = true;
      Parent->addToLog(F("Starting spray"));
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("SprayOff")) == 0)
    {
      AeroCommand1ToSend.SprayOff = true;
      Parent->addToLog(F("Stop spray"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOn")) == 0)
    {
      AeroCommand2ToSend.PumpOn = true;
      Parent->addToLog(F("Aero pump ON"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpOff")) == 0) //Sending a turn OFF command re-enables the pump and resets its status to IDLE
    {
      AeroCommand2ToSend.PumpOff = true;
      Parent->addToLog(F("Aero pump IDLE"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PumpDis")) == 0)
    {
      AeroCommand2ToSend.PumpDisable = true;
      Parent->addToLog(F("Aero pump disabled"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Mix")) == 0)
    {
      AeroCommand2ToSend.MixReservoir = true;
      Parent->addToLog(F("Mixing reservoir"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("T")) == 0)
    {
      AeroCommand2ToSend.TareWeight = true;
      Parent->addToLog(F("Taring aero scale"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Refill")) == 0)
    {
      if (AeroResponse1Received.PressureTankPresent)
      {
        AeroCommand2ToSend.RefillPressureTank = true;
        Parent->addToLog(F("Refilling tank"), false);
      }
      else
      {
        Parent->addToLog(F("Pressure tank not available"), false);
      }
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Drain")) == 0)
    {
      if (AeroResponse1Received.PressureTankPresent)
      {
        AeroCommand2ToSend.DrainPressureTank = true;
        Parent->addToLog(F("Draining tank"), false);
      }
      else
      {
        Parent->addToLog(F("Pressure tank not available"), false);
      }
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Dur")) == 0)
    {
      DefaultSettings->Duration = toFloat(Data);
      Parent->addToLog(F("Spray duration updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("DInt")) == 0)
    {
      DefaultSettings->DayInterval = toInt(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("NInt")) == 0)
    {
      DefaultSettings->NightInterval = toInt(Data);
      Parent->addToLog(F("Spray interval updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PS")) == 0)
    {
      DefaultSettings->PumpSpeed = toInt(Data);
      Parent->addToLog(F("Pump speed updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PT")) == 0)
    {
      DefaultSettings->PumpTimeOut = toInt(Data);
      Parent->addToLog(F("Pump timeout updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PPT")) == 0)
    {
      DefaultSettings->PrimingTime = toInt(Data);
      Parent->addToLog(F("Priming time updated"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PMn")) == 0)
    {
      DefaultSettings->MinPressure = toFloat(Data);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("PMx")) == 0)
    {
      DefaultSettings->MaxPressure = toFloat(Data);
      Parent->addToLog(F("Pressure limits updated"), false);
    }
    SyncRequested = true;
  }
}

/**
* @brief Refresh state, Called every second
*/
void AeroModule_Web::refresh_Sec()
{
  Common::refresh_Sec();
  if (SyncRequested)
  {
    SyncRequested = false;
    sendMessages();
  }
}

/**
* @brief Refresh state, Called every five seconds
*/
void AeroModule_Web::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  sendMessages();
}

/**
* @brief Refresh state, Called every minute
*/
void AeroModule_Web::refresh_Minute()
{
  Common::refresh_Minute();
}

/**
* @brief Exchange messages with the wireless Aeroponics module
*/
void AeroModule_Web::sendMessages()
{
  updateCommands();
  sendCommand(&AeroResetToSend);          ///< special Command, resets communication to first message
  sendCommand(&AeroModuleCommand1ToSend); ///< Command - Response exchange
  sendCommand(&AeroCommand1ToSend);
  sendCommand(&AeroCommand2ToSend); ///< Command - Response exchange
  if (*(Parent->SerialReportWireless) && *Debug)
  {
    logToSerials(F("Message exchange finished"), true, 1);
  }
}

/**
* @brief Send a command message and process the response message
*/
AeroMessages AeroModule_Web::sendCommand(void *CommandToSend)
{
  AeroMessages SequenceIDToSend = ((AeroCommonTemplate *)CommandToSend)->SequenceID;
  AeroMessages ReceivedSequenceID = NULL;
  if (*(Parent->SerialReportWireless))
  {
    logToSerials(F("Sending:"), false, 1);
    logToSerials(toText_aeroSequenceID(SequenceIDToSend), false, 1);
  }
  Parent->Wireless->openWritingPipe(WirelessChannel);
  Parent->Wireless->flush_rx(); ///< Dump all previously received but unprocessed messages
  if (Parent->Wireless->write(CommandToSend, WirelessPayloadSize))
  {
    delay(50); ///< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
    if (Parent->Wireless->isAckPayloadAvailable())
    {
      OnlineStatus = true;
      Parent->Wireless->read(ReceivedResponse, WirelessPayloadSize);
      ReceivedSequenceID = ((AeroCommonTemplate *)ReceivedResponse)->SequenceID;
      if (*(Parent->SerialReportWireless))
      {
        logToSerials(F("; Response:"), false, 1);
        logToSerials(toText_aeroSequenceID(ReceivedSequenceID), false, 1);
        logToSerials(F("; Data:"), false, 1);
      }

      switch (ReceivedSequenceID)
      {
      case AeroMessages::AeroModuleResponse1:
        memcpy(&AeroModuleResponse1Received, ReceivedResponse, sizeof(struct AeroModuleResponse));
        if (*(Parent->SerialReportWireless))
        {
          logToSerials(AeroModuleResponse1Received.Status, true, 1);
        }
        break;
      case AeroMessages::AeroResponse1:
        memcpy(&AeroResponse1Received, ReceivedResponse, sizeof(struct AeroResponse_P1));
        if (*(Parent->SerialReportWireless))
        {
          logToSerials(AeroResponse1Received.ConfirmSprayEnabled, false, 1);
          logToSerials(AeroResponse1Received.ConfirmSprayDisabled, false, 1);
          logToSerials(AeroResponse1Received.ConfirmSprayNow, false, 1);
          logToSerials(AeroResponse1Received.ConfirmSprayOff, false, 1);
          logToSerials(toText((int)AeroResponse1Received.AeroState), false, 1);
          logToSerials(AeroResponse1Received.PressureTankPresent, false, 1);
          logToSerials(AeroResponse1Received.SprayEnabled, false, 1);
          logToSerials(AeroResponse1Received.Pressure, false, 1);
          logToSerials(toText((int)AeroResponse1Received.PumpState), false, 1);
          logToSerials(AeroResponse1Received.LastSprayPressure, false, 1);
          logToSerials(AeroResponse1Received.Weight, true, 1);
        }
        if (AeroCommand1ToSend.SprayEnabled || AeroCommand1ToSend.SprayDisabled || AeroCommand1ToSend.SprayNow || AeroCommand1ToSend.SprayOff)
        {
          SyncRequested = true; ///< Force another message exchange when a command is active
        }
        if(AeroResponse1Received.ConfirmSprayEnabled) AeroCommand1ToSend.SprayEnabled = false;  //Turn off the Flag once the Receiver confirms processing it 
        if(AeroResponse1Received.ConfirmSprayDisabled) AeroCommand1ToSend.SprayDisabled = false;
        if(AeroResponse1Received.ConfirmSprayNow) AeroCommand1ToSend.SprayNow = false;
        if(AeroResponse1Received.ConfirmSprayOff) AeroCommand1ToSend.SprayOff = false;    
        break;
      case AeroMessages::AeroResponse2:
        memcpy(&AeroResponse2Received, ReceivedResponse, sizeof(struct AeroResponse_P2));
        if (*(Parent->SerialReportWireless))
        {
          logToSerials(AeroResponse2Received.ConfirmPumpOn, false, 1);
          logToSerials(AeroResponse2Received.ConfirmPumpOff, false, 1);
          logToSerials(AeroResponse2Received.ConfirmPumpDisable, false, 1);
          logToSerials(AeroResponse2Received.ConfirmMixReservoir, false, 1);
          logToSerials(AeroResponse2Received.ConfirmRefillPressureTank, false, 1);
          logToSerials(AeroResponse2Received.ConfirmDrainPressureTank, false, 1);
          logToSerials(AeroResponse2Received.ConfirmTareWeight, true, 1);
        }
        if (AeroCommand2ToSend.PumpOn || AeroCommand2ToSend.PumpOff || AeroCommand2ToSend.PumpDisable || AeroCommand2ToSend.MixReservoir || AeroCommand2ToSend.RefillPressureTank || AeroCommand2ToSend.DrainPressureTank || AeroCommand2ToSend.TareWeight)
        {
          SyncRequested = true; ///< Force another message exchange when a command is active
        }
        if(AeroResponse2Received.ConfirmPumpOn) AeroCommand2ToSend.PumpOn = false;  //Turn off the Flag once the Receiver confirms processing it 
        if(AeroResponse2Received.ConfirmPumpOff) AeroCommand2ToSend.PumpOff = false;
        if(AeroResponse2Received.ConfirmPumpDisable) AeroCommand2ToSend.PumpDisable = false;
        if(AeroResponse2Received.ConfirmMixReservoir) AeroCommand2ToSend.MixReservoir = false;
        if(AeroResponse2Received.ConfirmRefillPressureTank) AeroCommand2ToSend.RefillPressureTank = false;
        if(AeroResponse2Received.ConfirmDrainPressureTank) AeroCommand2ToSend.DrainPressureTank = false;
        if(AeroResponse2Received.ConfirmTareWeight) AeroCommand2ToSend.TareWeight = false;
        break;
      case AeroMessages::AeroReset:
        if (*(Parent->SerialReportWireless))
        {
          logToSerials(F("-"), true, 1);  ///< Reset messages does not have any data
        }
        break;
      default:
        logToSerials(F("SequenceID unknown"), true, 1);
        break;
      }
      LastResponseReceived = millis();
    }
    else
    {
      if (*(Parent->SerialReportWireless))
        logToSerials(F("; Ack received without data"), true, 1);
    }
  }
  else
  {
    if (*(Parent->SerialReportWireless))
      logToSerials(F("; No response"), true, 1);
    if (millis() - LastResponseReceived > WirelessReceiveTimeout)
    {
      OnlineStatus = false;
    }
  }
  return ReceivedSequenceID;
}

/**
* @brief Update the wireless command content
*/
void AeroModule_Web::updateCommands()
{
  AeroModuleCommand1ToSend.Time = now();
  AeroModuleCommand1ToSend.Debug = *Debug;
  AeroModuleCommand1ToSend.Metric = *Metric;
  AeroModuleCommand1ToSend.SerialReportFrequency = *(Parent->SerialReportFrequency);
  AeroModuleCommand1ToSend.SerialReportDate = *(Parent->SerialReportDate);
  AeroModuleCommand1ToSend.SerialReportMemory = *(Parent->SerialReportMemory);
  AeroModuleCommand1ToSend.SerialReportJSONFriendly = *(Parent->SerialReportJSONFriendly);
  AeroModuleCommand1ToSend.SerialReportJSON = *(Parent->SerialReportJSON);
  AeroModuleCommand1ToSend.SerialReportWireless = *(Parent->SerialReportWireless);
  AeroCommand1ToSend.DayMode = ((MainModule *)Parent)->getDayMode();
  AeroCommand1ToSend.Duration = DefaultSettings->Duration;
  AeroCommand1ToSend.DayInterval = DefaultSettings->DayInterval;
  AeroCommand1ToSend.NightInterval = DefaultSettings->NightInterval;
  AeroCommand1ToSend.MinPressure = DefaultSettings->MinPressure;
  AeroCommand1ToSend.MaxPressure = DefaultSettings->MaxPressure;
  AeroCommand2ToSend.PumpSpeed = DefaultSettings->PumpSpeed;
  AeroCommand2ToSend.PumpTimeOut = DefaultSettings->PumpTimeOut;
  AeroCommand2ToSend.PumpPrimingTime = DefaultSettings->PrimingTime;
}
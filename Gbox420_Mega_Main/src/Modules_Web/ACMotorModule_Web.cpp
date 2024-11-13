#include "ACMotorModule_Web.h"

struct ACMotorModuleCommand ACMotorModuleCommand1ToSend = {ACMotorMessages::ACMotorModuleCommand1};      ///< Command to send will be stored here
struct ACMotorModuleResponse ACMotorModuleResponse1Received = {ACMotorMessages::ACMotorModuleResponse1}; ///< Response will be stored here
struct ACMotorCommand ACMotorCommand1ToSend = {ACMotorMessages::ACMotorCommand1};      ///< Command to send will be stored here
struct ACMotorResponse ACMotorResponse1Received = {ACMotorMessages::ACMotorResponse1}; ///< Response will be stored here
struct ACMotorCommonTemplate ACMotorResetToSend = {ACMotorMessages::ACMotorReset};                       ///< Special command to fetch the next Response from the Receiver

/**
* @brief Constructor: creates an instance of the class, loads the EEPROM stored persistent settings and subscribes to events
*/
ACMotorModule_Web::ACMotorModule_Web(const __FlashStringHelper *Name, MainModule *Parent, Settings::ACMotorModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name), Parent(Parent)
{
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
  logToSerials(F("ACMotorModule_Web ready"), true, 3);
}

/**
* @brief Report current state in a JSON format to the LongMessage buffer
*/
void ACMotorModule_Web::report(bool FriendlyFormat)
{
  Common::report(true); ///< Adds "NAME":{  to the LongMessage buffer. The curly bracket { needs to be closed at the end
  strcat_P(LongMessage, (PGM_P)F("\"S\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_onlineStatus(OnlineStatus) : toText(OnlineStatus));
  strcat_P(LongMessage, (PGM_P)F("\",\"St\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_ACMotorState(ACMotorResponse1Received.ACMotorState) : toText((int)ACMotorResponse1Received.ACMotorState));
  strcat_P(LongMessage, (PGM_P)F("\",\"R\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_rpm(ACMotorResponse1Received.RPM) : toText((int)ACMotorResponse1Received.RPM));
  strcat_P(LongMessage, (PGM_P)F("\",\"S\":\""));
  strcat(LongMessage, FriendlyFormat ? toText_percentage(ACMotorCommand1ToSend.Speed) : toText(ACMotorCommand1ToSend.Speed));  
  strcat_P(LongMessage, (PGM_P)F("\"}")); ///< closing the curly bracket at the end of the JSON
}

void ACMotorModule_Web::websiteEvent_Load(__attribute__((unused)) char *Url)
{
  WebServer.setArgInt(getName(F("Sp"), true), ACMotorCommand1ToSend.Speed); 
}

void ACMotorModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *Url) ///< called when website is refreshed.
{
  WebServer.setArgString(getName(F("S"), true), toText_onlineStatus(OnlineStatus));
  WebServer.setArgString(getName(F("R"), true), toText_rpm(ACMotorResponse1Received.RPM));
  WebServer.setArgString(getName(F("St"), true), toText_ACMotorState(ACMotorResponse1Received.ACMotorState));
}

bool ACMotorModule_Web::commandEvent(char *Command, char *Data)
{
  if (!isThisMine(Command))
  {
    return false;
  }
  else
  {
    if (strcmp_P(ShortMessage, (PGM_P)F("F")) == 0)
    {
      ACMotorCommand1ToSend.Forward = true;
      Parent->addToLog(F("Motor forward"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("B")) == 0)
    {
      ACMotorCommand1ToSend.Backward = true;
      Parent->addToLog(F("Motor backard"), false);
    }
    else if (strcmp_P(ShortMessage, (PGM_P)F("Stp")) == 0)
    {
      ACMotorCommand1ToSend.Stop = true;
      Parent->addToLog(F("Motor stop"), false);
    }  
    else if (strcmp_P(ShortMessage, (PGM_P)F("Sp")) == 0)
    {
      DefaultSettings->Speed = toInt(Data);
      Parent->addToLog(F("Speed updated"), false);
    }  
    else
    {
      return false;
    }
    SyncRequested = true;
    return true;    
  }
}

void ACMotorModule_Web::refresh_Sec()
{
  Common::refresh_Sec();
  if (SyncRequested)
  {
    SyncRequested = false;
    //syncModule(WirelessChannel,&Command,&Response);
    sendMessages();
  }
}

void ACMotorModule_Web::refresh_FiveSec()
{
  Common::refresh_FiveSec();
  //syncModule(WirelessChannel,&Command,&Response);
  sendMessages();
}

void ACMotorModule_Web::refresh_Minute()
{
  Common::refresh_Minute();
}

void ACMotorModule_Web::sendMessages()
{
  updateCommands();
  sendCommand(&ACMotorResetToSend);          ///< special Command, resets communication to first message
  sendCommand(&ACMotorModuleCommand1ToSend); ///< Command - Response exchange
  sendCommand(&ACMotorCommand1ToSend); ///< Command - Response exchange
  if (Parent->SerialReportWireless && Debug)
  {
    logToSerials(F("Message exchange finished"), true, 1);
  }
}

ACMotorMessages ACMotorModule_Web::sendCommand(void *CommandToSend)
{
  ACMotorMessages SequenceIDToSend = ((ACMotorCommonTemplate *)CommandToSend)->SequenceID;
  ACMotorMessages ReceivedSequenceID;
  if (Parent->SerialReportWireless)
  {
    logToSerials(F("Sending:"), false, 1);
    logToSerials(toText_ACMotorSequenceID(SequenceIDToSend), false, 1);
  }
  Parent->Wireless->openWritingPipe(WirelessChannel);
  Parent->Wireless->flush_rx(); ///< Dump all previously received but unprocessed messages
  if (Parent->Wireless->write(CommandToSend, WirelessPayloadSize))
  {
    delay(50); ///< give a little time to the nRF024L01+ chip to update the isAckPayloadAvailable flag
    if (Parent->Wireless->isAckPayloadAvailable())
    {
      OnlineStatus = true; ///< Mark that the module responded
      Parent->Wireless->read(ReceivedResponse, WirelessPayloadSize);
      ReceivedSequenceID = ((ACMotorCommonTemplate *)ReceivedResponse)->SequenceID;
      if (Parent->SerialReportWireless)
      {
        logToSerials(F("; Response:"), false, 1);
        logToSerials(toText_ACMotorSequenceID(ReceivedSequenceID), false, 1);
        logToSerials(F("; Data:"), false, 1);
      }

      switch (ReceivedSequenceID)
      {
      case ACMotorMessages::ACMotorModuleResponse1:
        memcpy(&ACMotorModuleResponse1Received, ReceivedResponse, sizeof(struct ACMotorModuleResponse));
        if (Parent->SerialReportWireless)
        {
          logToSerials(ACMotorModuleResponse1Received.Status, true, 1);
        }
        break;
      case ACMotorMessages::ACMotorResponse1:
        memcpy(&ACMotorResponse1Received, ReceivedResponse, sizeof(struct ACMotorResponse));
        if (Parent->SerialReportWireless)
        {
          logToSerials(toText((int)ACMotorResponse1Received.ACMotorState), false, 1);
          logToSerials(ACMotorResponse1Received.RPM, true, 1);
        }
        if (ACMotorCommand1ToSend.Forward || ACMotorCommand1ToSend.Backward || ACMotorCommand1ToSend.Stop) ///< Command flag active
        {
          SyncRequested = true; ///< Force a second packet to actualize the response
        }
        if (ACMotorResponse1Received.ConfirmForward)
          ACMotorCommand1ToSend.Forward = false; //Turn off the Flag once the Receiver confirms processing it
        if (ACMotorResponse1Received.ConfirmBackward)
          ACMotorCommand1ToSend.Backward = false;
        if (ACMotorResponse1Received.ConfirmStop)
          ACMotorCommand1ToSend.Stop = false;
        break;
      case ACMotorMessages::ACMotorReset:
        if (Parent->SerialReportWireless)
        {
          logToSerials(F("-"), true, 1); ///< Reset messages does not have any data
        }
        break;
      default:
        if (Parent->SerialReportWireless)
        {
          logToSerials(F("SequenceID unknown"), true, 1);
        }
        break;
      }
      LastResponseReceived = millis();
    }
    else
    {
      if (Parent->SerialReportWireless)
        logToSerials(F("; Ack received without data"), true, 1); ///< Indicates a communication problem - Make sure to have bypass capacitors across the 3.3V power line and ground powering the nRF24L01+
    }
  }
  else
  {
    if (Parent->SerialReportWireless)
      logToSerials(F("; No response"), true, 1);
    if (millis() - LastResponseReceived > WirelessReceiveTimeout)
    {
      OnlineStatus = false; ///< Comment this out if you have modules that do not return any data
    }
  }
  return ReceivedSequenceID;
}

void ACMotorModule_Web::updateCommands()
{
  ACMotorModuleCommand1ToSend.Time = now();
  ACMotorModuleCommand1ToSend.Debug = Debug;
  ACMotorModuleCommand1ToSend.Metric = Metric;
  ACMotorModuleCommand1ToSend.SerialReportDate = Parent->SerialReportDate;
  ACMotorModuleCommand1ToSend.SerialReportMemory = Parent->SerialReportMemory;
  ACMotorModuleCommand1ToSend.SerialReportJSONFriendly = Parent->SerialReportJSONFriendly;
  ACMotorModuleCommand1ToSend.SerialReportJSON = Parent->SerialReportJSON;
  ACMotorModuleCommand1ToSend.SerialReportWireless = Parent->SerialReportWireless;
  ACMotorCommand1ToSend.Speed = DefaultSettings->Speed;
}
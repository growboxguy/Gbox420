#include "ReservoirModule_Web.h"

ReservoirModule_Web::ReservoirModule_Web(const __FlashStringHelper *Name, Module_Web *Parent,Settings::ReservoirModuleSettings *DefaultSettings) : Common(Name), Common_Web(Name)
{
  this->Parent = Parent;
  this->DefaultSettings = DefaultSettings;
  updateCommand();
  memcpy_P(this->WirelessChannel,(PGM_P)Name,sizeof(this->WirelessChannel));
  Parent->addToReportQueue(this);              
  Parent->addToRefreshQueue_FiveSec(this);             
  Parent->addToWebsiteQueue_Refresh(this);         
  logToSerials(F("ReservoirModule_Web object created"), true, 1);
}

void ReservoirModule_Web::report()
{
  Common::report();
  memset(&LongMessage[0], 0, sizeof(LongMessage)); ///clear variable
  strcat_P(LongMessage, (PGM_P)F("PH:"));
  strcat(LongMessage, toText(Response.PH));
  strcat_P(LongMessage, (PGM_P)F(" ; Water level distance:"));
  strcat(LongMessage, toText_distance(Response.Distance));
  strcat_P(LongMessage, (PGM_P)F(" ; Water temp:"));
  strcat(LongMessage, toText_temp(Response.WaterTemperature));
  strcat_P(LongMessage, (PGM_P)F(" ; Air temp:"));
  strcat(LongMessage, toText_temp(Response.AirTemperature));
  strcat_P(LongMessage, (PGM_P)F(" ; Humidity:"));
  strcat(LongMessage, toText_percentage(Response.Humidity));
  logToSerials(&LongMessage, true, 1);
}

void ReservoirModule_Web::reportToJSON()
{
    Common_Web::reportToJSON(); ///< Adds a curly bracket {  that needs to be closed at the end
    strcat_P(LongMessage, (PGM_P)F("\"PH\":\""));
    strcat(LongMessage, toText(Response.PH));   
    strcat_P(LongMessage, (PGM_P)F("\",\"Distance\":\""));
    strcat(LongMessage, toText(Response.Distance));     
    strcat_P(LongMessage, (PGM_P)F("\",\"WaterTemp\":\""));
    strcat(LongMessage, toText(Response.WaterTemperature));  
    strcat_P(LongMessage, (PGM_P)F("\",\"AirTemp\":\""));
    strcat(LongMessage, toText(Response.AirTemperature));   
    strcat_P(LongMessage, (PGM_P)F("\",\"Humidity\":\""));
    strcat(LongMessage, toText(Response.Humidity));
    strcat_P(LongMessage, (PGM_P)F("\"}"));  ///< closing the curly bracket
}

void ReservoirModule_Web::websiteEvent_Refresh(__attribute__((unused)) char *url) ///called when website is refreshed.
{
  if (strncmp(url, "/G",2) == 0)
  {
    WebServer.setArgString(getComponentName(F("PH")), toText(Response.PH));
    WebServer.setArgString(getComponentName(F("Dist")), toText_distance(Response.Distance));
    WebServer.setArgString(getComponentName(F("WTemp")),  toText_temp(Response.WaterTemperature));
    WebServer.setArgString(getComponentName(F("ATemp")), toText_temp(Response.AirTemperature));
    WebServer.setArgString(getComponentName(F("Humi")), toText_percentage(Response.Humidity));
  }
}

void ReservoirModule_Web::refresh_FiveSec()
{
  if (*Debug)
    Common::refresh_FiveSec();  
  syncModule(WirelessChannel,&Command,&Response);
}

void ReservoirModule_Web::syncModule( const byte WirelessChannel[], reservoirCommand *Command, reservoirResponse *Response){
  /**
   * @brief Send a command to the remote Reservoir Module and receive a response containing the module's current status
   */
  updateCommand();
  logToSerials(F("Sending sync command..."),false,3);
  Parent -> Wireless -> openWritingPipe(WirelessChannel);
  if (Parent -> Wireless -> write(Command, sizeof(*Command) )) {
      if ( Parent -> Wireless -> isAckPayloadAvailable() ) {
          Parent -> Wireless -> read(Response, sizeof(*Response));
           logToSerials(F("Acknowledgement received ["),false,2);            
          Serial.print(sizeof(*Response)); /// \todo Use LogToSerial
          logToSerials(F("bytes]"),true,1);

           if(*Debug){
          logToSerials(Response -> PH,false,3);
          logToSerials(F(","),false,1);
          logToSerials(Response -> Distance,false,1);
          logToSerials(F(","),false,1);
          logToSerials(Response -> WaterTemperature,false,1);
          logToSerials(F(","),false,1);
          logToSerials(Response -> AirTemperature,false,1);
          logToSerials(F(","),false,1);
          logToSerials(Response -> Humidity,true,1);       
          } 
      }
      else {
          Serial.println(F(" Acknowledgement received without any data."));
      }        
  }
  else{
      Serial.println(F(" No response."));
      }
  }


void ReservoirModule_Web::updateCommand() { 
  /**
   * @brief Updates the command sent to the remote Reservoir Module wirelessly
  */
  Command.Time = now();
  Command.Debug = *Debug;
  Command.Metric = *Metric;
}
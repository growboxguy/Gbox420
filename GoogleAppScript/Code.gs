/*
GrowBoxGuy - http:///sites.google.com/site/growboxguy/
JSON data receiver and Google Sheets logging

The file need to be added to Google Sheets, under Tools\Script editor
More info: https:///sites.google.com/site/growboxguy/arduino/logging
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///UPDATE THESE VALUES: The spreadsheet ID points to the Google Sheet spreadsheet document

var spreadSheetID = '162557mj4XdpdER_PC6Kbq_GGUgc5nV-MYzr7J0Ims4Q';  ///Update to the ID of the Log Spreadsheet based on the URL: https:///docs.google.com/spreadsheetURL/d/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/edit   (Only the XXXXX part is needed)
var timeZone = 'GMT+1';  ///Update this to match your timezone


///END OF STUFF TO UPDATE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///This is where data arrives for processing

function doPost(receivedData) {  ///ESP-link sends the Logs here through PushingBox. Needs a bit of parsing to make it a proper JSON
  Logger.log(receivedData);
  if(receivedData != null){  
    var spreadSheet = SpreadsheetApp.openById(spreadSheetID); ///Open the spreadsheet
    var statusSheet = spreadSheet.getSheetByName("Status");  ///Get the Status sheet and log the raw received data
    statusSheet.getRange(2, 4).setValue(receivedData.parameter.BoxData);
    var dateFormat = getSettingsValue("DateFormat");
    statusSheet.getRange(7, 5).setValue(Utilities.formatDate(new Date(), timeZone, dateFormat));
    statusSheet.getRange(8, 5).setValue("NO");      
    var BoxData = JSON.parse(receivedData.parameter.BoxData);
    statusSheet.getRange(8, 5).setValue("YES")    
    if(BoxData.Settings != null) UpdateSettings(BoxData.Settings);
    if(BoxData.Log != null) ProcessLog(BoxData.Log);    
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Normal flow of data processing

function ProcessLog(Log){
  Logger.log(Log);  ///Log the received data on the console output  
  SaveToLog(Log);  ///Save the log to the Logs sheet on the spreadsheet specified by spreadSheetID
  UpdateAlerts(Log); ///Add missing columns to the Alert settings sheet
  UpdateCharts();  ///Updates the chart datasources in case the columns were reordered
  CheckAlerts(Log);  ///Checks for alerts and send an email alert
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Updates the Settings sheets, adds newly discovered keys 

function UpdateSettings(Settings){
  Logger.log(Settings);  ///Log the received data on the console output  
  var spreadSheet = SpreadsheetApp.openById(spreadSheetID); ///Open the spreadsheet
  var settingsSheet = spreadSheet.getSheetByName("Settings");   ///Get the Settings sheet
  var settings = settingsSheet.getRange(3,1,settingsSheet.getLastRow(),1);
  var nextRow = settingsSheet.getLastRow() + 1; ///Get next row after the last row
  
  ///Add all other Header + Data pairs from the received Log JSON
  for each (var key in Object.keys(Settings)){
    var match = settings.createTextFinder(key).matchEntireCell(true).findNext(); ///lookup the alert limit row for each Log key
    if(match == null){ ///If settings row does not exists
      settingsSheet.getRange(nextRow, 1).setValue(key);   ///Insert key in first column     
      settingsSheet.getRange(nextRow, 2).setValue(Settings[key]);   ///Insert value in second column
      nextRow++;
    } 
    else
    {
      settingsSheet.getRange(match.getRow(), 2).setValue(Settings[key]);   ///Update value in second column
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Test functions with fake data for running specific steps of the data processing

function ProcessLog_Fake(){  ///For testing only: Pass a properly formated JSON to processing
  var FakeData = {"InternalTemp":"22.10","ExternalTemp":"22.62","InternalHumidity":"43.41","ExternalHumidity":"41.52","InternalFan":"0","ExhaustFan":"0","Lt1_Status":"0","Lt1_Brightness":"25","LightReading":"-296","Dark":"1","WaterLevel":"0","WaterTemp":"18.35","PH":"5.37","Pressure":"0.12","Power":"0.00","Energy":"391.58","Voltage":"222.40","Current":"0.00","Lt1_Timer":"1","Lt1_OnTime":"19:26","Lt1_OffTime":"19:28","AeroInterval":"15","AeroDuration":"2"};
  FakeData.replace
  ProcessLog(FakeData);
}

function UpdateAlerts_Fake(){  ///For testing only: Pass a properly formated JSON to processing
  var FakeData = {"InternalTemp":"19.80","ExternalTemp":"22.50","InternalHumidity":"46.50","ExternalHumidity":"47.20","InternalFan":"1","ExhaustFan":"0","Lt1_Status":"1","Lt1_Brightness":"95","LightReading":"95.00","Dark":"0","WaterLevel":"3","WaterTemp":"20.06","PH":"5.96","Pressure":"6.73","Power":"445.00","Energy":"10.00","Voltage":"230","Current":"1.80","Frequency":"-1.00","PowerFactor":"1.00","Lt1_Timer":"1","Lt1_OnTime":"04:20","Lt1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"};
  UpdateAlerts(FakeData);
}

function CheckAlerts_Fake(){  ///For testing only: Pass a properly formated JSON to processing
  var FakeData = {"InternalTemp":"19.80","ExternalTemp":"22.50","InternalHumidity":"46.50","ExternalHumidity":"47.20","InternalFan":"1","ExhaustFan":"0","Lt1_Status":"1","Lt1_Brightness":"95","LightReading":"95.00","Dark":"0","WaterLevel":"3","WaterTemp":"20.06","PH":"5.96","Pressure":"6.73","Power":"445.00","Energy":"10.00","Voltage":"230","Current":"1.80","Frequency":"-1.00","PowerFactor":"1.00","Lt1_Timer":"1","Lt1_OnTime":"04:20","Lt1_OffTime":"16:20","AeroInterval":"15","AeroDuration":"2"};
  CheckAlerts(FakeData);
}

function UpdateSettings_Fake(){
  var FakeData = {"Metric":"1"};
  UpdateSettings(FakeData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Helper function: Retrievs the value of a setting from the Settings sheet

function getSettingsValue(settingName){
  var spreadSheet = SpreadsheetApp.openById(spreadSheetID); ///Open the spreadsheet
  var settingsSheet = spreadSheet.getSheetByName("Settings");  ///Get the Log sheet
  var settingsKeys = settingsSheet.getRange(3,1,settingsSheet.getLastRow(),1)  ///getRange(row, column, numRows, numColumns),
  var match = settingsKeys.createTextFinder(settingName).matchEntireCell(true).findNext();
  if(match != null){
    return settingsSheet.getRange(match.getRow(),match.getColumn() + 1).getDisplayValue();
  }
  else
    return null;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Stores the received data on the Log sheet

function SaveToLog(Log) {
  var spreadSheet = SpreadsheetApp.openById(spreadSheetID); ///Open the spreadsheet
  var logSheet = spreadSheet.getSheetByName("Log");  ///Get the Log sheet
  var alertsSheet = spreadSheet.getSheetByName("Alerts");   ///Get the Settings sheet
  
  var dateFormat = getSettingsValue("DateFormat");  
  var headers = logSheet.getRange(1,1,1,logSheet.getLastColumn());
  var nextFreeRow = logSheet.getLastRow() + 1;  
  
  ///Add logged time to the first column : Date and time when the log was received
  logSheet.getRange(1, 1).setValue("LogDate");
  logSheet.getRange(nextFreeRow, 1).setValue(Utilities.formatDate(new Date(), timeZone, dateFormat));
  
  ///Add all other Header + Data pairs from the received Log JSON
  for each (var key in Object.keys(Log)){
    var match = headers.createTextFinder(key).matchEntireCell(true).findNext(); ///lookup the alert limit row for each Log key
    if(match != null){ ///If colum header exists
      logSheet.getRange(nextFreeRow,match.getColumn()).setValue(Log[key]);   ///Insert data to last row, into the matched column   
    }
    else  ///If column is new:
    {
      logSheet.getRange(1,logSheet.getLastColumn() + 1).setValue(key);  ///Create a new column at the end and add the header
      logSheet.getRange(nextFreeRow,logSheet.getLastColumn()).setValue(Log[key]); ///Insert data to last row, into the new column
    } 
  }
  
  ///Apply some formatting
  logSheet.getRange(1,1,1,logSheet.getLastColumn()).setFontWeight("bold");  ///Set header bold
  logSheet.setFrozenRows(2); ///sets headers row frozen to the top
  logSheet.autoResizeColumns(1, logSheet.getLastColumn()); ///resize columns to fit the data  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Updates the dataRange of each chart on the Charts sheet

function UpdateCharts(){ 
  var spreadSheet = SpreadsheetApp.openById(spreadSheetID); ///Open the spreadsheet
  var chartsSheet = spreadSheet.getSheetByName("Charts");  ///Get the Charts sheet
  var logSheet = spreadSheet.getSheetByName("Log");  ///Get the Log sheet
  var headers = logSheet.getRange(1,1,1,64);  ///getRange(row, column, numRows, numColumns), gets the area where log headers are stored (A1:A64), up to 64 components.
  
  var tempAndHumidityChart = chartsSheet.getCharts()[1];
  var tempAndHumidityChart_updated = tempAndHumidityChart
  .modify()
  .clearRanges()
  .addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("InternalTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("ExternalTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("WaterTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("InternalHumidity").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("ExternalHumidity").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .build();
  chartsSheet.updateChart(tempAndHumidityChart_updated);  
  
  var powerUsageChart = chartsSheet.getCharts()[2];
  var powerUsageChart_updated = powerUsageChart
  .modify()
  .clearRanges()
  .addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("Power").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("Energy").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("Lt1_Brightness").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .build();
  chartsSheet.updateChart(powerUsageChart_updated);
  
  var phAndLightStatusChart = chartsSheet.getCharts()[3];
  var phAndLightStatusChart_updated = phAndLightStatusChart
  .modify()
  .clearRanges()
  .addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("PH").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("Lt1_Status").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("WaterLevel").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .build();
  chartsSheet.updateChart(phAndLightStatusChart_updated);
  
  var pressureAndWaterLevelChart = chartsSheet.getCharts()[4];
  var pressureAndWaterLevelChart_updated = pressureAndWaterLevelChart
  .modify()
  .clearRanges()
  .addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("Pressure").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .build();
  chartsSheet.updateChart(pressureAndWaterLevelChart_updated);
  
  
  var fanChart = chartsSheet.getCharts()[0];
  var fanChart_updated = fanChart
  .modify()
  .clearRanges()
  .addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("InternalFan").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,headers.createTextFinder("ExhaustFan").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .build();
  chartsSheet.updateChart(fanChart_updated);   
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Updates the Alerts sheets, adds newly discovered keys 

function UpdateAlerts(Log){
  var spreadSheet = SpreadsheetApp.openById(spreadSheetID); ///Open the spreadsheet
  var alertsSheet = spreadSheet.getSheetByName("Alerts");   ///Get the Settings sheet
  var settings = alertsSheet.getRange(3,1,alertsSheet.getLastRow(),1);
  var nextRow = alertsSheet.getLastRow() + 1; ///Get next row after the last row
  
  ///Add all other Header + Data pairs from the received Log JSON
  for each (var key in Object.keys(Log)){
    var match = settings.createTextFinder(key).matchEntireCell(true).findNext(); ///lookup the alert limit row for each Log key
    if(match == null){ ///If settings row does not exists
      alertsSheet.getRange(nextRow, 1).setValue(key);   ///Insert key in first Key column     
      var rule = SpreadsheetApp.newDataValidation().requireValueInList(['YES', 'NO']).build();
      alertsSheet.getRange(nextRow, 4).setDataValidation(rule); ///Add YES/NO options for Triggered column
      alertsSheet.getRange(nextRow, 4).setHorizontalAlignment("right");
      nextRow++;
    }      
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Triggers/Resolves email alerts based on the Alerts sheet settings

function CheckAlerts(Log) {
  var spreadSheet = SpreadsheetApp.openById(spreadSheetID); ///Open the spreadsheet
  var alertsSheet = spreadSheet.getSheetByName("Alerts");   ///Get the Settings sheet
  var chartsSheet = spreadSheet.getSheetByName("Charts");  ///Get the Charts sheet
  var statusSheet = spreadSheet.getSheetByName("Status");  ///Get the Log sheet
  
  var statusReport = statusSheet.getRange(2, 1, statusSheet.getLastRow(), 2);  ///getRange(row, column, numRows, numColumns)  
  var alerts = []; ///For storing alert keys
  var alertMessages = [];  ///For storing alert messages
  var recoveredMessages = [];  ///For storing recovery messages  
  var sendEmailAlert = false;  ///Initially assume everything is OK
  
  var alertEmailEnabled = getSettingsValue("AlertEmailEnabled") == 'YES';
  var alertEmailAddress = getSettingsValue("AlertEmail");  
  var alwaysSendEmail = getSettingsValue("AlwaysSendEmail")  == 'YES'; ///Send email when a report is received, even if there are no active/recovered alerts
  
  var alertLimits = alertsSheet.getRange(3, 1, alertsSheet.getLastRow(), 1);  ///getRange(row, column, numRows, numColumns), gets the area where alert limits are stored (A4:D20)
  for each (var key in Object.keys(Log)){
    var match = alertLimits.createTextFinder(key).matchEntireCell(true).findNext(); ///lookup the alert limit row for each Log key
    if(match != null){ ///If Log entry has a Min/Max limit set
      var minLimit = alertsSheet.getRange(match.getRow(),2).getValue(); ///get the 2nd column value from the matched row: Minimum limit
      var maxLimit = alertsSheet.getRange(match.getRow(),3).getValue(); ///get the 3rd column value from the matched row: Maximum limit
      if((minLimit != "" && maxLimit != "" && (Log[key] < minLimit || maxLimit < Log[key]))  || (minLimit == "" && maxLimit != "" && maxLimit < Log[key]) || (maxLimit == "" && minLimit != ""&&  Log[key] < minLimit)){ ///if reading was out of limits: activate alert
        alerts.push(key);
        if(alertsSheet.getRange(match.getRow(),4).getValue() != 'YES'){ ///if alert is new
          alertMessages.push("<strong>" +"[NEW] " + "</strong><font color='red'>" + key + "</font> out of limits: <strong>" + Log[key] + "</strong> [" + alertsSheet.getRange(match.getRow(),2).getDisplayValue() + " / " + alertsSheet.getRange(match.getRow(),3).getDisplayValue() + "]"); ///save new alerts for the email
          sendEmailAlert = true; ///Send notification that a parameter is out of range 
        }
        else alertMessages.push("<font color='red'>" + key + "</font> out of limits: <strong>" + Log[key] + "</strong> [" + alertsSheet.getRange(match.getRow(),2).getDisplayValue() + " / " + alertsSheet.getRange(match.getRow(),3).getDisplayValue() + "]");    ///save active alerts for the email       
        
        ///Mark alert active
        alertsSheet.getRange(match.getRow(),4).setValue('YES');  ///Triggered column       
        statusMatch = statusReport.createTextFinder(key).matchEntireCell(true).findNext()
        if(statusMatch != null) statusSheet.getRange(statusMatch.getRow(),statusMatch.getColumn() + 1).setFontColor('red');
      }
      else {          
        if(alertsSheet.getRange(match.getRow(),4).getValue() != 'NO'){ ///if alert was active before
          recoveredMessages.push("<font color='green'>" + key + "</font> recovered: <strong>" + Log[key] + "</strong> [" + alertsSheet.getRange(match.getRow(),2).getDisplayValue() + " / " + alertsSheet.getRange(match.getRow(),3).getDisplayValue() + "]"); /// ///save recovered alerts for the email
          sendEmailAlert = true; ///Send notification that a parameter recovered
        }
        
        ///Mark alert inactive
        alertsSheet.getRange(match.getRow(),4).setValue('NO'); ///Triggered column                  
        statusMatch = statusReport.createTextFinder(key).matchEntireCell(true).findNext()
        if(statusMatch != null) statusSheet.getRange(statusMatch.getRow(),statusMatch.getColumn() + 1).setFontColor('black');
      }
      
    }
  }    
  if(alertEmailEnabled && (sendEmailAlert || alwaysSendEmail)) {
    var emailTemplate = HtmlService.createTemplateFromFile('AlertEmailTemplate');  ///Prepare the AlertEmailTemplate.html template
    emailTemplate.Alerts = alerts;  ///Fill Alert messages into the template
    emailTemplate.AlertMessages = alertMessages;  ///Fill Alert messages into the template
    emailTemplate.RecoveredMessages = recoveredMessages; ///Fill Recovered messages into the template
    emailTemplate.Data = statusReport.getDisplayValues();    
    var emailMessage = emailTemplate.evaluate().getContent();
    
    ///This section converts the charts to images and attaches them to the email message
    var emailImages={};
    var charts = chartsSheet.getCharts();
    var chartBlobs=new Array(charts.length);
    var chartOrder = [1,2,3,4,0];  ///display the charts in this order
    
    for each(var chartNumber in chartOrder){
      var builder = charts[chartNumber].modify();
      builder.setOption('chartArea',{width:'90%',height:'75%',bottom:'17%',left:'8%'});  ///{left:10,left:10,width:'90%',height:'75%'}
      builder.setOption('width',1024);
      builder.setOption('height',768);
      builder.setOption('maxLines',3);
      builder.setOption('legend',{position: 'top', textStyle: {fontSize: 18}});
      builder.setOption('titleTextStyle',{ fontSize: 22, bold: true });   /// { color: <string>, fontName: <string>, fontSize: <number>, bold: <boolean>, italic: <boolean> }
      
      var newchart = builder.build();
      chartBlobs[chartNumber]= newchart.getAs('image/png');
      emailMessage= emailMessage + "<img style='width:100%' src='cid:chart"+chartNumber+"'>";
      emailImages["chart"+chartNumber]= chartBlobs[chartNumber];
    }
    
    ///This section sends out the email message      
    MailApp.sendEmail({
      to: alertEmailAddress,
      subject: "Growbox report",
      htmlBody: emailMessage,
      inlineImages:emailImages
    });     
  }
}
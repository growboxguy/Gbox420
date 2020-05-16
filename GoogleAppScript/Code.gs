/*
GrowBoxGuy - http://sites.google.com/site/growboxguy/
JSON data receiver and Google Sheets logging

The file need to be added to Google Sheets, under Tools\Script editor
More info: https://sites.google.com/site/growboxguy/arduino/logging
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//UPDATE THESE VALUES: The spreadsheet ID points to the Google Sheet spreadsheet document

var spreadSheetID = '162557mj4XdpdER_PC6Kbq_GGUgc5nV-MYzr7J0Ims4Q';  //Update to the ID of the Log Spreadsheet based on the URL: https://docs.google.com/spreadsheetURL/d/XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX/edit   (Only the XXXXX part is needed)

//END OF STUFF TO UPDATE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

var FakeJSONData = {"Log":{"IFan":{"Speed":"1"},"EFan":{"Speed":"0"},"Lt1":{"Status":"1","Brightness":"15","Timer":"1","On":"04:20","Off":"16:20"},"LtSen1":{"Reading":"834","Dark":"0"},"Pow1":{"Power":"25.00","Energy":"10.00","Voltage":"231.00","Current":"1.00"},"DHT1":{"Temp":"21.00","Humidity":"46.00"},"Hemp1":{"PumpB1":"1","WeightB1":"4.76","StartB1":"4.20","StopB1":"6.90","PumpB2":"4","WeightB2":"4.19","StartB2":"4.20","StopB2":"6.90"},"Aero1":{"Pressure":"6.66","LastSpray":"6.00","SprayEnabled":"1","Interval":"15","Duration":"10"},"Res1":{"PH":"6.25","Distance":"16.04","WaterTemp":"21.25","AirTemp":"21.30","Humidity":"37.80"},"Main1":{"Metric":"1","Debug":"1"}}}
var FakeJSONData_LightCalibration = {"LightSensor1":{"Readings":[352,710,850,900,923,936,945,952,957,962,965]}}
  
var spreadSheet = SpreadsheetApp.openById(spreadSheetID); //Open the spreadsheet
var statusSheet = spreadSheet.getSheetByName("Status");   //Gets the Status sheet
var chartsSheet = spreadSheet.getSheetByName("Charts");  //Get the Charts sheet
var settingsSheet = spreadSheet.getSheetByName("Settings");   //Gets the Settings sheet
var logSheet = spreadSheet.getSheetByName("Log");   //Get the Log sheet
var calibrationSheet = spreadSheet.getSheetByName("Calibration");   //Get the Calibration sheet

//Loading headers for specific parts of tabs - getRange(row, column, numRows, numColumns)
var settings = settingsSheet.getRange(3,9,settingsSheet.getLastRow(),1);  //Settins sheet - 3rd row, 9th column, all rows, one column
var alerts = settingsSheet.getRange(3,1,settingsSheet.getLastRow(),1);   //Settins sheet - 3rd row, 1st column, all rows, one column
var latestStatus = statusSheet.getRange(3,1,statusSheet.getLastRow(),1);  //Status sheet - 3rd row - 1st column, all rows, one column
var logs = logSheet.getRange(1,1,2,logSheet.getLastColumn());  //Logs Sheet - 1st row, 1st column, two rows , all columns


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This is where data arrives for processing
function doPost(receivedData) {  
  Logger.log(receivedData);
  if(receivedData != null)
  {    
    if(receivedData.parameter.BoxData != null)
    {  //Only Start processing if the report contains a BoxData parameter
      var BoxData = JSON.parse(receivedData.parameter.BoxData);
      ProcessBoxData(BoxData);      
    } 
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Trigger a fake JSON report for testing

function Test_MainModule_ProcessBoxData(){  
  ProcessBoxData(FakeJSONData);			 
}

function Test_MainModule_CheckAlerts(){
  CheckAlerts(FakeJSONData.Log);			 
}

function Test_MainModule_SaveToLog(){
  SaveToLog(FakeJSONData.Log);			 
}

function Test_MainModule_UpdateStatus(){
  UpdateStatus(FakeJSONData.Log);			 
}

function Test_LightSensor1_ProcessBoxData(){
  ProcessBoxData(FakeJSONData_LightCalibration);			 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Normal flow of data processing

function ProcessBoxData(BoxData){
  var JSONBoxData = UpdateStatusRaw(BoxData);  //Updates the Status page and returns BoxData as a JSON object
  if(JSONBoxData.Log != null)
  {
    SaveToLog(JSONBoxData.Log);  //Save the log to the Logs sheet on the spreadsheet specified by spreadSheetID
    UpdateAlerts(JSONBoxData.Log); //Add missing columns to the Alert settings sheet
    UpdateStatus(JSONBoxData.Log); //Add formatted status of each component to the Status page   
    CheckAlerts(JSONBoxData.Log);  //Checks for alerts and send an email alert
    UpdateCharts();  //Updates the chart datasources in case the columns were reordered
  }
  else{
     MailApp.sendEmail({
      to: GetSettingsValue("Email address"),
      subject: "GrowBox report empty",
      htmlBody: JSONBoxData.Log
    });     
  }
  // if(JSONBoxData.GBox1.Settings != null) {
  //   UpdateSettings(JSONBoxData.GBox1.Settings);
  // }  
  
  if(JSONBoxData.LightSensor1 != null){
    UpdateCalibrationValues("LightSensor1",JSONBoxData.LightSensor1.Readings);
  }   
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the Status page with the received JSON raw value
function UpdateStatusRaw(BoxData)
{
  statusSheet.getRange(4, 5).setValue(BoxData);
  statusSheet.getRange(2, 5).setValue(Utilities.formatDate(new Date(), GetSettingsValue("Time zone"), GetSettingsValue("Date format")));  //Log when the JSON was received
  statusSheet.getRange(3, 5).setValue("NO"); //Set Valid JSON initially to not valid  
  var ParsedBoxData = JSON.parse(JSON.stringify(BoxData));
  statusSheet.getRange(3, 5).setValue("YES"); //If parsing did not fail set Valid JSON to true
  return ParsedBoxData;
}

function UpdateStatus(Log)
{ 
  var nextRow = statusSheet.getLastRow() + 1; //Get next row after the last row
  
  //Add all other Header + Data pairs from the received Log JSON
  for each (var Component in Object.keys(Log)){ 
    var Report = JSON.parse(JSON.stringify(Log[Component]));
    for each (var ReportEntry in Object.keys(Report)){  
      
      var match = latestStatus.createTextFinder(Component + '_' + ReportEntry).matchEntireCell(true).findNext();  //Trying to find Status entry with the relayed component name
      if(match != null){ //If status row is found       
        statusSheet.getRange(match.getRow(), 2).setValue(GetLatestReading(Component + '_' + ReportEntry,true));   //Insert latest reading in second column         
      }
      else{        
        statusSheet.getRange(nextRow, 1).setValue(Component + '_' + ReportEntry);   //Insert latest reading in second column 
        statusSheet.getRange(nextRow, 2).setValue(GetLatestReading(Component + '_' + ReportEntry,true));   //Insert latest reading in second column 
        nextRow++;
      }      
    }
  }
  //statusSheet.autoResizeColumns(1, statusSheet.getLastColumn()); //resize columns to fit the data 
}

function GetFriendlyName(key){
  var match = alerts.createTextFinder(key).matchEntireCell(true).findNext();   // \todo: Alerts is not a good name for the range
  if(match == null){ //If key is not found
    return key;
  }
  else {
    if(settingsSheet.getRange(match.getRow(), 7).isBlank()) {
      return key;
    }
    else
    {
      return settingsSheet.getRange(match.getRow(), 7).getValue();
    }      
  }  
}

function GetLatestReading(key,useFriendlyFormat){
  var match = logs.createTextFinder(key).matchEntireCell(true).findNext();
  if(match == null){ //If key is not found
    return null;
  }
  else {
    var rawValue = logSheet.getRange(logSheet.getLastRow(), match.getColumn()).getValue();
    
    if(useFriendlyFormat)
    {
      var settignsMatch = alerts.createTextFinder(key).matchEntireCell(true).findNext();  // \todo: Alerts is not a good name for the range
      
      if(settignsMatch == null){ //If key is not found
        return rawValue;
      }
      else {
        var dataType = settingsSheet.getRange(settignsMatch.getRow(),6).getValue();
        
        switch (dataType)
        {
          case "Date":
            return Utilities.formatDate(rawValue,GetSettingsValue("Time zone"),GetSettingsValue("Date format")) ;  //https://docs.oracle.com/javase/7/docs/api/java/text/SimpleDateformat.html
            break;
          case "Distance":
            if(GetSettingsValue("Units") == "Metric")
            {
              return rawValue+' cm';
            }
            else
            {
              return rawValue+' inch';
            }
            break;
          case "Current":
            return rawValue+' A';
            break;
          case "EnabledDisabled":
            switch (rawValue)  {
              case 0:
                return "Disabled";
                break;
              case 1:
                return "Enabled";
                break;              
            }
            break;
          case "Energy":
            return rawValue+' kWh';
            break;
          case "FanSpeed":
            switch (rawValue)  {
              case 0:
                return "OFF";
                break;
              case 1:
                return "LOW";
                break;
              case 2:
                return "HIGH";
                break; 
            }
            break;
          case "Minute":
            return rawValue+' min';
            break;
          case "OnOff":
            switch (rawValue)  {
              case 0:
                return "OFF";
                break;
              case 1:
                return "ON";
                break;              
            }
            break;
          case "Percentage":
            return rawValue+' %';
            break;
          case "Power":
            return rawValue+' W';
            break;
          case "PumpState":
            switch (rawValue)  {
              case 0:
                return "DISABLED";
                break;
              case 1:
                return "IDLE";
                break;
              case 2:
                return "PRIMING";
                break;
              case 3:
                return "RUNNING";
                break;
              case 4:
                return "BLOWOFF";
                break;
              case 5:
                return "MIXING";
                break;
            }
            break;
          case "Pressure":
            if(GetSettingsValue("Units") == "Metric")
            {
              return rawValue+' bar';
            }
            else
            {
              return rawValue+' psi';
            }
            break;
          case "Second":
            return rawValue+' sec';
            break;
          case "Temperature":
            if(GetSettingsValue("Units") == "Metric")
            {
              return rawValue+' °C';
            }
            else
            {
              return rawValue+' °F';
            }
            break;         
          case "Time":
            return Utilities.formatDate(rawValue,GetSettingsValue("Time zone"),GetSettingsValue("Time format"));
            break; 
          case "YesNo":
            switch (rawValue)  {
              case 0:
                return "NO";
                break;
              case 1:
                return "YES";
                break;              
            }
            break;
          case "Voltage":
            return rawValue+' V';
            break;
          case "Weight":
            if(GetSettingsValue("Units") == "Metric")
            {
              return rawValue+' kg';
            }
            else
            {
              return rawValue+' lbs';
            }
            break;
          default:   //Number and Text return as they are
            return rawValue;
            break;
        }
        
      }
    }
    else {
      return rawValue;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the Settings sheets, adds newly discovered keys 
/*
function UpdateSettings(Settings){
Logger.log(Settings);  //Log the received data on the console output  
var settings = settingsSheet.getRange(3,9,settingsSheet.getLastRow(),1);
var nextRow = settingsSheet.getLastRow() + 1; //Get next row after the last row

//Add all other Header + Data pairs from the received Log JSON
for each (var key in Object.keys(Settings)){
var match = settings.createTextFinder(key).matchEntireCell(true).findNext(); //lookup the alert limit row for each Log key
if(match == null){ //If settings row does not exists
settingsSheet.getRange(nextRow, 1).setValue(key);   //Insert key in first column     
settingsSheet.getRange(nextRow, 2).setValue(Settings[key]);   //Insert value in second column
nextRow++;
} 
else
{
settingsSheet.getRange(match.getRow(), 2).setValue(Settings[key]);   //Update value in second column
}
}
}*/

function UpdateCalibrationValues(Name,Readings){ //NOT READY
  Logger.log(Readings);  //Log the received data on the console output  
  var settings = calibrationSheet.getRange(3,1,calibrationSheet.getLastRow(),1);
  var nextRow = calibrationSheet.getLastRow() + 1; //Get next row after the last row
  
  //Add all other Header + Data pairs from the received Log JSON
  var match = settings.createTextFinder(Name).matchEntireCell(true).findNext(); //lookup the alert limit row for each Log key
  if(match == null){ //If settings row does not exists
    calibrationSheet.getRange(nextRow, 1).setValue(Name);   //Insert key in first column     
    calibrationSheet.getRange(nextRow, 2).setValue(Readings);   //Insert value in second column
    nextRow++;
  } 
  else
  {
    calibrationSheet.getRange(match.getRow(), 2).setValue(Readings);   //Update value in second column
  }
  
  for(var i =0 ; i < Readings.length;i++){
    calibrationSheet.getRange(3+i,5).setValue(Readings[i]);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper function: Retrievs the value of a setting from the Settings sheet

function GetSettingsValue(settingName){
  var settingsKeys = settingsSheet.getRange(3,9,settingsSheet.getLastRow(),1)  //getRange(row, column, numRows, numColumns),
  var match = settingsKeys.createTextFinder(settingName).matchEntireCell(true).findNext();
  if(match != null){
    return settingsSheet.getRange(match.getRow(),match.getColumn() + 1).getDisplayValue();
  }
  else
    return null;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Stores the received data on the Log sheet

function SaveToLog(Log) {  
  //Add logged time to the first column : Date and time when the log was received
  logSheet.getRange(1, 1).setValue("LogDate");
  logSheet.getRange(2, 1).setValue(" ");
  
  var nextFreeRow = logSheet.getLastRow() + 1; 
  logSheet.getRange(nextFreeRow, 1).setValue(Utilities.formatDate(new Date(), GetSettingsValue("Time zone"), GetSettingsValue("Date format")));
    
  //Add all other Header + Data pairs from the received Log JSON
  for each (var Component in Object.keys(Log)){ 
    var Report = JSON.parse(JSON.stringify(Log[Component]));
    for each (var ReportEntry in Object.keys(Report)){    
      var match = logs.createTextFinder(Component + '_' + ReportEntry).matchEntireCell(true).findNext(); //lookup the alert limit row for each Log key
      if(match != null){ //If colum header exists
        logSheet.getRange(nextFreeRow,match.getColumn()).setValue(Log[Component][ReportEntry]);   //Insert data to last row, into the matched column
        logSheet.getRange(1,match.getColumn()).setValue(GetFriendlyName(Component + '_' + ReportEntry));  //Keep Friendly name up to date
      }
      else  //If column is new:
      {
        logSheet.getRange(1,logSheet.getLastColumn() + 1).setValue(GetFriendlyName(Component + '_' + ReportEntry));  //Create a new column at the end and add the Frendly header
        logSheet.getRange(2,logSheet.getLastColumn()).setValue(Component + '_' + ReportEntry);  //Create a new column at the end and add the Component name header
        logSheet.getRange(nextFreeRow,logSheet.getLastColumn()).setValue(Log[Component][ReportEntry]); //Insert data to last row, into the new column
      }
    }
  }
  
  //Apply some formatting
  logSheet.getRange(1,1,1,logSheet.getLastColumn()).setFontWeight("bold");  //Set header bold
  logSheet.setFrozenRows(2); //sets headers row frozen to the top
  logSheet.autoResizeColumns(1, logSheet.getLastColumn()); //resize columns to fit the data  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the dataRange of each chart on the Charts sheet

function UpdateCharts(){    //https://developers.google.com/apps-script/reference/spreadsheet/embedded-chart
  
  //Clear all charts
  var charts = chartsSheet.getCharts();
  for (var i in charts) {
    chartsSheet.removeChart(charts[i]);
  }
  
  //Lights chart
  var chartBuilder = chartsSheet.newChart();
  var chart_Lights = chartBuilder
  .setPosition(200,1,0,0)
  .setChartType(Charts.ChartType.COMBO)
  .setNumHeaders(1)
  .addRange(logSheet.getRange(1,logs.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))  
  .addRange(logSheet.getRange(1,logs.createTextFinder("LtSen1_Reading").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Lt1_Brightness").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .setOption('title', 'LED lights')
  .setOption('width', 800)
  .setOption('height', 600)  
  .setOption('series', {
     0:{type: 'line', color: 'orange',targetAxisIndex:0},
         1:{type: 'line', color: 'green', targetAxisIndex:1}   
    })
   .setOption('vAxes', {0: {title: 'Light reading'},1: {title: 'Brightness %'}})
  .setOption('hAxis', {slantedText: true , 'slantedTextAngle':30})
  .build();  
  chartsSheet.insertChart(chart_Lights);
  
  //PowerConsumption chart
  var chartBuilder = chartsSheet.newChart();
  var chart_PowerConsumption = chartBuilder
  .setPosition(160,1,0,0)
  .setChartType(Charts.ChartType.COMBO)
  .setNumHeaders(1)
  .addRange(logSheet.getRange(1,logs.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Pow1_Power").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Pow1_Energy").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Pow1_Voltage").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Pow1_Current").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .setOption('title', 'Power consumption')
  .setOption('width', 800)
  .setOption('height', 600)
  .setOption('vAxes', {0: {viewWindow: {min: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()-1, 
                                        max: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()+1
                                       }}})
   .setOption('series', {
      0:{type: 'bars', color: 'blue'},
      1:{type: 'line', color: 'red'},
      2:{type: 'line', color: 'orange'},
      3:{type: 'line', color: 'green'}
    })
  .setOption('hAxis', {slantedText: true , 'slantedTextAngle':30})
  .build();  
  chartsSheet.insertChart(chart_PowerConsumption);
  
  //Temperature/Humidity chart
  var chartBuilder = chartsSheet.newChart();
  var chart_TempHumidity = chartBuilder
  .setPosition(90,1,0,0)
  .setChartType(Charts.ChartType.LINE)
  .setNumHeaders(1)
  .addRange(logSheet.getRange(1,logs.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("DHT1_Temp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("DHT1_Humidity").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Res1_WaterTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Res1_AirTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Res1_Humidity").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .setOption('title', 'Temperature/Humidity')
  .setOption('width', 800)
  .setOption('height', 600)
  .setOption('vAxes', {0: {viewWindow: {min: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()-1, 
                                        max: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()+1
                                       }}})
  .setOption('hAxis', {slantedText: true , 'slantedTextAngle':30})
  .build();  
  chartsSheet.insertChart(chart_TempHumidity);
  
  
  //Generate Hempy Bucket 1 chart
  var chartBuilder = chartsSheet.newChart();
  var chart_HempyBucket1 = chartBuilder
  .setPosition(30,1,0,0)
  .setChartType(Charts.ChartType.LINE)
  .setNumHeaders(1)
  .addRange(logSheet.getRange(1,logs.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Hemp1_WeightB1").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Hemp1_StartB1").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Hemp1_StopB1").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .setOption('title', 'Hempy Bucket 1')
  .setOption('width', 800)
  .setOption('height', 600)
  .setOption('vAxes', {0: {title: 'kg', 
                           viewWindow: {min: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StartB1").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()-1, 
                                        max: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StopB1").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()+1
                                       }}})
  .setOption('hAxis', {slantedText: true , 'slantedTextAngle':30})
  .build();  
  chartsSheet.insertChart(chart_HempyBucket1);
  
  //Generate Hempy Bucket 2 chart
  var chartBuilder = chartsSheet.newChart();
  var chart_HempyBucket2 = chartBuilder
  .setPosition(60,1,0,0)
  .setChartType(Charts.ChartType.LINE)
  .setNumHeaders(1)
  .addRange(logSheet.getRange(1,logs.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Hemp1_WeightB2").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .setOption('title', 'Hempy Bucket 2')
  .setOption('width', 800)
  .setOption('height', 600)
  .setOption('vAxes', {0: {title: 'kg', 
                           viewWindow: {min: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()-1, 
                                        max: logSheet.getRange(logSheet.getLastRow(),logs.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(),1,1).getDisplayValue()+1
                                       }}})
  .setOption('hAxis', {slantedText: true , 'slantedTextAngle':30})
  .build();  
  chartsSheet.insertChart(chart_HempyBucket2);
  
  //Generate Fan speed chart
  var chartBuilder = chartsSheet.newChart();
  var chart_FanSpeed = chartBuilder
  .setPosition(120,1,0,0)
  .setChartType(Charts.ChartType.AREA)
  .setNumHeaders(1)
  .addRange(logSheet.getRange(1,logs.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("IFan_Speed").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .addRange(logSheet.getRange(1,logs.createTextFinder("EFan_Speed").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
  .setOption('title', 'Fan speed')
  .setOption('width', 800)
  .setOption('height', 600)
  .setOption('vAxes', {0: {viewWindow: {min: 0, max: 2}, gridlines: {count: 3}}}) 
  .setOption('hAxis', {slantedText: true , 'slantedTextAngle':30})
  .build();  
  chartsSheet.insertChart(chart_FanSpeed);
  
}


/*
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
.addRange(logSheet.getRange(1,headers.createTextFinder("Light1_Brightness").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.build();
chartsSheet.updateChart(powerUsageChart_updated);

var phAndLightStatusChart = chartsSheet.getCharts()[3];
var phAndLightStatusChart_updated = phAndLightStatusChart
.modify()
.clearRanges()
.addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("PH").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("Light1_Status").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("WaterLevel").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.build();
chartsSheet.updateChart(phAndLightStatusChart_updated);

var pressureAndWaterLevelChart = chartsSheet.getCharts()[4];
var pressureAndWaterLevelChart_updated = pressureAndWaterLevelChart
.modify()
.clearRanges()
.addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("Pressure").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("AeroSprayPressure").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
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
*/
//}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the Alerts sheets, adds newly discovered keys 

function UpdateAlerts(Log){
  var nextRow = settingsSheet.getLastRow() + 1; //Get next row after the last row
  
  //Add all other Header + Data pairs from the received Log JSON
  for each (var Component in Object.keys(Log)){ 
    var Report = JSON.parse(JSON.stringify(Log[Component]));
    for each (var ReportEntry in Object.keys(Report)){  
      var match = settings.createTextFinder(Component + '_' + ReportEntry).matchEntireCell(true).findNext(); //lookup the alert limit row for each Log key
      if(match == null){ //If settings row does not exists
        settingsSheet.getRange(nextRow, 1).setValue(Component + '_' + ReportEntry);   //Insert key in first Key column  
        
        //Adding enable/disable alert checkbox
        settingsSheet.getRange(nextRow, 2).insertCheckboxes();
        
        //Adding YES/NO options for Triggered column
        var YesNo = SpreadsheetApp.newDataValidation().requireValueInList(['YES', 'NO']).build();
        settingsSheet.getRange(nextRow, 5).setDataValidation(YesNo); 
        settingsSheet.getRange(nextRow, 5).setHorizontalAlignment("right");
        settingsSheet.getRange(nextRow, 5).setValue('NO');  //By default do not enable the new alert
        
        //Adding Type column
        var DataType = SpreadsheetApp.newDataValidation().requireValueInList(['Date','Distance','Current','EnabledDisabled','Energy','FanSpeed','Minute','Number','OnOff','Percentage','Power','PumpState','Pressure','Second','Temperature','Text','Time','YesNo','Voltage','Weight']).build();
        settingsSheet.getRange(nextRow, 6).setDataValidation(DataType); //Add YES/NO options for Triggered column
        settingsSheet.getRange(nextRow, 6).setHorizontalAlignment("right");
        settingsSheet.getRange(nextRow, 6).setValue('Number');  //By default do not enable the new alert        
        
        nextRow++;
      }      
    }
  }
  settingsSheet.autoResizeColumns(1, settingsSheet.getLastColumn()); //resize columns to fit the data 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Triggers/Resolves email alerts based on the Alerts sheet settings

function CheckAlerts(Log) 
{
  
  var statusReport = statusSheet.getRange(2, 1, statusSheet.getLastRow(), 2);  //getRange(row, column, numRows, numColumns)  
  var alerts = []; //For storing alert keys
  var alertMessages = [];  //For storing alert messages
  var recoveredMessages = [];  //For storing recovery messages  
  var sendEmailAlert = false;  //Initially assume everything is OK
  var alertLimits = settingsSheet.getRange(3, 1, settingsSheet.getLastRow(), 1);  //getRange(row, column, numRows, numColumns), gets the area where alert limits are stored
  
  for each (var Component in Object.keys(Log)){     
    var Report = JSON.parse(JSON.stringify(Log[Component]));
    for each (var ReportEntry in Object.keys(Report))
    {   
      var match = alertLimits.createTextFinder(Component + '_' + ReportEntry).matchEntireCell(true).findNext(); //lookup the alert limit row for each Log key
      if(match != null)
      { //If found in Settings
        if(settingsSheet.getRange(match.getRow(),2).isChecked() == true)
        {        
          var minLimit = settingsSheet.getRange(match.getRow(),3).getDisplayValue(); //get the 2nd column value from the matched row: Minimum limit
          var maxLimit = settingsSheet.getRange(match.getRow(),4).getDisplayValue(); //get the 3rd column value from the matched row: Maximum limit
          
          if((minLimit != "" && maxLimit != "" && (Log[Component][ReportEntry] < minLimit || maxLimit < Log[Component][ReportEntry]))  || (minLimit == "" && maxLimit != "" && maxLimit < Log[Component][ReportEntry]) || (maxLimit == "" && minLimit != ""&&  Log[Component][ReportEntry] < minLimit))
          { //if reading was out of limits: activate alert
            alerts.push(Component + '_' + ReportEntry);
            
            if(settingsSheet.getRange(match.getRow(),5).getValue() != 'YES')
            { //if alert is new
              alertMessages.push("<strong>" +"[NEW] " + "</strong><font color='red'>" + Component + '_' + ReportEntry + "</font> out of limits: <strong>" + Log[Component][ReportEntry] + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save new alerts for the email
              sendEmailAlert = true; //Send notification that a parameter is out of range 
            }
            else 
            {
              alertMessages.push("<font color='red'>" + Component + '_' + ReportEntry + "</font> out of limits: <strong>" + Log[Component][ReportEntry] + "</strong> [" + minLimit + " / " + maxLimit + "]");    //save active alerts for the email       
            }
            
            //Mark alert active
            settingsSheet.getRange(match.getRow(),5).setValue('YES');  //Triggered column       
            statusMatch = statusReport.createTextFinder(Component + '_' + ReportEntry).matchEntireCell(true).findNext()
            if(statusMatch != null) statusSheet.getRange(statusMatch.getRow(),statusMatch.getColumn() + 1).setFontColor('red');
          }
          else 
          {    
            if(settingsSheet.getRange(match.getRow(),5).getValue() != 'NO')
            { //if alert was active before
              recoveredMessages.push("<font color='green'>" + Component + '_' + ReportEntry + "</font> recovered: <strong>" + Log[Component][ReportEntry] + "</strong> [" + settingsSheet.getRange(match.getRow(),3).getDisplayValue() + " / " + settingsSheet.getRange(match.getRow(),4).getDisplayValue() + "]"); // //save recovered alerts for the email
              sendEmailAlert = true; //Send notification that a parameter recovered
            }
            
            //Mark alert inactive
            settingsSheet.getRange(match.getRow(),5).setValue('NO'); //Triggered column                  
            statusMatch = statusReport.createTextFinder(Component + '_' + ReportEntry).matchEntireCell(true).findNext()
            if(statusMatch != null) statusSheet.getRange(statusMatch.getRow(),statusMatch.getColumn() + 1).setFontColor('black');
          }
        }  
      }
    }
  }
  
  if(sendEmailAlert || GetSettingsValue("Send an email") == "When a report is received")  //When there was a new event
  {
    var emailTemplate = HtmlService.createTemplateFromFile('AlertEmailTemplate');  //Prepare the AlertEmailTemplate.html as a template
    emailTemplate.Alerts = alerts;  //Fill Alert messages into the template
    emailTemplate.AlertMessages = alertMessages;  //Fill Alert messages into the template
    emailTemplate.RecoveredMessages = recoveredMessages; //Fill Recovered messages into the template
    emailTemplate.Data = statusReport.getDisplayValues();  
    sendEmail(emailTemplate);
  }
}

function sendEmail(emailTemplate)
{    
  if(GetSettingsValue("Send an email") != "Disabled") 
  {  
    var emailMessage = emailTemplate.evaluate().getContent();
    
    //This section converts the charts to images and attaches them to the email message
    var emailImages={};
    
    if(GetSettingsValue("Attach charts") == "Yes")
    {
      var charts = chartsSheet.getCharts();
      var chartBlobs=new Array(charts.length);
      
      for(var i =0; i< charts.length; i++){
        var builder = charts[i].modify();
        builder.setOption('chartArea',{width:'90%',height:'75%',bottom:'17%',left:'8%'});  //{left:10,left:10,width:'90%',height:'75%'}
        builder.setOption('width',1024);
        builder.setOption('height',768);
        builder.setOption('maxLines',3);
        builder.setOption('legend',{position: 'top', textStyle: {fontSize: 18}});
        builder.setOption('titleTextStyle',{ fontSize: 22, bold: true });   // { color: <string>, fontName: <string>, fontSize: <number>, bold: <boolean>, italic: <boolean> }
        
        var newchart = builder.build();
        chartBlobs[i]= newchart.getAs('image/png');
        emailMessage= emailMessage + "<img style='width:100%' src='cid:chart"+i+"'>";
        emailImages["chart"+i]= chartBlobs[i];
      }
    }
    
    //This section sends out the email message      
    MailApp.sendEmail({
      to: GetSettingsValue("Email address"),
      subject: "GrowBox report",
      htmlBody: emailMessage,
      inlineImages:emailImages
    });     
  }
  
}
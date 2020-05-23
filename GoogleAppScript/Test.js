/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Trigger a fake JSON report for testing

function RunFakeReport(){
  SpreadsheetApp.getActive().getRangeByName("LastReportTime").setValue(Utilities.formatDate(new Date(), GetSettingsValue("Time zone"), GetSettingsValue("Date format")));
  SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Processing...");
  console.log("Loading fake report data:"); 
  var FakeBoxData = SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue();
  console.log(FakeBoxData); 
  //Browser.msgBox(FakeReportData);
  
  console.log("Parsing fake BoxData:");     
  try{
    var FakeBoxDataJSON = JSON.parse(FakeBoxData);
    console.log(FakeBoxDataJSON);
  }
  catch(e){
    console.log("Error parsing BoxData to JSON. Error: " + e);
    SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Error parsing BoxData to JSON. Error: " + e);
  } 
  ProcessBoxData(FakeBoxDataJSON); 
}

function Test_MainModule_ProcessBoxData(){  
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  ProcessBoxData(FakeJSONData);			 
}

function Test_MainModule_CheckAlerts(){
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  CheckAlerts(FakeJSONData.Log);			 
}

function Test_MainModule_SaveToLog(){
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  SaveToLog(FakeJSONData.Log);			 
}

function Test_UpdateColumns(){
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  UpdateColumns(FakeJSONData.Log);			 
}

function Test_MainModule_UpdateStatus(){
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  UpdateStatus(FakeJSONData.Log);			 
}

function Test_MainModule_UpdateColumns(){
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  UpdateColumns(FakeJSONData.Log);			 
}

function Test_LightSensor1_ProcessBoxData(){
  ProcessBoxData(FakeJSONData_LightCalibration);			 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stuff under development


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
//Updates the Settings sheets, adds newly discovered keys 
/*
function UpdateColumns(Settings){
Logger.log(Settings);  //Log the received data on the console output  
var settings = settingsSheet.getRange(3,9,settingsSheet.getLastRow(),1);
var nextRow = settingsSheet.getLastRow() + 1; //Get next row after the last row

//Add all other Header + Data pairs from the received Log JSON
for (var key in Object.keys(Settings)){
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


/**
 * Write to multiple, disjoint data ranges.
 * @param {string} spreadsheetId The spreadsheet ID to write to.
 */



function writeToMultipleRanges(spreadsheetId) {
  // Specify some values to write to the sheet.
  var columnAValues = [
    ['Item', 'Wheel', 'Door', 'Engine']
  ];
  var rowValues = [
    ['Cost', 'Stocked', 'Ship Date'],
    ['$20.50', '4', '3/1/2016']
  ];

  var request = {
    'valueInputOption': 'USER_ENTERED',
    'data': [
      {
        'range': 'Sheet1!A1:A4',
        'majorDimension': 'COLUMNS',
        'values': columnAValues
      },
      {
        'range': 'Sheet1!B1:D2',
        'majorDimension': 'ROWS',
        'values': rowValues
      }
    ]
  };

  var response = Sheets.Spreadsheets.Values.batchUpdate(request, spreadsheetId);
  Logger.log(response);
}


function scrollToBottom(){
 var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
 var range = sheet.getRange(sheet.getLastRow(),1);
 sheet.setActiveRange(range);
}

//function onOpen(){  //When spreadsheet is opened go to Log sheet's latest entry
// var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Log");
// var range = sheet.getRange(sheet.getLastRow(),1);
// sheet.setActiveRange(range);
//}
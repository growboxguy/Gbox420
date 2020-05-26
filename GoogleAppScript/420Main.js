/*
GrowBoxGuy - http://sites.google.com/site/growboxguy/
JSON data receiver and Google Sheets logging

-Works with the new App Scripts V8 only
-Enable Google Apps Script API from: https://script.google.com/home/usersettings
-DEVs ONLY:
Connect Google App Scripts to Visual Studio Code: https://yagisanatode.com/2019/04/01/working-with-google-apps-script-in-visual-studio-code-using-clasp/
You will also need the latest version of NPM,https://phoenixnap.com/kb/update-node-js-version
*/

var SupportedDataTypes = ['Date', 'Distance', 'Current', 'EnabledDisabled', 'Energy', 'FanSpeed', 'Minute', 'Number', 'OnOff', 'Percentage', 'Power', 'PumpState', 'Pressure', 'Second', 'Temperature', 'Text', 'Time', 'YesNo', 'Voltage', 'Weight'];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Test functions
function Test_ProcessBoxData() {
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  ProcessBoxData(FakeJSONData);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This is where data arrives for the Arduino for processing
function doPost(receivedData) {
  LogToConsole(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", true,0);  
  LogToConsole("Data received from Arduino, Raw:", false, 0);
  LogToConsole(receivedData, true, 1);
  SpreadsheetApp.getActive().getRangeByName("LastReportTime").setValue(Utilities.formatDate(new Date(), GetSettingsValue("Time zone"), GetSettingsValue("Date format"))); //Log when the JSON was received
  SpreadsheetApp.getActive().getRangeByName("LastReportRaw").setValue(receivedData); //Log the received data
  SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Processing...");
  try {
    if (receivedData.parameter.BoxData != null) //Only Start processing if the report contains a BoxData parameter
    {
      LogToConsole("Parsing BoxData: ", false, 1);
      try {
        var BoxDataJSON = JSON.parse(receivedData.parameter.BoxData);
        SpreadsheetApp.getActive().getRangeByName("LastReportJSON").setValue(JSON.stringify(BoxDataJSON)); //Log the received data
        LogToConsole(JSON.stringify(BoxDataJSON), true, 0);
        ProcessBoxData(BoxDataJSON);
      }
      catch (e) {
        LogToConsole("Error: " + e, true, 0);
        SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Error parsing BoxData to JSON");
      }
    }
  }
  catch (e) {
    LogToConsole("Received parameters does not contain a BoxData object. Error: " + e, true, 1);
    SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("BoxData is empty. Input should be in the form of: {parameter={BoxData=JSON_OBJECT} where JSON_OBJECT is a valid JSON. Error: " + e); //the received parameter is not in {parameter={BoxData={"
  }
  LogToConsole(">>>>>>>End of script>>>>>>>", true, 0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This is where the extracted JSON report is processed
function ProcessBoxData(JSONBoxData) {
  LogToConsole("Processing BoxDataJSON: ", false, 1);
  try {
    if (JSONBoxData.Log != null) {
      LogToConsole(JSON.stringify(JSONBoxData.Log), true, 0);  //Print the JSON on the Stackdriver logging (View / Stackdriver logging)
      SaveToLog(JSONBoxData.Log); //Save the log to the Logs sheet 
      UpdateColumns(JSONBoxData.Log); //Add missing columns to the Settings sheet 
      UpdateStatus(JSONBoxData.Log); //Add the latest status to the Status page     
      CheckAlerts(JSONBoxData.Log); //Checks for alerts and send an email alert
      UpdateCharts();  //Updates the chart datasources in case the columns were reordered
    }
    else {
      LogToConsole("Received BoxData does not contain a Log section. Skipping log processing.", true, 1);
    }
    // if(JSONBoxData.Settings != null) {
    //   UpdateColumns(JSONBoxData.Settings);
    // }
    // else{
    //    console.log("Received BoxData does not contain a Settings section. Skipping settings processing."
    // }
    if (JSONBoxData.LightSensor1 != null) {
      UpdateCalibrationValues("LightSensor1", JSONBoxData.LightSensor1.Readings);
    }
    SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("BoxData processed successfully");
  }
  catch (e) {
    LogToConsole("Error: " + e, true, 1);
    SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Error processing BoxDataJSON. Error:" + e);
  }
}

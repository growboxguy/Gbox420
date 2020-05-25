/*
GrowBoxGuy - http://sites.google.com/site/growboxguy/
JSON data receiver and Google Sheets logging

-Works with the new App Scripts V8 only
-Enable Google Apps Script API from: https://script.google.com/home/usersettings  
-Recommended to connect Google App Scripts to Visual Studio Code:
  https://yagisanatode.com/2019/04/01/working-with-google-apps-script-in-visual-studio-code-using-clasp/
  You will also need the latest version of NPM,https://phoenixnap.com/kb/update-node-js-version
*/

var Debug = true;  //Activates extra Stackdriver logs


//Column numbers - Only change these after rearranging columns
var status_RawLog_Column = 5;
var settings_AlertEnabled_Column = 2;
var settings_Triggered_Column = 5;
var settings_DataType_Column = 6;



var SupportedDataTypes = ['Date','Distance','Current','EnabledDisabled','Energy','FanSpeed','Minute','Number','OnOff','Percentage','Power','PumpState','Pressure','Second','Temperature','Text','Time','YesNo','Voltage','Weight'];



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Test functions
function Test_MainModule_ProcessBoxData(){  
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  ProcessBoxData(FakeJSONData);			 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This is where data arrives for the Arduino for processing
function doPost(receivedData) {  
  LogToConsole("Data received from Arduino, Raw:",false,0);
  LogToConsole(receivedData,true,1);  
  SpreadsheetApp.getActive().getRangeByName("LastReportTime").setValue(Utilities.formatDate(new Date(), GetSettingsValue("Time zone"), GetSettingsValue("Date format")));  //Log when the JSON was received
  SpreadsheetApp.getActive().getRangeByName("LastReportRaw").setValue(receivedData); //Log the received data
  SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Processing...");
  
  LogToConsole("Checking received data...",true,1);

  try{ 
    if(receivedData.parameter.BoxData != null) //Only Start processing if the report contains a BoxData parameter
    {  
      LogToConsole("Parsing BoxData:",false,2);     
      try{
        var BoxDataJSON = JSON.parse(receivedData.parameter.BoxData);
        SpreadsheetApp.getActive().getRangeByName("LastReportJSON").setValue(JSON.stringify(BoxDataJSON)); //Log the received data
        LogToConsole(BoxDataJSON,true,1);
        ProcessBoxData(BoxDataJSON);        
      }
      catch(e){
        LogToConsole("Error: " + e,true,1);
        SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Error parsing BoxData to JSON");
      }
    }       
  }  
  catch(e){
    LogToConsole("Received parameters does not contain a BoxData object. Error: " + e,true,1);
    SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("BoxData is empty. Input should be in the form of: {parameter={BoxData=JSON_OBJECT} where JSON_OBJECT is a valid JSON. Error: " + e);  //the received parameter is not in {parameter={BoxData={"
    }
  LogToConsole("End of script",true,0)
}   


function ProcessBoxData(JSONBoxData)
{
  LogToConsole("Processing BoxDataJSON:",false,2);   
  try{
    if(JSONBoxData.Log != null)
    {
      LogToConsole(JSON.stringify(JSONBoxData.Log), true, 1);
      SaveToLog(JSONBoxData.Log);  //Save the log to the Logs sheet on the spreadsheet specified by spreadSheetID 
      UpdateColumns(JSONBoxData.Log); //Add missing columns to the Settings sheet 
      UpdateStatus(JSONBoxData.Log); //Add formatted status of each component to the Status page     
      CheckAlerts(JSONBoxData.Log);  //Checks for alerts and send an email alert
    //  UpdateCharts();  //Updates the chart datasources in case the columns were reordered
    }
    else{
      LogToConsole("Received BoxData does not contain a Log section. Skipping log processing.",true,1);
    }
    
    // if(JSONBoxData.Settings != null) {
    //   UpdateColumns(JSONBoxData.Settings);
    // }
    // else{
    //    console.log("Received BoxData does not contain a Settings section. Skipping settings processing."
    // }
    
    if(JSONBoxData.LightSensor1 != null){
      UpdateCalibrationValues("LightSensor1",JSONBoxData.LightSensor1.Readings);
    }
    
    SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("BoxData processed successfully");
  }
  catch(e){
    LogToConsole("Error: " + e, true, 1);
    SpreadsheetApp.getActive().getRangeByName("ImportResult").setValue("Error processing BoxDataJSON. Error:" + e);
  }
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relay an email based on a HTML email template - Used to send out Alerts

function sendEmail(emailTemplate)
{    
  if(GetSettingsValue("Send an email") != "Disabled") 
  {  
    var emailMessage = emailTemplate.evaluate().getContent();
    //Browser.msgBox(emailMessage)
    
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
    
    //Send out the email message      
    MailApp.sendEmail({
      to: GetSettingsValue("Email address"),
      subject: "GrowBox report",
      htmlBody: emailMessage,
      inlineImages:emailImages
    });     
  }  
}
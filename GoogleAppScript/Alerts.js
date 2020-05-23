
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Triggers/Resolves email alerts based on the Alerts sheet settings

function CheckAlerts(Log) 
{  
  var alerts = []; //For storing alert keys
  var alertMessages = [];  //For storing alert messages
  var recoveredMessages = [];  //For storing recovery messages  
  var sendEmailAlert = false;  //Initially assume everything is OK
  
  for (var Component in Object.keys(Log)){     
    var Report = JSON.parse(JSON.stringify(Log[Component]));
    for (var ReportEntry in Object.keys(Report))
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
            statusMatch = statusHeader.createTextFinder(Component + '_' + ReportEntry).matchEntireCell(true).findNext()
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Triggers/Resolves email alerts based on the Alerts sheet settings

function CheckAlerts(Log) 
{  
  var alerts = []; //For storing alert keys
  var alertMessages = [];  //For storing alert messages
  var recoveredMessages = [];  //For storing recovery messages  
  var sendEmailAlert = false;  //Initially assume everything is OK
  
  var columns = GetNamedRange("Columns");

  var Components = Object.getOwnPropertyNames(Log);  
  for (var i = 0; i < Components.length; i++) {
    var Properties = Object.getOwnPropertyNames(Log[Components[i]]);    
    for (var j = 0; j < Properties.length; j++) {
      var key = Components[i] + '_' + Properties[j];
      var value = Log[Components[i]][Properties[j]];

      var match = columns.filter(function(row){
        return row[0] == key;
      });
      LogToConsole(key + " alert lookup: " + match,true,3);

     if(match != null && match.length >0)
      { //If match found in Settings
        if(match[0][1])  //2nd column: Alert Enabled column: True or False
        {        
          var minLimit = match[0][2]; //3rd column: Minimum limit
          var maxLimit = match[0][3]; //4th column: Maximum limit
          LogToConsole(key + ": Alert active: ["+minLimit + "/"+maxLimit+"]", true, 3); 
          
          if((minLimit != "" && maxLimit != "" && (value < minLimit || maxLimit < value))  || (minLimit == "" && maxLimit != "" && maxLimit < value) || (maxLimit == "" && minLimit != ""&&  value < minLimit))
          { //if reading was out of limits: activate alert
            alerts.push(key);
            
            if(match[0][4] != 'YES') //Triggered column
            { //if alert is new
              alertMessages.push("<strong>" +"[NEW] " + "</strong><font color='red'>" + key + "</font> out of limits: <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save new alerts for the email
              sendEmailAlert = true; //Send notification that a parameter is out of range 
              SpreadsheetApp.getActive().getRangeByName("Columns").createTextFinder("key").matchEntireCell(true).findNext().getRow(),logSheet.getLastRow(),1))
            }
            else 
            {
              alertMessages.push("<font color='red'>" + key + "</font> out of limits: <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]");    //save active alerts for the email       
            }
            
            //Mark alert active
            settingsSheet.getRange(match.getRow(),5).setValue('YES');  //Triggered column       
            statusMatch = statusHeader.createTextFinder(key).matchEntireCell(true).findNext()
            if(statusMatch != null) statusSheet.getRange(statusMatch.getRow(),statusMatch.getColumn() + 1).setFontColor('red');
          }
          else 
          {    
            if(settingsSheet.getRange(match.getRow(),5).getValue() != 'NO')
            { //if alert was active before
              recoveredMessages.push("<font color='green'>" + key + "</font> recovered: <strong>" + value + "</strong> [" + settingsSheet.getRange(match.getRow(),3).getDisplayValue() + " / " + settingsSheet.getRange(match.getRow(),4).getDisplayValue() + "]"); // //save recovered alerts for the email
              sendEmailAlert = true; //Send notification that a parameter recovered
            }
            
            //Mark alert inactive
            settingsSheet.getRange(match.getRow(),5).setValue('NO'); //Triggered column                  
            statusMatch = statusReport.createTextFinder(key).matchEntireCell(true).findNext()
            if(statusMatch != null) statusSheet.getRange(statusMatch.getRow(),statusMatch.getColumn() + 1).setFontColor('black');
          }
        } 
        else {
          LogToConsole(key + ": Alert not active", true, 3); 
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
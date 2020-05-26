/*
 Check if the received values are between the Minimum and Maximum limit based on the Settings \ Columns section
 Sends an email alert if a value gets out of range or recoveres
*/

function Test_CheckAlerts() {
  FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
  CheckAlerts(FakeJSONData.Log);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Triggers/Resolves email alerts based on the Alerts sheet settings
function CheckAlerts(Log) {
  LogToConsole("Processing alerts...",true,0);
  var alerts = []; //For storing alert keys
  var alertMessages = []; //For storing alert messages
  var recoveredMessages = []; //For storing recovery messages  
  var sendEmailAlert = false; //Initially assume everything is OK
  
  var columns = GetNamedRangeValues("Columns",true);
  var Components = Object.getOwnPropertyNames(Log);
  for (var i = 0; i < Components.length; i++) {
    var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
    for (var j = 0; j < Properties.length; j++) {
      var key = Components[i] + '_' + Properties[j];
      var value = Log[Components[i]][Properties[j]];
      var match = columns.filter(function (row) {
        return row[0] == key;
      });      
     
      if (match != null && match.length > 0) { //If match found in Settings
        var originalRow = match.map(row => columns.indexOf(row));     //Points to the original row number in the Columns range    
        if (match[0][1]) //2nd column: Alert Enabled column: True or False
        {       
          var minLimit = match[0][2]; //3rd column: Minimum limit
          var maxLimit = match[0][3]; //4th column: Maximum limit
          var test = Number("Test");
          LogToConsole(key + " : " + value + ", Alert active: [" + minLimit + "/" + maxLimit + "]", false, 1);
                   
          if ((minLimit != "" && maxLimit != "" && (value < minLimit || maxLimit < value)) || (minLimit == "" && maxLimit != "" && maxLimit < value) || (maxLimit == "" && minLimit != "" && value < minLimit)) { //if reading was out of limits: activate alert
            alerts.push(key);
            LogToConsole(" -> out of range", true, 0);
            if (match[0][4] != 'YES') //Triggered column
            { //if alert is new
              alertMessages.push("<strong>" + "[NEW] " + "</strong><font color='red'>" + key + "</font> out of limits: <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save new alerts for the email
              sendEmailAlert = true; //Send notification that a new value is out of range 
              columns[originalRow][4] = 'YES'; //Flag the alert active
            }
            else {
              alertMessages.push("<font color='red'>" + key + "</font> out of limits: <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save active alerts for the email       
            }
            //Mark alert active
            //settingsSheet.getRange(match.getRow(), 5).setValue('YES'); //Triggered column       
            //statusMatch = statusHeader.createTextFinder(key).matchEntireCell(true).findNext();
            //if (statusMatch != null)
            //    statusSheet.getRange(statusMatch.getRow(), statusMatch.getColumn() + 1).setFontColor('red');
          }
          else {
            LogToConsole(" -> OK", true, 0);
            if (match[0][4] != 'NO') { //if alert was active before
              recoveredMessages.push("<font color='green'>" + key + "</font> recovered: <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); // //save recovered alerts for the email
              sendEmailAlert = true; //Send notification that a parameter recovered
              columns[originalRow][4] = 'NO'; //Flag the alert active
            }
            //Mark alert inactive
            //settingsSheet.getRange(match.getRow(), 5).setValue('NO'); //Triggered column                  
            //statusMatch = statusReport.createTextFinder(key).matchEntireCell(true).findNext();
            // if (statusMatch != null)
            //    statusSheet.getRange(statusMatch.getRow(), statusMatch.getColumn() + 1).setFontColor('black');
          }
        }
        else {
         if(Debug) LogToConsole(key + ": Alert not active", true, 3);
        }
      }
    }
  }
  SaveNamedRange("Columns",columns); //Write back the alert status changes to Sheets
  
  if (sendEmailAlert || GetSettingsValue("Send an email") == "When a report is received") //When there was a new event
  {
    LogToConsole("Sending email alert", true, 1);
    var emailTemplate = HtmlService.createTemplateFromFile('AlertTemplate'); //Prepare the AlertEmailTemplate.html as a template
    emailTemplate.Alerts = alerts; //Fill Alert messages into the template
    emailTemplate.AlertMessages = alertMessages; //Fill Alert messages into the template
    emailTemplate.RecoveredMessages = recoveredMessages; //Fill Recovered messages into the template
    emailTemplate.Data = GetNamedRangeValues("Status");
    sendEmail(emailTemplate);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relay an email based on a HTML email template - Used to send out Alerts
function sendEmail(emailTemplate) {
    if (GetSettingsValue("Send an email") != "Disabled") {
        var emailMessage = emailTemplate.evaluate().getContent();
        //Browser.msgBox(emailMessage)
        //This section converts the charts to images and attaches them to the email message
        var emailImages = {};
        if (GetSettingsValue("Attach charts") == "Yes") {
            var charts = SpreadsheetApp.getActive().getSheetByName("Charts").getCharts();
            var chartBlobs = new Array(charts.length);
            for (var i = 0; i < charts.length; i++) {
                var builder = charts[i].modify();
                builder.setOption('chartArea', { width: '90%', height: '75%', bottom: '17%', left: '8%' }); //{left:10,left:10,width:'90%',height:'75%'}
                builder.setOption('width', 1024);
                builder.setOption('height', 768);
                builder.setOption('maxLines', 3);
                builder.setOption('legend', { position: 'top', textStyle: { fontSize: 18 } });
                builder.setOption('titleTextStyle', { fontSize: 22, bold: true }); // { color: <string>, fontName: <string>, fontSize: <number>, bold: <boolean>, italic: <boolean> }        
                var newchart = builder.build();
                chartBlobs[i] = newchart.getAs('image/png');
                emailMessage = emailMessage + "<img style='width:100%' src='cid:chart" + i + "'>";
                emailImages["chart" + i] = chartBlobs[i];
            }
        }
        //Send out the email message      
        MailApp.sendEmail({
            to: GetSettingsValue("Email address"),
            subject: "GrowBox report",
            htmlBody: emailMessage,
            inlineImages: emailImages
        });
    }
}
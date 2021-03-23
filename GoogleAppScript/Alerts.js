/*
 Check if the received values are between the Minimum and Maximum limit based on the Settings \ Columns section
 Sends an email alert if a value gets out of range or recoveres
*/

function Test_CheckAlerts() {
  CheckAlerts(getTestJSONData().Log);
}


//Triggers/Resolves email alerts based on the Alerts sheet settings
function CheckAlerts(Log) {
  LogToConsole("Processing alerts...", true, 0);
  var alerts = []; //For storing alert keys
  var alertMessages = []; //For storing alert messages
  var recoveredMessages = []; //For storing recovery messages  
  var sendEmailAlert = false; //Initially assume everything is OK

  var columns = GetNamedRangeValues("Columns");
  var Components = Object.getOwnPropertyNames(Log);
  for (var i = 0; i < Components.length; i++) {
    var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
    for (var j = 0; j < Properties.length; j++) {
      var key = Components[i] + '_' + Properties[j];
      var value = Log[Components[i]][Properties[j]];
      var match = columns.filter(function (row) {
        return row[columns_keyColumn] == key;
      });

      if (match != null && match.length > 0) { //If match found in Settings
        var originalRow = match.map(row => columns.indexOf(row));     //Points to the original row number in the Columns range    
        if (match[0][columns_alertEnabledColumn]) {
          var minLimit = match[0][columns_alertMinColumn];
          var maxLimit = match[0][columns_alertMaxColumn];
          LogToConsole(GetFriendlyColumnName(key,false) + " : " + GetFriendlyValue(key, value) + " [" + key + " : " + value + "], Alert limits: [" + minLimit + "/" + maxLimit + "]", false, 1);

          if ((minLimit != "" && maxLimit != "" && (value < minLimit || maxLimit < value)) || (minLimit == "" && maxLimit != "" && maxLimit < value) || (maxLimit == "" && minLimit != "" && value < minLimit)) { //if reading was out of limits: activate alert
            alerts.push(key);
            LogToConsole(" -> Out of range", true, 0);
            if (match[0][columns_triggeredColumn] != 'YES') { //if alert is new
              alertMessages.push("<strong>" + "NEW " + "<font color='red'>" + GetFriendlyColumnName(key,false) + " : " + GetFriendlyValue(key, value) + "</font></strong> - " + key + " : <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save new alerts for the email
              sendEmailAlert = true; //Send notification that a new value is out of range 
              columns[originalRow][columns_triggeredColumn] = 'YES'; //Flag the alert active
            }
            else {
              alertMessages.push("<strong><font color='red'>" + GetFriendlyColumnName(key,false) + " : " + GetFriendlyValue(key, value) + "</font></strong> - " + key + " : <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save active alerts for the email       
            }
            //Mark alert active on Status page
            //settingsSheet.getRange(match.getRow(), 5).setValue('YES'); //Triggered column       
            //statusMatch = statusHeader.createTextFinder(key).matchEntireCell(true).findNext();
            //if (statusMatch != null)
            //    statusSheet.getRange(statusMatch.getRow(), statusMatch.getColumn() + 1).setFontColor('red');
          }
          else {
            LogToConsole(" -> OK", true, 0);
            if (match[0][columns_triggeredColumn] != 'NO') { //if alert was active before
              recoveredMessages.push("<strong><font color='green'>" + GetFriendlyColumnName(key,false) + " : " + GetFriendlyValue(key, value) + "</font></strong> - " + key + " : <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); // //save recovered alerts for the email
              sendEmailAlert = true; //Send notification that a parameter recovered
              columns[originalRow][columns_triggeredColumn] = 'NO'; //Flag the alert active
            }
            //Mark alert inactive on Status page
            //settingsSheet.getRange(match.getRow(), 5).setValue('NO'); //Triggered column                  
            //statusMatch = statusReport.createTextFinder(key).matchEntireCell(true).findNext();
            // if (statusMatch != null)
            //    statusSheet.getRange(statusMatch.getRow(), statusMatch.getColumn() + 1).setFontColor('black');
          }
        }
        else {
          if (Debug) LogToConsole(GetFriendlyColumnName(key,false) + " : " + GetFriendlyValue(key, value) + " [" + key + " : " + value + "] : Alert not active", true, 3);
        }
      }
    }
  }
  SaveNamedRange("Columns", columns); //Write back the alert status changes to Sheets
  LogToConsole("Total alerts: " + alerts.length + ", Recovered alerts: " + recoveredMessages.length, true, 1);

  var EmailMode = GetSettingsValue("Send an email");
  if (sendEmailAlert || EmailMode == "When a report is received" || (EmailMode == "When an alert is active" && alertMessages.length != 0)) //When there was a new event
  {
    LogToConsole("Generating email template...", true, 1);
    var emailTemplate = HtmlService.createTemplateFromFile('AlertTemplate'); //Prepare the AlertEmailTemplate.html as a template
    emailTemplate.Alerts = alerts; //Fill Alert messages into the template
    emailTemplate.AlertMessages = alertMessages; //Fill Alert messages into the template
    emailTemplate.RecoveredMessages = recoveredMessages; //Fill Recovered messages into the template
    emailTemplate.LatestStatus = convertRange2html(ActiveSpreadsheetApp.getRangeByName("Status").getDataRegion());
    //emailTemplate.LatestStatus = GetNamedRangeValues("Status",true).filter(function (row) {
    //  return row[columns_keyColumn] != null && row[columns_keyColumn] != "";  //Filter out rows with a blank key colum
    //});
    sendEmail(emailTemplate);
  }
  else {
    LogToConsole("Skipped sending an email", true, 1);
  }
}


// Relay an email based on a HTML email template - Used to send out Alerts
function sendEmail(emailTemplate) {
  if (GetSettingsValue("Send an email") != "Disabled") {
    LogToConsole("Sending email...", true, 1);
    var emailMessage = emailTemplate.evaluate().getContent();
    var emailMessage = emailMessage.replace(/(&lt;)/ig, "<");
    var emailMessage = emailMessage.replace(/(&gt;)/ig, ">");
    var emailMessage = emailMessage.replace(/(&#34;)/ig, '"');
    var emailMessage = emailMessage.replace(/(&#43;)/ig, "+");
    var emailMessage = emailMessage.replace(/(&amp;nbsp;)/ig, " ");

    //Browser.msgBox(emailMessage)
    //This section converts the charts to images and attaches them to the email message
    var emailImages = {};
    if (GetSettingsValue("Attach charts") == "Yes") {
      if (Debug) LogToConsole("Attaching charts...", true, 2);
      var charts = chartsSheet.getCharts();
      var chartBlobs = new Array(charts.length);
      for (var i = 0; i < charts.length; i++) {
        var builder = charts[i].modify();
        builder.setOption('chartArea', { width: '90%', height: '75%', left: '8%' }); //{left:10,left:10,width:'90%',height:'75%'}
        builder.setOption('width', 1024);
        builder.setOption('height', 768);
        //builder.setOption('maxLines', 3);
        builder.setOption('legend', { position: 'top', textStyle: { fontSize: 18 } });
        builder.setOption('titleTextStyle', { fontSize: 22, bold: true }); // { color: <string>, fontName: <string>, fontSize: <number>, bold: <boolean>, italic: <boolean> }  
        builder.setOption('hAxis', { viewWindowMode: 'pretty' });
        builder.setOption('vAxis', { viewWindowMode: 'pretty' });
        //builder.setOption('interpolateNulls', true);

        var newchart = builder.build();
        chartBlobs[i] = newchart.getAs('image/png');
        emailMessage = emailMessage + "<img style='width:100%' src='cid:chart" + i + "'>";
        emailImages["chart" + i] = chartBlobs[i];
      }
    }
    //Send out the email message      
    MailApp.sendEmail({
      to: GetSettingsValue("Email address"),
      subject: GetSettingsValue("Email title"),
      htmlBody: emailMessage,
      inlineImages: emailImages
    });
  }
  else {
    LogToConsole("Send an email - Disabled", true, 1);
  }
}

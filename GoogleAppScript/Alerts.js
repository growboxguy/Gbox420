/*
 Check if the received values are between the Minimum and Maximum limit based on the Settings \ Columns section
 Sends an email alert if a value gets out of range or recoveres
*/

function Test_checkAlerts() {
  checkAlerts(getTestJSONData().Log);
}

function Test_sendEmail() {
  LogToConsole("Generating email template...", true, 1);

  var alerts = [];
  var alertMessages = [];
  var recoveredMessages = [];
  var emailTemplate = HtmlService.createTemplateFromFile('AlertTemplate');

  emailTemplate.Alerts = alerts;
  emailTemplate.AlertMessages = alertMessages;
  emailTemplate.RecoveredMessages = recoveredMessages;
  emailTemplate.LatestStatus = convertRange2html(SpreadsheetApp.getActiveSpreadsheet().getRangeByName("Status").getDataRegion());

  sendEmail(emailTemplate);
}


//Triggers/Resolves email alerts based on the Alerts sheet settings
function checkAlerts(Log) {
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
          LogToConsole(GetFriendlyColumnName(key, false) + " : " + GetFriendlyValue(key, value) + " [" + key + " : " + value + "], Alert limits: [" + minLimit + "/" + maxLimit + "]", false, 1);

          if ((minLimit != "" && maxLimit != "" && (value < minLimit || maxLimit < value)) || (minLimit == "" && maxLimit != "" && maxLimit < value) || (maxLimit == "" && minLimit != "" && value < minLimit)) { //if reading was out of limits: activate alert
            alerts.push(key);
            LogToConsole(" -> Out of range", true, 0);
            if (match[0][columns_triggeredColumn] != 'YES') { //if alert is new
              alertMessages.push("<strong>" + "NEW " + "<font color='red'>" + GetFriendlyColumnName(key, false) + " : " + GetFriendlyValue(key, value) + "</font></strong> - " + key + " : <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save new alerts for the email
              sendEmailAlert = true; //Send notification that a new value is out of range 
              columns[originalRow][columns_triggeredColumn] = 'YES'; //Flag the alert active
            }
            else {
              alertMessages.push("<strong><font color='red'>" + GetFriendlyColumnName(key, false) + " : " + GetFriendlyValue(key, value) + "</font></strong> - " + key + " : <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save active alerts for the email       
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
              recoveredMessages.push("<strong><font color='green'>" + GetFriendlyColumnName(key, false) + " : " + GetFriendlyValue(key, value) + "</font></strong> - " + key + " : <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); // //save recovered alerts for the email
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
          //if (Debug) LogToConsole(GetFriendlyColumnName(key,false) + " : " + GetFriendlyValue(key, value) + " [" + key + " : " + value + "] : Alert not active", true, 3);
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
    let emailMessage = emailTemplate.evaluate().getContent();
    emailMessage = emailMessage.replace(/(&lt;)/ig, "<")
      .replace(/(&gt;)/ig, ">")
      .replace(/(&#34;)/ig, '"')
      .replace(/(&#43;)/ig, "+")
      .replace(/(&amp;nbsp;)/ig, " ");

    let emailImages = {};
    if (GetSettingsValue("Attach charts") == "Yes") {
      LogToConsole("Attaching charts...", true, 2);

      const spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
      const chartsSheet = spreadsheet.getSheetByName("Charts");
      const charts = chartsSheet.getCharts();
      Logger.log(`Found ${charts.length} charts.`);

      let slideDeck;
      const slideDeckId = PropertiesService.getScriptProperties().getProperty("slideDeckId");
      if (slideDeckId) {
        slideDeck = SlidesApp.openById(slideDeckId); // Slide deck already exists, so open it
      } else {
        slideDeck = SlidesApp.create("Gbox420_Slides"); // Create new slide deck
        PropertiesService.getScriptProperties().setProperty("slideDeckId", slideDeck.getId()); // Save the slide deck ID
      }

      for (let i = 0; i < charts.length; i++) {
        try {
          Logger.log(`Processing chart ${i}`);

          const slides = slideDeck.getSlides();
          if (i >= slides.length) {
            slideDeck.appendSlide(SlidesApp.PredefinedLayout.BLANK);  // Append a blank slide if not enough slides
            Logger.log(`Added new slide for chart ${i}`);
          }
          slide = slideDeck.getSlides()[i];
          const pageElements = slide.getPageElements();
          for (const element of pageElements) {
            element.remove();
          }
          const chartImage = slide.insertSheetsChartAsImage(charts[i]); // Add chart to slide
          const chartBlob = chartImage.getAs('image/png'); // Convert to image
          emailImages[`chart${i}`] = chartBlob; // Store in emailImages object
          emailMessage = emailMessage + "<img style='width:100%' src='cid:chart" + i + "'>";
        } catch (error) {
          Logger.log(`Error processing chart ${i}: ${error.message}`);
        }
      }
    }

    MailApp.sendEmail({
      to: GetSettingsValue("Email address"),
      subject: GetSettingsValue("Email title"),
      htmlBody: emailMessage,
      inlineImages: emailImages,
    });
  } else {
    LogToConsole("Send an email - Disabled", true, 1);
  }
}
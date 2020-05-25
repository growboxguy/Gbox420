function Test_CheckAlerts() {
    FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
    CheckAlerts(FakeJSONData.Log);
}


//Triggers/Resolves email alerts based on the Alerts sheet settings
function CheckAlerts(Log) {
    var alerts = []; //For storing alert keys
    var alertMessages = []; //For storing alert messages
    var recoveredMessages = []; //For storing recovery messages  
    var sendEmailAlert = false; //Initially assume everything is OK

    var columns = GetNamedRange("Columns");
    var Components = Object.getOwnPropertyNames(Log);
    for (var i = 0; i < Components.length; i++) {
        var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
        for (var j = 0; j < Properties.length; j++) {
            var key = Components[i] + '_' + Properties[j];
            var value = Number(Log[Components[i]][Properties[j]]);
            var match = columns.filter(function (row) {
                return row[0] == key;
            });

            LogToConsole(key + " alert lookup: " + match, true, 3);
            if (match != null && match.length > 0) { //If match found in Settings
                var originalIndex = match.map(row => columns.indexOf(row));
                console.log("Original match index: " + originalIndex);

                if (match[0][1]) //2nd column: Alert Enabled column: True or False
                 {
                    var minLimit = Number(match[0][2]); //3rd column: Minimum limit
                    var maxLimit = Number(match[0][3]); //4th column: Maximum limit
                    LogToConsole(key + " : " + value + ", Alert active: [" + minLimit + "/" + maxLimit + "]", false, 3);
                    if ((minLimit != "" && maxLimit != "" && (value < minLimit || maxLimit < value)) || (minLimit == "" && maxLimit != "" && maxLimit < value) || (maxLimit == "" && minLimit != "" && value < minLimit)) { //if reading was out of limits: activate alert
                        alerts.push(key);
                        LogToConsole(" -> out of range", true, 0);
                        if (match[0][4] != 'YES') //Triggered column
                         { //if alert is new
                            alertMessages.push("<strong>" + "[NEW] " + "</strong><font color='red'>" + key + "</font> out of limits: <strong>" + value + "</strong> [" + minLimit + " / " + maxLimit + "]"); //save new alerts for the email
                            sendEmailAlert = true; //Send notification that a new value is out of range 
                            columns[originalIndex][4] = 'YES'; //Flag the alert active
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
                            columns[originalIndex][4] = 'NO'; //Flag the alert active
                        }
                        //Mark alert inactive
                        //settingsSheet.getRange(match.getRow(), 5).setValue('NO'); //Triggered column                  
                        //statusMatch = statusReport.createTextFinder(key).matchEntireCell(true).findNext();
                       // if (statusMatch != null)
                        //    statusSheet.getRange(statusMatch.getRow(), statusMatch.getColumn() + 1).setFontColor('black');
                    }
                }
                else {
                    LogToConsole(key + ": Alert not active", true, 3);
                }
            }
        }
    }
    SaveNamedRange("Columns",columns); //Write back the alert status changes to Sheets

    if (sendEmailAlert || GetSettingsValue("Send an email") == "When a report is received") //When there was a new event
     {
        var emailTemplate = HtmlService.createTemplateFromFile('EmailTemplate'); //Prepare the AlertEmailTemplate.html as a template
        emailTemplate.Alerts = alerts; //Fill Alert messages into the template
        emailTemplate.AlertMessages = alertMessages; //Fill Alert messages into the template
        emailTemplate.RecoveredMessages = recoveredMessages; //Fill Recovered messages into the template
        emailTemplate.Data = statusReport.getDisplayValues();
        sendEmail(emailTemplate);
    }
}

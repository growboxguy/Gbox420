// Compiled using ts2gas 3.6.2 (TypeScript 3.9.3)
function Test_UpdateStatus() {
    FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
    UpdateStatus(FakeJSONData.Log);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the Status sheet
function UpdateStatus(Log) {
    console.log("Updating Status sheet...");
    var status = GetNamedRange("Status");
    var newStatus = [];
    for (var i = 0; i < status.length; i++) {
        newStatus.push([status[i][0], null]); //pre-fill with null values
    }
    LogToConsole("Blank status: " + newStatus, true, 1);
    //Add all Key + Data pairs to the Status sheet from the received Log JSON  
    var Components = Object.getOwnPropertyNames(Log);
    for (var i = 0; i < Components.length; i++) {
        var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
        for (var j = 0; j < Properties.length; j++) {
            var key = Components[i] + '_' + Properties[j];
            var value = Log[Components[i]][Properties[j]];
            var valueToWriteBack = GetFriendlyValue(key, value);
            var match = -1;
            for (var k = 0; k < newStatus.length; k++) {
                if (newStatus[k][0] == key) {
                    match = k; //Trying to find Status entry with the relayed component name
                    break;
                }
            }
            if (match != -1) {
                try {
                    newStatus[match][1] = valueToWriteBack; //Update matched row's second column
                    if (Debug)
                        LogToConsole(key + " log column matched: " + match + ". Updating to: " + valueToWriteBack, true, 3);
                }
                catch (e) {
                    LogToConsole(e, true, 1);
                }
            }
            else {
                try {
                    newStatus.push([key, valueToWriteBack]); //Create a new row  
                    if (Debug)
                        LogToConsole(key + " log column not found, adding it with value: " + valueToWriteBack, true, 3);
                }
                catch (e) {
                    LogToConsole(e, true, 1);
                }
            }
        }
    }
    LogToConsole("Writing latest log row: " + newStatus, true, 2);
    SpreadsheetApp.getActive().getSheetByName("Status").getRange(1, 1, newStatus.length, 2).setValues(newStatus);
    //SpreadsheetApp.getActive().setNamedRange("statusKeys", 'Status!A2:B');
}

// Compiled using ts2gas 3.6.2 (TypeScript 3.9.3)
function Test_SaveToLog() {
    FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
    SaveToLog(FakeJSONData.Log);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Stores the received data on the Log sheet
function SaveToLog(Log) {
    console.log("Updating Log sheet...");
    var logSheet = SpreadsheetApp.getActive().getSheetByName("Log");
    var headings = logSheet.getDataRange().offset(0, 0, 1).getValues()[0]; //Get first row of the Log sheet
    var rowToInsert = [];
    for (var i = 0; i < headings.length; i++) {
        rowToInsert.push(null); //pre-fill with null values
    }
    rowToInsert[0] = (Utilities.formatDate(new Date(), GetSettingsValue("Time zone"), GetSettingsValue("Date format"))); //Place the current date+time to the first cell
    var Components = Object.getOwnPropertyNames(Log);
    for (var i = 0; i < Components.length; i++) {
        var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
        for (var j = 0; j < Properties.length; j++) {
            var key = Components[i] + '_' + Properties[j];
            if (!headings.includes(key)) {
                headings.push(key); //add key to the header's end
                rowToInsert.push(); //add empty value to the end
            }
            rowToInsert[headings.indexOf(key)] = Log[Components[i]][Properties[j]];
        }
    }
    LogToConsole("Row to insert: ", true, 1);
    LogToConsole(headings, true, 2);
    LogToConsole(rowToInsert, true, 2);
    logSheet.getRange(1, 1, 1, headings.length).setValues([headings]);
    logSheet.appendRow(rowToInsert);
    //Apply some formatting
    logSheet.getRange(1, 1, 1, logSheet.getLastColumn()).setFontWeight("bold"); //Set header bold (Frendly name)
    logSheet.setFrozenRows(2); //sets headers row frozen to the top
    logSheet.autoResizeColumns(1, logSheet.getLastColumn()); //resize columns to fit the data  
}

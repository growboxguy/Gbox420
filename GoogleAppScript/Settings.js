// Compiled using ts2gas 3.6.2 (TypeScript 3.9.3)
var Debug = GetSettingsValue("Stackdriver logging") == "Debug"; //Activates extra Stackdriver logs
function Test_GetSettingsValue() {
    console.log(GetSettingsValue("Stackdriver logging"));
}
function Test_UpdateColumns() {
    FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
    UpdateColumns(FakeJSONData.Log);
}
function GetSettingsValue(key) {
    //var SettingsLog = SpreadsheetApp.getActive().getRangeByName("Settings").getValues();
    var match = GetNamedRange("Settings").filter(function (row) {
        return row[0] == key;
    });
    if (match != null && match.length > 0) {
        if (Debug)
            LogToConsole(key + " settings key matched: " + match[0][1], true, 1); //Normally this should be a single row
        return match[0][1]; //return the first matching row's second colum
    }
    else {
        LogToConsole("No match for settings key: " + key, true, 1);
        return null;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the Settings sheet: add newly discovered keys to Settings - Column section
function UpdateColumns(Log) {
    LogToConsole("Updating Columns in Settings sheet...", true, 1);
    var columns = GetNamedRange("Columns");
    var nextRow = getLastRowInRange(columns) + 1; //Get next row after the last row
    //Add all other Header + Data pairs from the received Log JSON
    var Components = Object.getOwnPropertyNames(Log);
    for (var i = 0; i < Components.length; i++) {
        var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
        for (var j = 0; j < Properties.length; j++) {
            var key = Components[i] + '_' + Properties[j];
            var match = columns.filter(function (row) {
                return row[0] == key;
            });
            if (Debug)
                LogToConsole(key + " column match:" + match, true, 2);
            if (match == null || match.length == 0) { //If settings row does not exists
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, 1).setValue(key); //Insert key in first Key column  
                //Adding enable/disable alert checkbox
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, settings_AlertEnabled_Column).insertCheckboxes();
                //Adding YES/NO options for Triggered column
                var YesNo = SpreadsheetApp.newDataValidation().requireValueInList(['YES', 'NO']).build();
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, settings_Triggered_Column).setDataValidation(YesNo);
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, settings_Triggered_Column).setHorizontalAlignment("right");
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, settings_Triggered_Column).setValue('NO'); //By default do not enable the new alert
                //Adding Type column
                var DataType = SpreadsheetApp.newDataValidation().requireValueInList(SupportedDataTypes).build();
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, settings_DataType_Column).setDataValidation(DataType); //Add YES/NO options for Triggered column
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, settings_DataType_Column).setHorizontalAlignment("right");
                SpreadsheetApp.getActive().getSheetByName("Settings").getRange(nextRow, settings_DataType_Column).setValue('Text'); //By default do not enable the new alert        
                nextRow++;
            }
        }
    }
    SpreadsheetApp.getActive().getSheetByName("Settings").autoResizeColumns(1, SpreadsheetApp.getActive().getSheetByName("Settings").getLastColumn()); //resize columns to fit the data 
}

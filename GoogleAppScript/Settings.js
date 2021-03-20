
///< Columns section

var Debug = GetSettingsValue("Stackdriver logging") == "Debug"; //Activates extra Stackdriver logs

function Test_GetSettingsValue() {
    console.log(GetSettingsValue("Stackdriver logging"));
}

function Test_UpdateColumns() {
    UpdateColumns(getTestJSONData().Log);
}

function GetSettingsValue(key) {
    if (Debug) LogToConsole("Looking up settings value for: " + key, true, 3);
    //var SettingsLog = ActiveSpreadsheetApp.getRangeByName("Settings").getValues();
    var match = GetNamedRangeValues("Settings").filter(function (row) {
        return row[settings_keyColumn] == key;
    });
    if (match != null && match.length > 0) {
        if (Debug)
            LogToConsole(key + " settings key matched: " + match[settings_keyColumn][settings_valueColumn], true, 3); //Normally this should be a single row
        return match[settings_keyColumn][settings_valueColumn]; //return the first matching row's second colum
    }
    else {
        LogToConsole("No match for settings key: " + key, true, 1);
        return null;
    }
}
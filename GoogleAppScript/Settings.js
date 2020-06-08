/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Columns section

var Debug = GetSettingsValue("Stackdriver logging") == "Debug"; //Activates extra Stackdriver logs

function Test_GetSettingsValue() {
    console.log(GetSettingsValue("Stackdriver logging"));
}

function Test_UpdateColumns() {
    UpdateColumns(getTestJSONData().Log);
}

function GetSettingsValue(key) {
  if(Debug)LogToConsole("Looking up settings value for: " + key,true,3);
    //var SettingsLog = SpreadsheetApp.getActive().getRangeByName("Settings").getValues();
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Columns section

function UpdateColumns(Log) { ///< Add newly discovered keys to Settings tab - Columns section
  LogToConsole("Updating Columns in Settings sheet...", true, 0);
  var newColumnDiscovered = false;
  var columns = GetNamedRangeValues("Columns");
  //Adding all columns from the received Log JSON
  var Components = Object.getOwnPropertyNames(Log);
  for (var i = 0; i < Components.length; i++) {
    var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
    for (var j = 0; j < Properties.length; j++) {
      var key = Components[i] + '_' + Properties[j];
      var match = columns.filter(function (row) {
        return row[columns_keyColumn] == key;
      });
      if (Debug) LogToConsole(key + " column matched settings row: [" + match + "]", true, 3) ;
      if (match == null || match.length == 0) { //If settings row does not exists
        newColumnDiscovered = true;  
        addColumnsRow(key);
      }
    }
  }
  if(newColumnDiscovered) GetNamedRangeValues("Columns",true); //Force a cache refresh if a new column was added
  //SpreadsheetApp.getActive().getSheetByName("Settings").autoResizeColumns(1, SpreadsheetApp.getActive().getSheetByName("Settings").getLastColumn()); //resize columns to fit the data 
}

function test_addColumnsRow(){
 addColumnsRow("TestKey"); 
}

function addColumnsRow(newKey) {
  LogToConsole("Adding new key to Columns: " + newKey,false,3);
  var settingsSheet = SpreadsheetApp.getActive().getSheetByName("Settings");
  var columnsRange = SpreadsheetApp.getActive().getRangeByName("Columns");  
  var lastRow = getLastRowInRange(columnsRange.getValues()); 
  var lastColumn = columnsRange.getLastColumn();   
  LogToConsole(" (lastRow: "+ lastRow + " , lastColumn: " + lastColumn + ")",true,0);
  
  var range = settingsSheet.getRange(lastRow,1,1,lastColumn);  
  range.copyTo(settingsSheet.getRange(lastRow+1, 1, 1, lastColumn), {contentsOnly:false}); //copy last row, including the validation rules for each cell
  
  defaultValues = range.getValues();
  defaultValues[0][columns_keyColumn] = newKey;
  defaultValues[0][columns_alertEnabledColumn] = false;
  defaultValues[0][columns_alertMinColumn] = "";
  defaultValues[0][columns_alertMaxColumn] = "";
  defaultValues[0][columns_triggeredColumn] = "NO";
  defaultValues[0][columns_dataTypeColumn] = "Text";
  defaultValues[0][columns_chartColumn] = "-";
  defaultValues[0][columns_seriesColumn] = "line";
  defaultValues[0][columns_nameColumn] = newKey;
  defaultValues[0][columns_targetAxisColumn] = 0;
  settingsSheet.getRange(lastRow+1, 1, 1, lastColumn).setValues(defaultValues);  //Setting default values for the new column
  
}

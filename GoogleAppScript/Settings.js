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
    var match = GetNamedRangeValues("Settings").filter(function (row) {
        return row[0] == key;
    });
    if (match != null && match.length > 0) {
        if (Debug)
            LogToConsole(key + " settings key matched: " + match[0][1], true, 3); //Normally this should be a single row
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
  LogToConsole("Updating Columns in Settings sheet...", true, 0);
  var newColumnDiscovered = false;
  var columns = GetNamedRangeValues("Columns");
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
      if (Debug) LogToConsole(key + " column matched settings row: [" + match + "]", true, 3) ;
      if (match == null || match.length == 0) { //If settings row does not exists
        newColumnDiscovered = true;  
        addColumnsRow(key);
        nextRow++;
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
  defaultValues[0][0] = newKey;
  defaultValues[0][1] = false;
  defaultValues[0][2] = "";
  defaultValues[0][3] = "";
  defaultValues[0][4] = "NO";
  defaultValues[0][5] = "Text";
  defaultValues[0][6] = "-";
  defaultValues[0][7] = "line";
  defaultValues[0][8] = newKey;
  defaultValues[0][9] = 0;
  settingsSheet.getRange(lastRow+1, 1, 1, lastColumn).setValues(defaultValues);  //Setting default values for the new column
  
}

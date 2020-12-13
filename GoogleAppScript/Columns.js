
///< Columns section

function Test_UpdateColumns() {
  UpdateColumns(getTestJSONData().Log);
}

function UpdateColumns(Log) {
  FindNewColumns(Log);
  UpdateFriendlyName();
}

function UpdateFriendlyName() {
  var columns = GetNamedRangeValues("Columns", true);
  for (var i = 0; i < columns.length; i++) {
    key = columns[i][columns_keyColumn];
    friendlyName = GetFriendlyValue(key, null);
    if (friendlyName != null && friendlyName != "") {
      columns[i][columns_friendlyNameColumn] = columns[i][columns_nameColumn] + " (" + friendlyName + ")";
    }
    else {
      columns[i][columns_friendlyNameColumn] = columns[i][columns_nameColumn];
    }
    if (Debug) LogToConsole(key + " column matched friendly name: " + columns[i][columns_friendlyNameColumn], true, 3);
  }
  SaveNamedRange("Columns", columns);
}

function FindNewColumns(Log) { ///< Add newly discovered keys to Columns tab and update FriendlyName column
  LogToConsole("Updating Columns sheet...", true, 0);
  var newColumnDiscovered = false;
  var columns = GetNamedRangeValues("Columns", true);
  //Adding all columns from the received Log JSON
  var Components = Object.getOwnPropertyNames(Log);
  for (var i = 0; i < Components.length; i++) {
    if (Debug) LogToConsole("Updating " + Components[i], true, 3);
    var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
    for (var j = 0; j < Properties.length; j++) {
      var key = Components[i] + '_' + Properties[j];
      var match = columns.filter(function (row) {
        return row[columns_keyColumn] == key;
      });

      if (Debug) LogToConsole(key + " column matched settings row: [" + match + "]", true, 4);
      if (match == null || match.length == 0) { //If settings row does not exists
        newColumnDiscovered = true;
        addColumnsRow(key);
      }
    }
  }
  if (newColumnDiscovered) GetNamedRangeValues("Columns", true); //Force a cache refresh if a new column was added
  //SpreadsheetApp.getActive().getSheetByName("Settings").autoResizeColumns(1, SpreadsheetApp.getActive().getSheetByName("Settings").getLastColumn()); //resize columns to fit the data 

}

function test_addColumnsRow() {
  addColumnsRow("TestKey");
}

function addColumnsRow(newKey) {
  LogToConsole("Adding new key to Columns: " + newKey, false, 3);
  var columnsSheet = SpreadsheetApp.getActive().getSheetByName("Columns");
  var columnsRange = SpreadsheetApp.getActive().getRangeByName("Columns");
  var lastRow = getLastRowInRange(columnsRange.getValues());
  var lastColumn = columnsRange.getLastColumn();
  LogToConsole(" (lastRow: " + lastRow + " , lastColumn: " + lastColumn + ")", true, 0);

  var range = columnsSheet.getRange(lastRow, 1, 1, lastColumn);
  range.copyTo(columnsSheet.getRange(lastRow + 1, 1, 1, lastColumn), { contentsOnly: false }); //copy last row, including the validation rules for each cell

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
  columnsSheet.getRange(lastRow + 1, 1, 1, lastColumn).setValues(defaultValues);  //Setting default values for the new column  
}
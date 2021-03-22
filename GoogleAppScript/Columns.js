
///< Columns section

function Test_UpdateColumns() {
  UpdateColumns(getTestJSONData().Log);
}

function UpdateColumns(Log) {
  FindNewColumns(Log);
  UpdateFriendlyName();
}

function UpdateFriendlyName() {
  var columns = GetNamedRangeValues("Columns");
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
  var columns = GetNamedRangeValues("Columns");
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
    
      if (match == null || match.length == 0) { //If settings row does not exists
        newColumnDiscovered = true;
        addColumnsRow(key,columns);
      }
      else{
        if (Debug) LogToConsole(key + " column matched settings row: [" + match + "]", true, 4);
      }
    }
  }
  if (newColumnDiscovered) GetNamedRangeValues("Columns", true); //Force a cache refresh if a new column was added
  //ActiveSpreadsheetApp.getSheetByName("Settings").autoResizeColumns(1, ActiveSpreadsheetApp.getSheetByName("Settings").getLastColumn()); //resize columns to fit the data 

}

function test_addColumnsRow() {
  addColumnsRow("TestKey",GetNamedRangeValues("Columns"));
}

function addColumnsRow(newKey, columns) {
  LogToConsole("Adding new key to Columns: " + newKey, false, 3);  
  //var lastColumn = columnsSheet.getLastColumn();
  //var lastRow = columnsSheet.getLastRow(); 
  var lastRow = columns.length; 
  var lastColumn = columns[lastRow-1].length; 
  LogToConsole(" (lastRow: " + (lastRow+1) + " , lastColumn: " + lastColumn + ")", true, 0); 
      
  var lastRowRange = columnsSheet.getRange(lastRow,1,1,lastColumn);  
  defaultValues = lastRowRange.getValues();
  columnsSheet.insertRowsAfter(lastRow, 1);
  var newRowRange = columnsSheet.getRange(lastRow + 1, 1, 1, lastColumn);  
  
  lastRowRange.copyTo(newRowRange, { contentsOnly: false }); //copy last row, including the validation rules for each cell  
  defaultValues[0][columns_keyColumn] = newKey;
  defaultValues[0][columns_nameColumn] = newKey;
  defaultValues[0][columns_dataTypeColumn] = "Text";
  defaultValues[0][columns_alertEnabledColumn] = false;
  defaultValues[0][columns_alertMinColumn] = "";
  defaultValues[0][columns_alertMaxColumn] = "";
  defaultValues[0][columns_triggeredColumn] = "NO";  
  defaultValues[0][columns_chartColumn] = "-";
  defaultValues[0][columns_seriesColumn] = "line";
  defaultValues[0][columns_friendlyNameColumn] = newKey;
  defaultValues[0][columns_targetAxisColumn] = 0;
  newRowRange.setValues(defaultValues);  //Setting default values for the new column    
}
var status_keyColumn = 0;
var status_valueColumn = 1;

function Test_UpdateStatus() {
    FakeJSONData = JSON.parse(SpreadsheetApp.getActive().getRangeByName("LastReportJSON").getDisplayValue());
    UpdateStatus(FakeJSONData.Log);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the Status sheet
function UpdateStatus(Log) {
  LogToConsole("Updating Status sheet...",true,0);
  var status = GetNamedRangeValues("Status").filter(function (row) {
    return row[0] != "";  //filtering out empty rows
  });
  var newStatus = [];
  for (var i = 0; i < status.length; i++) {
    newStatus.push([status[i][status_keyColumn], null]); //pre-fill with null values
  }
  //Add all Key + Data pairs to the Status sheet from the received Log JSON  
  var Components = Object.getOwnPropertyNames(Log);
  if(Debug)LogToConsole(Components,true,3);
  for (var i = 0; i < Components.length; i++) {
    var Properties = Object.getOwnPropertyNames(Log[Components[i]]);
    if(Debug)LogToConsole(Log[Components[i]] + " : " + Properties,true,3);
                              for (var j = 0; j < Properties.length; j++) {
                              var key = Components[i] + '_' + Properties[j];
                          var value = Log[Components[i]][Properties[j]];
                          var valueToWriteBack = GetFriendlyValue(key, value);
    var match = -1;
    for (var k = 0; k < newStatus.length; k++) {
      if (newStatus[k][status_keyColumn] == key) {
        match = k; //Trying to find Status entry with the relayed component name
        break;
      }
    }
    if (match != -1) { //if a match was found
      try {
        newStatus[match][status_valueColumn] = valueToWriteBack; //Update matched row's second column                    
      }
      catch (e) {
        LogToConsole("Error updating status row " + key +" : "+ e, true, 1);
      }
    }
    else {  //no match found
      try {
        newStatus.push([key, valueToWriteBack]); //Create a new row  
        if (Debug)
          LogToConsole(key + " log column not found, adding it with value: " + valueToWriteBack, true, 3);
      }
      catch (e) {
        LogToConsole("Error adding status row " + key +" : " + e, true, 1);
      }
    }
  }
}
LogToConsole("Writing " + newStatus, true, 1);
SpreadsheetApp.getActive().getSheetByName("Status").getRange(1, 1, newStatus.length, 2).setValues(newStatus);
}

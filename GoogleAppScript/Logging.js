function Test_SaveToLog() {
  SaveToLog(getTestJSONData().Log);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Stores the received data on the Log sheet
function SaveToLog(Log) {
  LogToConsole("Updating Log sheet...",true,0);
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
      rowToInsert[headings.indexOf(key)] = Log[Components[i]][Properties[j]];  //insert the value under the matching heading
    }
  }
  LogToConsole("Heading: " + headings, true, 2);
  LogToConsole("Data: " + rowToInsert, true, 2);
  logSheet.getRange(1, 1, 1, headings.length).setValues([headings]);
  logSheet.appendRow(rowToInsert);
  //Apply some formatting
  logSheet.getRange(1, 1, 1, logSheet.getLastColumn()).setFontWeight("bold"); //Set header bold (Frendly name)
  logSheet.setFrozenRows(2); //sets headers row frozen to the top
  logSheet.autoResizeColumns(1, logSheet.getLastColumn()); //resize columns to fit the data  
}

function GetLogColumnRange(name,rowLimit){  //returns the entire column with the matching header name
  var logSheet = SpreadsheetApp.getActive().getSheetByName("Log");
  var logHeader = logSheet.getDataRange().offset(0, 0, 1).getValues()[0]; //Get first row of the Log sheet
  
  match = null;
  for (var column in logHeader) {
    if (logHeader[column] == name) {
      match = parseInt(column);
    }
  }
  if(match != null)
  {
    if(Debug)LogToConsole(name + " header matched log column: "+ match,true,3);
    if(!isNaN(parseInt(rowLimit)) && isFinite(rowLimit) && rowLimit>0 && logSheet.getLastRow()>rowLimit){
      return logSheet.getRange(logSheet.getLastRow()-rowLimit, match+1, logSheet.getLastRow(), 1);
    }
    else{
      return logSheet.getRange(1, match+1, logSheet.getLastRow(), 1);
    }
  }
  else
  {
    LogToConsole(name + " header did not match any log columns",true,3);
    return null;
  }  
}

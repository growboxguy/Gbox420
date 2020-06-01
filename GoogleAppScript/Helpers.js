// Compiled using ts2gas 3.6.2 (TypeScript 3.9.3)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper functions: Retrieves a value for other functions
function getLastRowInRange(range) {
    var rowNum = range.length;
    var blank = false;
    for (var i = 0; i < range.length; i++) {
        if (range[i][0] === "" && !blank) {
            rowNum = i;
            blank = true;
        }
        else if (range[i][0] !== "") {
            blank = false;
        }
    }
    if (Debug)
        LogToConsole("First blank row in range: " + rowNum, true, 3);
    return rowNum;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Stackdriver logging - Add a log entry ( View it from Sheets: View \  Stackdriver logging
function LogToConsole(message, breakRow, indent) {
    var cache = CacheService.getScriptCache();
    var messageToLog = cache.get("previousMessage");
    if (indent > 0) {
        for (i = 0; i < indent; i++) {
            messageToLog += ">";
        }
    }
    messageToLog += message;
    if (breakRow == true) {
        console.log(messageToLog);
        messageToLog = "";
    }
    cache.put("previousMessage", messageToLog);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper functions: For getting settings
function Test_GetNamedRangeValues() {
    console.log(GetNamedRangeValues("Settings", false));
}

function GetNamedRangeValues(rangeName, dropCache) {
    var cache = CacheService.getScriptCache();
    var cached = cache.get(rangeName);
    if (cached != null && dropCache != true) {
        //console.log("Returning cached version of: " +  rangeName);
        return JSON.parse(cached);
    }
    //console.log("Refreshed cached version of: " +  rangeName);
    var rangeData = SpreadsheetApp.getActive().getRangeByName(rangeName).getValues();
    cache.put(rangeName, JSON.stringify(rangeData), 120); // cache for 120 seconds to ensure it is not queried multiple times during script execution
    return rangeData;
}

function WipeCache(){  ///Force to drop all cached named ranges
  LogToConsole("Wiping cached Named ranges", true, 1);
  var storedCache = CacheService.getUserCache();
  CacheService.getUserCache().remove(storedCache);

}

function Test_SaveNamedRange() {
    var data = GetNamedRangeValues("Columns", false);
    data[0][0] = "TEST";  //Updates the first cell in the range
    SaveNamedRange("Columns",data);
}

function SaveNamedRange(rangeName, data) {  //updates a Named Range in Google Sheets with data (type: Object[][] )
    SpreadsheetApp.getActive().getRangeByName(rangeName).setValues(data);    
}

function Test_GetFriendlyValue(){
  GetFriendlyValue("Lt1_On","16:20");
}

function GetFriendlyValue(key, value) {
  var returnValue = value; //This will get overwritten if a friendly value is found
  var settingsMatch = GetNamedRangeValues("Columns").filter(function (row) {
    return row[0] == key;
  });
  if (settingsMatch == null) { //If key is not found
    LogToConsole(key + " : " + value + ", type: UNKNOWN", true, 1);
  }
  else {
    var dataType = settingsMatch[0][5];
    switch (dataType) {
      case "Date":
        returnValue = Utilities.formatDate(value, GetSettingsValue("Time zone"), GetSettingsValue("Date format")); //https://docs.oracle.com/javase/7/docs/api/java/text/SimpleDateformat.html
        break;
      case "Distance":
        if (GetSettingsValue("Units") == "Metric") {
          returnValue = value + ' cm';
        }
        else {
          returnValue = value + ' inch';
        }
        break;
      case "Current":
        returnValue = value + ' A';
        break;
      case "EnabledDisabled":
        switch (value) {
          case "0":
            returnValue = "Disabled";
            break;
          case "1":
            returnValue = "Enabled";
            break;
        }
        break;
      case "Energy":
        returnValue = value + ' kWh';
        break;
      case "FanSpeed":
        switch (value) {
          case "0":
            returnValue = "OFF";
            break;
          case "1":
            returnValue = "LOW";
            break;
          case "2":
            returnValue = "HIGH";
            break;
        }
        break;
      case "Minute":
        returnValue = value + ' min';
        break;
      case "OnOff":
        switch (value) {
          case "0":
            returnValue = "OFF";
            break;
          case "1":
            returnValue = "ON";
            break;
        }
        break;
      case "Percentage":
        returnValue = value + ' %';
        break;
      case "Power":
        returnValue = value + ' W';
        break;
      case "PumpState":
        switch (value) {
          case "0":
            returnValue = "DISABLED";
            break;
          case "1":
            returnValue = "IDLE";
            break;
          case "2":
            returnValue = "PRIMING";
            break;
          case "3":
            returnValue = "RUNNING";
            break;
          case "4":
            returnValue = "BLOWOFF";
            break;
          case "5":
            returnValue = "MIXING";
            break;
        }
        break;
      case "Pressure":
        if (GetSettingsValue("Units") == "Metric") {
          returnValue = value + ' bar';
        }
        else {
          returnValue = value + ' psi';
        }
        break;
      case "Second":
        returnValue = value + ' sec';
        break;
      case "Temperature":
        if (GetSettingsValue("Units") == "Metric") {
          returnValue = value + ' °C';
        }
        else {
          returnValue = value + ' °F';
        }
        break;
      case "Time":
        var Time = new Date();
        Time.setHours(value.split(":")[0]);
        Time.setMinutes(value.split(":")[1]);
        returnValue = Utilities.formatDate(Time, "GMT", GetSettingsValue("Time format"));  //Arriving date is already in the correct time zone, using GMT to avoid changing the time zone
        break;
      case "YesNo":
        switch (value) {
          case "0":
            returnValue = "NO";
            break;
          case "1":
            returnValue = "YES";
            break;
        }
        break;
      case "Voltage":
        returnValue = value + ' V';
        break;
      case "Weight":
        if (GetSettingsValue("Units") == "Metric") {
          returnValue = value + ' kg';
        }
        else {
          returnValue = value + ' lbs';
        }
        break;
      default: //Number and Text returnValue = as they are
        returnValue = value;
        break;
    }
    if (Debug)
      LogToConsole(key + " : " + value + " , type: " + dataType + " , FriendlyValue: " + returnValue, true, 3);
  }
  return returnValue;
}

/*
function GetLatestLogEntry(key, useFriendlyFormat) {
    var match = logHeader.createTextFinder(key).matchEntireCell(true).findNext();
    if (match == null) { //If key is not found
        return null;
    }
    else {
        var value = logSheet.getRange(logSheet.getLastRow(), match.getColumn()).getValue();
        if (!useFriendlyFormat) {
            return value;
        }
        else //When Friendly format is requested
         {
            return GetFriendlyName(key, value);
        }
    }
}
*/

function scrollToLast() {
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
    var range = sheet.getRange(sheet.getLastRow(), 1);
    sheet.setActiveSelection(range);
}

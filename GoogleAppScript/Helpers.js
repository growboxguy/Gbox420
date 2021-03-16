
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


///<  Stackdriver logging - Add a log entry ( View it from Sheets: View \  Stackdriver logging
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

//Helper functions: For getting settings
function Test_GetNamedRangeValues() {
  LogToConsole(GetNamedRangeValues("Status", false), true, 1);
}

function GetNamedRangeValues(rangeName, dropCache) {
  var cache = CacheService.getScriptCache();
  var cached = cache.get(rangeName);
  if (cached != null && dropCache != true) {
    if (Debug) LogToConsole("Returning cached version of: " + rangeName, true, 2);
    return JSON.parse(cached);
  }
  else {
    if (Debug) LogToConsole("Updating cache for: " + rangeName, true, 2);
    var rangeData = SpreadsheetApp.getActive().getRangeByName(rangeName).getValues();
    cache.put(rangeName, JSON.stringify(rangeData), 21600); // cache for 6 hours to ensure it is not queried multiple times during script execution
    return rangeData;
  }
}

function LoadCache(rangeName)
{
   var rangeData = SpreadsheetApp.getActive().getRangeByName(rangeName).getValues();
   CacheService.getScriptCache().put(rangeName, JSON.stringify(rangeData), 21600); // cache for 6 hours to ensure it is not queried multiple times during script execution
}

function WipeCache() {  ///< Force to drop all cached named ranges
  LogToConsole("Wiping cached Named ranges", true, 1);
  var cache = CacheService.getScriptCache();
  cache.remove("Charts");
  cache.remove("Columns");
  cache.remove("Settings");
  cache.remove("Status");
}

function ReloadCache() {  ///< Force to drop all cached named ranges
  LogToConsole("Reloading cached Named ranges", true, 1);
  WipeCache();
  LoadCache("Charts");
  LoadCache("Columns");
  LoadCache("Settings");
  LoadCache("Status");
}

function Test_SaveNamedRange() {
  var data = GetNamedRangeValues("Columns", false);
  data[0][0] = "TEST";  //Updates the first cell in the range
  SaveNamedRange("Columns", data);
}

function SaveNamedRange(rangeName, data) {  //updates a Named Range in Google Sheets with data (type: Object[][] )
  SpreadsheetApp.getActive().getRangeByName(rangeName).setValues(data);
  var cache = CacheService.getScriptCache();
  cache.remove(rangeName);  //Clear cached entry
  cache.put(rangeName, JSON.stringify(data), 21600); // cache for 6 hours to ensure it is not queried multiple times during script execution
}

function Test_GetFriendlyColumnName() {
  LogToConsole(GetFriendlyColumnName("Lt1_On"), true, 0);
}

function GetFriendlyColumnName(key, includeUnits = true) {
  var match = GetNamedRangeValues("Columns").filter(function (row) {
    return row[columns_keyColumn] == key;
  });
  if (match == null) { //If key is not found
    return key;
  }
  else {
    if (includeUnits)
      return match[0][columns_friendlyNameColumn];
    else
      return match[0][columns_nameColumn];
  }
}

function Test_GetFriendlyValue() {
  GetFriendlyValue("Lt1_On", "16:20");
}

function GetFriendlyValue(key, value) {
  try {
    var match = GetNamedRangeValues("Columns").filter(function (row) {
      return row[columns_keyColumn] == key;
    });
    if (match == null) { //If key is not found
      LogToConsole(key + " : " + value + ", type: UNKNOWN", true, 1);
      return value;
    }
    else {
      var dataType = match[0][columns_dataTypeColumn];
      var returnValue = "";
      switch (dataType) {
        case "AeroNoTankState":
          if (value != null) {
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
                returnValue = "SPRAY";
                break;
              case "4":
                returnValue = "RELEASE";
                break;
              case "5":
                returnValue = "MIX";
                break;
            }
          }
          break;
        case "AeroTankState":
          if (value != null) {
            switch (value) {
              case "0":
                returnValue = "DISABLED";
                break;
              case "1":
                returnValue = "IDLE";
                break;
              case "2":
                returnValue = "SPRAY";
                break;
              case "3":
                returnValue = "STOPSPRAY";
                break;
              case "4":
                returnValue = "RELEASE";
                break;
              case "5":
                returnValue = "REFILL";
                break;
              case "6":
                returnValue = "DRAIN";
                break;
              case "7":
                returnValue = "MIX";
                break;
            }
          }
          break;
        case "Date":
          if (value != null) {
            returnValue = Utilities.formatDate(value, GetSettingsValue("Time zone"), GetSettingsValue("Date format")); //https://docs.oracle.com/javase/7/docs/api/java/text/SimpleDateformat.html
          }
          break;
        case "Distance":
          if (value != null) {
            returnValue = value + " ";
          }
          if (GetSettingsValue("Units") == "Metric") {
            returnValue += 'cm';
          }
          else {
            returnValue += 'inch';
          }
          break;
        case "Current":
          if (value != null) {
            returnValue = value + " ";
          }
          returnValue += 'A';
          break;
        case "EnabledDisabled":
          if (value != null) {
            switch (value) {
              case "0":
                returnValue = "DISABLED";
                break;
              case "1":
                returnValue = "ENABLED";
                break;
            }
          }
          break;
        case "Energy":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += "kWh";
          break;
        case "FanSpeed":
          if (value != null) {
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
          }
          break;
        case "Frequency":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += 'hz';
          break;
        case "HempyState":
          if (value != null) {
            switch (value) {
              case "0":
                returnValue = "DISABLED";
                break;
              case "1":
                returnValue = "IDLE";
                break;
              case "2":
                returnValue = "WATERING";
                break;
              case "3":
                returnValue = "DRAINING";
                break;
            }
          }
          break;
        case "Minute":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += 'min';
          break;
        case "OnOff":
          if (value != null) {
            switch (value) {
              case "0":
                returnValue = "OFF";
                break;
              case "1":
                returnValue = "ON";
                break;
            }
          }
          break;
        case "OnlineOffline":
          if (value != null) {
            switch (value) {
              case "0":
                returnValue = "OFFLINE";
                break;
              case "1":
                returnValue = "ONLINE";
                break;
            }
          }
          break;
        case "Percentage":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += '%';
          break;
        case "Power":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += 'W';
          break;
        case "PPM":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += 'ppm';
          break;
        case "PressurePumpState":
          if (value != null) {
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
                returnValue = "MIX";
                break;
              case "6":
                returnValue = "BYPASSCLOSE";
                break;
              case "7":
                returnValue = "BYPASSOPEN";
                break;
            }
          }
          break;
        case "Pressure":
          if (value != null) {
            returnValue = value + ' ';
          }
          if (GetSettingsValue("Units") == "Metric") {
            returnValue += 'bar';
          }
          else {
            returnValue += 'psi';
          }
          break;
        case "Second":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += 'sec';
          break;
        case "Temperature":
          if (value != null) {
            returnValue = value + ' ';
          }
          if (GetSettingsValue("Units") == "Metric") {
            returnValue += '°C';
          }
          else {
            returnValue += '°F';
          }
          break;
        case "Time":
          if (value != null) {
            var Time = new Date();
            Time.setHours(value.split(":")[0]);
            Time.setMinutes(value.split(":")[1]);
            returnValue = Utilities.formatDate(Time, "GMT", GetSettingsValue("Time format"));  //Arriving date is already in the correct time zone, using GMT to avoid changing the time zone
          }
          break;
        case "YesNo":
          if (value != null) {
            switch (value) {
              case "0":
                returnValue = "NO";
                break;
              case "1":
                returnValue = "YES";
                break;
            }
          }
          break;
        case "Voltage":
          if (value != null) {
            returnValue = value + ' ';
          }
          returnValue += 'V';
          break;
        case "WaterPumpState":
          if (value != null) {
            switch (value) {
              case "0":
                returnValue = "DISABLED";
                break;
              case "1":
                returnValue = "IDLE";
                break;
              case "2":
                returnValue = "RUNNING";
                break;
            }
          }
          break;
        case "Weight":
          if (value != null) {
            returnValue = value + ' ';
          }
          if (GetSettingsValue("Units") == "Metric") {
            returnValue += 'kg';
          }
          else {
            returnValue += 'lbs';
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
  catch (e) {
    LogToConsole("Error processing friendly name: " + e, true, 0);
    return returnValue;
  }
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
  LogToConsole("Scrolling to last row...", true, 0);
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  sheet.setActiveSelection(sheet.getRange(1, 1));
  var range = sheet.getRange(sheet.getLastRow(), 1);
  sheet.setActiveSelection(range);
}

function cleanUpDebug() { /// < Removes log entries with debug mode enabled
  LogToConsole("Cleaning up the logs from debug messages...", true, 0);
  var logSheet = SpreadsheetApp.getActive().getSheetByName("Log");
  var logHeader = logSheet.getDataRange().offset(0, 0, 1).getValues()[0]; //Get first row of the Log sheet

  match = null;
  for (var column in logHeader) {
    if (logHeader[column] == "Main1_D") {
      match = parseInt(column);
    }
  }
  if (match != null) {
    if (Debug) LogToConsole("Main1_D matched log column: " + match, true, 3);
    var range = logSheet.getDataRange().offset(2,0);
    var column = match;
    var deleteCriteria = "1";  //Delete rows where cell content equals criteria        
    var rangeValues = range.getValues();

    var filteredValues = rangeValues.filter(function (val) {
      return val[column] != deleteCriteria;
    });
    range.clearContent();
    var newRange = logSheet.getRange(3, 1, filteredValues.length, filteredValues[0].length);
    newRange.setValues(filteredValues);
  }
  else {
    LogToConsole("Main1_D did not match any log columns", true, 3);
    return null;
  }
}
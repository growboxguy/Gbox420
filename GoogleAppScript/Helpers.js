/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper functions: Retrieves a value for other functions

function getLastRowInRange(range){
  var rowNum = range.length;
  var blank = false;
  for(var i = 0; i < range.length; i++)
  {
    
    if(range[i][0] === "" && !blank){
      rowNum = i;
      blank = true;      
    }
    else if(range[i][0] !== ""){
      blank = false;
    }
  }
  if(Debug) LogToConsole("First blank row in range: " + rowNum,true,4);
  return rowNum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Stackdriver logging - Add a log entry ( View it from Sheets: View \  Stackdriver logging

function LogToConsole(message,breakRow,indent)
{
  var cache = CacheService.getScriptCache();
  var messageToLog = cache.get("previousMessage");
  if(indent>0){
    for(i=0;i<indent;i++){
      messageToLog += "_";
    }
  }
  messageToLog += message;
  if(breakRow == true){
    console.log(messageToLog);
    messageToLog = "";
  }
  cache.put("previousMessage", messageToLog);  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Helper functions: For getting settings

function Test_GetNamedRange(){
  console.log(GetNamedRange("Settings",false));
  }

function GetNamedRange(rangeName,dropCache)  /// Returns a 2D array of the values
{  
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


function Test_GetSettingsValue(){
console.log(GetSettingsValue("Units"));
}

function GetSettingsValue(key){   
  //var SettingsLog = SpreadsheetApp.getActive().getRangeByName("Settings").getValues();
  var match = GetNamedRange("Settings").filter(function(row){
    return row[0] == key;
  });
  if(match != null){
    console.log(key + " settings key matched: " +  match[0][1]); //Normally this should be a single row
    return match[0][1];  //return the first matching row's second colum
  }
  else
  {
    console.log("No match for settings key: " +  key);
    return null;
  }
}

function Test_GetFriendlyValue(){
  console.log(GetFriendlyValue("EFan_Speed",1));
}

function GetFriendlyValue(key,rawValue)
{
  var settingsMatch = GetNamedRange("Columns").filter(function(row){
    return row[0] == key;
  });
  console.log(settingsMatch);
  if(settingsMatch == null){ //If key is not found
    console.log(key + " : " + rawValue + ", type: UNKNOWN");
    return rawValue;
  }
  else {
    var dataType = settingsMatch[0][5];
    console.log(key + " : " + rawValue + ", type: " + dataType);
  }
  
  switch (dataType)
  {
    case "Date":
      return Utilities.formatDate(rawValue,GetSettingsValue("Time zone"),GetSettingsValue("Date format")) ;  //https://docs.oracle.com/javase/7/docs/api/java/text/SimpleDateformat.html
      break;
    case "Distance":
      if(GetSettingsValue("Units") == "Metric")
      {
        return rawValue+' cm';
      }
      else
      {
        return rawValue+' inch';
      }
      break;
    case "Current":
      return rawValue+' A';
      break;
    case "EnabledDisabled":
      switch (rawValue)  
      {
        case 0:
          return "Disabled";
          break;
        case 1:
          return "Enabled";
          break;              
      }
      break;
    case "Energy":
      return rawValue+' kWh';
      break;
    case "FanSpeed":
      switch (rawValue)  
      {
        case 0:
          return "OFF";
          break;
        case 1:
          return "LOW";
          break;
        case 2:
          return "HIGH";
          break; 
      }
      break;
    case "Minute":
      return rawValue+' min';
      break;
    case "OnOff":
      switch (rawValue) 
      {
        case 0:
          return "OFF";
          break;
        case 1:
          return "ON";
          break;              
      }
      break;
    case "Percentage":
      return rawValue+' %';
      break;
    case "Power":
      return rawValue+' W';
      break;
    case "PumpState":
      switch (rawValue)  
      {
        case 0:
          return "DISABLED";
          break;
        case 1:
          return "IDLE";
          break;
        case 2:
          return "PRIMING";
          break;
        case 3:
          return "RUNNING";
          break;
        case 4:
          return "BLOWOFF";
          break;
        case 5:
          return "MIXING";
          break;
      }
      break;
    case "Pressure":
      if(GetSettingsValue("Units") == "Metric")
      {
        return rawValue+' bar';
      }
      else
      {
        return rawValue+' psi';
      }
      break;
    case "Second":
      return rawValue+' sec';
      break;
    case "Temperature":
      if(GetSettingsValue("Units") == "Metric")
      {
        return rawValue+' °C';
      }
      else
      {
        return rawValue+' °F';
      }
      break;         
    case "Time":
      console.log(rawValue +' , '+ GetSettingsValue("Time zone")  +' , '+GetSettingsValue("Time format"))
      var Time = new Date(rawValue);
      return Utilities.formatDate(Time,GetSettingsValue("Time zone"),GetSettingsValue("Time format"));
      break; 
    case "YesNo":
      switch (rawValue)  {
        case 0:
          return "NO";
          break;
        case 1:
          return "YES";
          break;              
      }
      break;
    case "Voltage":
      return rawValue+' V';
      break;
    case "Weight":
      if(GetSettingsValue("Units") == "Metric")
      {
        return rawValue+' kg';
      }
      else
      {
        return rawValue+' lbs';
      }
      break;
    default:   //Number and Text return as they are
      return rawValue;
      break;
  }  
}

function GetLatestLogEntry(key,useFriendlyFormat){
  var match = logHeader.createTextFinder(key).matchEntireCell(true).findNext();
  if(match == null){ //If key is not found
    return null;
  }
  else {
    var rawValue = logSheet.getRange(logSheet.getLastRow(), match.getColumn()).getValue();
    
    if(!useFriendlyFormat)
    {
      return rawValue;
    }      
    else  //When Friendly format is requested
    {       
      return GetFriendlyName(key,rawValue);
    }
  }
}
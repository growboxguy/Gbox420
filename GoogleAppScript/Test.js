// IGNORE THIS SECTION, just a playground for testing new functions :)



//Trigger a fake JSON report for testing

function getTestJSONData() {
  return JSON.parse(ActiveSpreadsheetApp.getRangeByName("LastReportJSON").getDisplayValue());
}


function RunFakeReport() {
  LogToConsole(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", true, 0);
  ReloadCache(); ///< Remove cached Named Ranges and get a fresh copy  
  ActiveSpreadsheetApp.getRangeByName("LastReportTime").setValue(Utilities.formatDate(new Date(), GetSettingsValue("Time zone"), GetSettingsValue("Date format")));
  ImportResultRange.setValue("Processing...");
  LogToConsole("Loading fake report data...", true, 0);
  var FakeBoxData = ActiveSpreadsheetApp.getRangeByName("LastReportJSON_Simulate").getDisplayValue();
  //Browser.msgBox(FakeReportData);
  LogToConsole("Parsing fake BoxData: ", false, 1);
  try {
    var FakeBoxDataJSON = JSON.parse(FakeBoxData);
    LogToConsole(JSON.stringify(FakeBoxDataJSON), true, 0);
  }
  catch (e) {
    LogToConsole("Error parsing BoxData to JSON. Error: " + e, true, 0);
    ImportResultRange.setValue("Error parsing BoxData to JSON. Error: " + e);
  }
  ProcessBoxData(FakeBoxDataJSON);
  LogToConsole(">>>>>>>End of script>>>>>>>", true, 0);
}

/*
function Test_MainModule_CheckAlerts() {
FakeJSONData = JSON.parse(ActiveSpreadsheetApp.getRangeByName("LastReportJSON").getDisplayValue());
CheckAlerts(FakeJSONData.Log);
}
*/
function Test_LightSensor1_ProcessBoxData() {
  ProcessBoxData(FakeJSONData_LightCalibration);
}
//
// Stuff under development
function UpdateCalibrationValues(Name, Readings) {
  Logger.log(Readings); //Log the received data on the console output  
  var settings = calibrationSheet.getRange(3, 1, calibrationSheet.getLastRow(), 1);
  var nextRow = calibrationSheet.getLastRow() + 1; //Get next row after the last row
  //Add all other Header + Data pairs from the received Log JSON
  var match = settings.createTextFinder(Name).matchEntireCell(true).findNext(); //lookup the alert limit row for each Log key
  if (match == null) { //If settings row does not exists
    calibrationSheet.getRange(nextRow, 1).setValue(Name); //Insert key in first column     
    calibrationSheet.getRange(nextRow, 2).setValue(Readings); //Insert value in second column
    nextRow++;
  }
  else {
    calibrationSheet.getRange(match.getRow(), 2).setValue(Readings); //Update value in second column
  }
  for (var i = 0; i < Readings.length; i++) {
    calibrationSheet.getRange(3 + i, 5).setValue(Readings[i]);
  }
}

//Updates the Settings sheets, adds newly discovered keys 
/*
function UpdateColumns(Settings){
Logger.log(Settings);  //Log the received data on the console output
var settings = settingsSheet.getRange(3,9,settingsSheet.getLastRow(),1);
var nextRow = settingsSheet.getLastRow() + 1; //Get next row after the last row

//Add all other Header + Data pairs from the received Log JSON
for (var key in Object.keys(Settings)){
var match = settings.createTextFinder(key).matchEntireCell(true).findNext(); //lookup the alert limit row for each Log key
if(match == null){ //If settings row does not exists
settingsSheet.getRange(nextRow, 1).setValue(key);   //Insert key in first column
settingsSheet.getRange(nextRow, 2).setValue(Settings[key]);   //Insert value in second column
nextRow++;
}
else
{
settingsSheet.getRange(match.getRow(), 2).setValue(Settings[key]);   //Update value in second column
}
}
}*/
/**
* Write to multiple, disjoint data ranges.
* @param {string} spreadsheetId The spreadsheet ID to write to.
*/

function writeToMultipleRanges(spreadsheetId) {
  // Specify some values to write to the sheet.
  var columnAValues = [
    ['Item', 'Wheel', 'Door', 'Engine']
  ];
  var rowValues = [
    ['Cost', 'Stocked', 'Ship Date'],
    ['$20.50', '4', '3/1/2016']
  ];
  var request = {
    'valueInputOption': 'USER_ENTERED',
    'data': [
      {
        'range': 'Sheet1!A1:A4',
        'majorDimension': 'COLUMNS',
        'values': columnAValues
      },
      {
        'range': 'Sheet1!B1:D2',
        'majorDimension': 'ROWS',
        'values': rowValues
      }
    ]
  };
  var response = Sheets.Spreadsheets.Values.batchUpdate(request, spreadsheetId);
  Logger.log(response);
}
//function onOpen(){  //When spreadsheet is opened go to Log sheet's latest entry
// var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName("Log");
// var range = sheet.getRange(sheet.getLastRow(),1);
// sheet.setActiveRange(range);
//}



//https://myengineeringworld.net/2018/10/column-charts-goolge-sheets.html
function createColumnChart() {

  /*
  ------------------------------------------------------------------------------
  The function creates a custom column chart in the active sheet using the data
  that exist in the range that is specified in the dataRange variable.
  
  Written By:  Christos Samaras
  Date:      30/10/2018
  Last Updated:  07/11/2018
  E-mail:    xristos.samaras@gmail.com
  Site:      https://www.myengineeringworld.net
  ------------------------------------------------------------------------------
  */

  // Range to get the data.
  var dataRange = 'A1:C4';

  // Variables for customizing colors.
  // Use this tool to generate "good" colors:
  // http://paletton.com
  var color1 = '#EE295A';   // Fill and data label color for the first series.
  var color2 = '#FFD42C';   // Fill and data label color for the second series.
  var color3 = '#8D7CEE';   // Trendline and axis titles color.
  var color4 = '#A6F870';   // Gridlines, axis labels and chart title color.

  // Text for titles (chart and both axes).
  var chartTitle = 'Vehicles Comparisons';
  var xAxisTitle = 'Vehicle Category';
  var yAxisTitle = 'Speed & Power';

  // Get the active sheet.
  var sheet = ActiveSpreadsheetApp.getActiveSheet();

  // Create a chart.
  var chart = sheet.newChart()
    .asColumnChart()
    .addRange(ActiveSpreadsheetApp.getRange(dataRange))

    // Set the generic options.
    .setNumHeaders(1)
    .setBackgroundColor('#333333')
    //  .setOption('is3D', true)   // For a 3D chart.
    //  .setOption('height', 400)  // Custom height. 
    //  .setOption('width', 600)   // Custom width.

    // Set the chart title options.
    .setOption('title', chartTitle)
    .setOption('titleTextStyle', {
      color: color4,
      alignment: 'center',
      fontName: 'Verdana',
      fontSize: 24,
      bold: true
    })

    // Set the legend options.
    .setOption('legend', {
      position: 'top',
      alignment: 'center',
      textStyle: {
        color: color3,
        fontName: 'Verdana',
        fontSize: 12,
        bold: true
      }
    })

    // Set the X-axis options.
    .setOption('hAxis', {
      title: xAxisTitle,
      titleTextStyle: {
        color: color3,
        alignment: 'center',
        fontName: 'Verdana',
        fontSize: 16,
        bold: true
      },
      textStyle: {
        color: color4,
        fontName: 'Verdana',
        fontSize: 14,
        bold: true
      }
    })

    // Set the Y-axis options.
    .setOption('vAxes', {
      0: {
        title: yAxisTitle,
        format: 'none',
        titleTextStyle: {
          color: color3,
          alignment: 'center',
          fontName: 'Verdana',
          fontSize: 16,
          bold: true
        },
        textStyle: {
          color: color4,
          fontName: 'Verdana',
          fontSize: 14,
          bold: true
        },
        gridlines: {
          count: -1,
          color: color4,
          width: 5
        }
      }
    })

    // Set the series options.
    .setOption("series", {
      0: {
        labelInLegend: 'Speed [km/h]',
        color: color1,
        hasAnnotations: true,
        dataLabel: 'value',
        dataLabelPlacement: 'outsideEnd',
        errorBars: {
          magnitude: 20,
          errorType: "percent"
        },
        dataLabel: "value",
        textStyle: {
          color: color1,
          fontSize: 12,
          fontName: 'Verdana',
          bold: true
        }
      },
      1: {
        labelInLegend: 'Power [kW]',
        color: color2,
        hasAnnotations: true,
        dataLabel: 'value',
        dataLabelPlacement: 'outsideEnd',
        errorBars: {
          magnitude: 20,
          errorType: "percent"
        },
        dataLabel: "value",
        textStyle: {
          color: color2,
          fontSize: 12,
          fontName: 'Verdana',
          bold: true
        }
      }
    })

    // Set the trendline options (here only for the first series).
    .setOption('trendlines', {
      0: {
        color: color3,
        type: 'linear',
        lineWidth: 4,
        opacity: 0.8,
        labelInLegend: 'Trendline',
        visibleInLegend: true
      }
    })

    // Position: row and column of top left corner (cell E2 here).
    // Optional: offset X and Y in pixels.
    .setPosition(2, 5, 0, 0)

    // Finally, build the chart.
    .build();
  sheet.insertChart(chart);

}

// End of the GAS code.

function ClearCharts(){
  var chartsSheet = SpreadsheetApp.getActive().getSheetByName("Charts");
  var charts = chartsSheet.getCharts();
  for (var i in charts) {
    chartsSheet.removeChart(charts[i]);
  }
}


function BuildCharts() {
  ClearCharts();
  
  var columns = GetNamedRange("Columns",true);
  var charts = GetNamedRange("Charts",true).filter(function (row) {
    return typeof row[0] == "number";  //filtering out blank rows based on first column (Contains the order number of the chart)
  });
  LogToConsole("Generating charts: " + charts,true,1);
  
  for(var i = 0; i < charts.length;i++){
    console.log(charts[i][1]);
    console.log(charts[i][2]);
        
    chartsSheet = SpreadsheetApp.getActive().getSheetByName("Charts");
    var chartBuilder = chartsSheet.newChart();
    
    var columnsToInclude = columns.filter(function (row) {
      return row[6] == charts[i][1];  //filtering out blank rows based on first column (Contains the order number of the chart)
    });
    console.log(columnsToInclude);
    chartBuilder.addRange(GetLogColumnRange("LogDate"));
   // var seriesType = [];
    for(var j = 0; j < columnsToInclude.length; j++)
    {   
      console.log(columnsToInclude[j]);
      chartBuilder.addRange(GetLogColumnRange(columnsToInclude[j][0])); 
      //seriesType.push({ j : { type: 'line', color: 'orange', targetAxisIndex: j }});
    }
    //console.log(seriesType);
    //chartBuilder.setOption('series', {seriesType});
    
    
    chartBuilder
    .setPosition(1 + i*29, 1, 0, 0)
    .setChartType(GetChartType(charts[i][2]))
    .setNumHeaders(1)

    // .addRange(logSheet.getRange(1, logHeader.createTextFinder("LtSen1_Reading").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
    // .addRange(logSheet.getRange(1, logHeader.createTextFinder("Lt1_Brightness").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
    .setOption('title', charts[i][1])
    .setOption('width', 800)
    .setOption('height', 600)
   // .setOption('series', {
   //   0: { type: 'line', color: 'orange', targetAxisIndex: 0 },
  //    1: { type: 'line', color: 'green', targetAxisIndex: 1 }
   // })
    //.setOption('vAxes', { 0: { title: 'Light reading' }, 1: { title: 'Brightness %' } })
    .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 });
    
        
    
    
    chartsSheet.insertChart(chartBuilder.build());    
  }  
}

function GetChartType(name)
{
  switch(name)
  {
    case "COMBO":
      return Charts.ChartType.COMBO
      break;
    case "AREA":
      return Charts.ChartType.AREA
      break;
    case "BAR":
      return Charts.ChartType.BAR
      break;
    case "BUBBLE":
      return Charts.ChartType.BUBBLE
      break;
    case "CANDLESTICK":
      return Charts.ChartType.CANDLESTICK
      break;
    case "COLUMN":
      return Charts.ChartType.COLUMN
      break;
    case "COMBO":
      return Charts.ChartType.COMBO
      break;
    case "GAUGE":
      return Charts.ChartType.GAUGE
      break;
    case "GEO":
      return Charts.ChartType.GEO
      break;
    case "HISTOGRAM":
      return Charts.ChartType.HISTOGRAM
      break;
    case "RADAR":
      return Charts.ChartType.RADAR
      break;
    case "LINE":
      return Charts.ChartType.LINE
      break;
    case "ORG":
      return Charts.ChartType.ORG
      break;
    case "PIE":
      return Charts.ChartType.PIE
      break;
    case "SCATTER":
      return Charts.ChartType.SCATTER
      break;
    case "SPARKLINE":
      return Charts.ChartType.SPARKLINE
      break;
    case "STEPPED_AREA":
      return Charts.ChartType.STEPPED_AREA
      break;
    case "TABLE":
      return Charts.ChartType.TABLE
      break;
    case "TIMELINE":
      return Charts.ChartType.TIMELINE
      break;
    case "TREEMAP":
      return Charts.ChartType.TREEMAP
      break;
    case "WATERFALL":
      return Charts.ChartType.WATERFALL
      break;
  }  
}


//Updates the dataRange of each chart on the Charts sheet
function UpdateCharts() {
    //Clear all charts
    var chartsSheet = SpreadsheetApp.getActive().getSheetByName("Charts");
  var logSheet = SpreadsheetApp.getActive().getSheetByName("Log");
  var logHeader = SpreadsheetApp.getActive().getRangeByName("logHeader");
  //  var charts = chartsSheet.getCharts();
  //  for (var i in charts) {
  //      chartsSheet.removeChart(charts[i]);
  //  }
    //Lights chart
    var chartBuilder = chartsSheet.newChart();
    var chart_Lights = chartBuilder
        .setPosition(10, 1, 0, 0)
        .setChartType(Charts.ChartType.COMBO)
        .setNumHeaders(1)
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
       // .addRange(logSheet.getRange(1, logHeader.createTextFinder("LtSen1_Reading").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
       // .addRange(logSheet.getRange(1, logHeader.createTextFinder("Lt1_Brightness").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        /*.setOption('title', 'LED lights')
        .setOption('width', 800)
        .setOption('height', 600)
        .setOption('series', {
        0: { type: 'line', color: 'orange', targetAxisIndex: 0 },
        1: { type: 'line', color: 'green', targetAxisIndex: 1 }
    })
        .setOption('vAxes', { 0: { title: 'Light reading' }, 1: { title: 'Brightness %' } })
        .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 })*/
        .build();
    chartsSheet.insertChart(chart_Lights);/*
    //PowerConsumption chart
    var chartBuilder = chartsSheet.newChart();
    var chart_PowerConsumption = chartBuilder
        .setPosition(160, 1, 0, 0)
        .setChartType(Charts.ChartType.COMBO)
        .setNumHeaders(1)
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Pow1_Power").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Pow1_Energy").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Pow1_Voltage").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Pow1_Current").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .setOption('title', 'Power consumption')
        .setOption('width', 800)
        .setOption('height', 600)
        .setOption('vAxes', { 0: { viewWindow: { min: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() - 1,
                max: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() + 1
            } } })
        .setOption('series', {
        0: { type: 'bars', color: 'blue' },
        1: { type: 'line', color: 'red' },
        2: { type: 'line', color: 'orange' },
        3: { type: 'line', color: 'green' }
    })
        .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 })
        .build();
    chartsSheet.insertChart(chart_PowerConsumption);
    //Temperature/Humidity chart
    var chartBuilder = chartsSheet.newChart();
    var chart_TempHumidity = chartBuilder
        .setPosition(90, 1, 0, 0)
        .setChartType(Charts.ChartType.LINE)
        .setNumHeaders(1)
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("DHT1_Temp").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("DHT1_Humidity").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Res1_WaterTemp").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Res1_AirTemp").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Res1_Humidity").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .setOption('title', 'Temperature/Humidity')
        .setOption('width', 800)
        .setOption('height', 600)
        .setOption('vAxes', { 0: { viewWindow: { min: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() - 1,
                max: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() + 1
            } } })
        .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 })
        .build();
    chartsSheet.insertChart(chart_TempHumidity);
    //Generate Hempy Bucket 1 chart
    var chartBuilder = chartsSheet.newChart();
    var chart_HempyBucket1 = chartBuilder
        .setPosition(30, 1, 0, 0)
        .setChartType(Charts.ChartType.LINE)
        .setNumHeaders(1)
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Hemp1_WeightB1").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Hemp1_StartB1").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Hemp1_StopB1").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .setOption('title', 'Hempy Bucket 1')
        .setOption('width', 800)
        .setOption('height', 600)
        .setOption('vAxes', { 0: { title: 'kg',
            viewWindow: { min: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StartB1").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() - 1,
                max: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StopB1").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() + 1
            } } })
        .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 })
        .build();
    chartsSheet.insertChart(chart_HempyBucket1);
    //Generate Hempy Bucket 2 chart
    var chartBuilder = chartsSheet.newChart();
    var chart_HempyBucket2 = chartBuilder
        .setPosition(60, 1, 0, 0)
        .setChartType(Charts.ChartType.LINE)
        .setNumHeaders(1)
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Hemp1_WeightB2").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .setOption('title', 'Hempy Bucket 2')
        .setOption('width', 800)
        .setOption('height', 600)
        .setOption('vAxes', { 0: { title: 'kg',
            viewWindow: { min: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StartB2").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() - 1,
                max: logSheet.getRange(logSheet.getLastRow(), logHeader.createTextFinder("Hemp1_StopB2").matchEntireCell(true).findNext().getColumn(), 1, 1).getDisplayValue() + 1
            } } })
        .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 })
        .build();
    chartsSheet.insertChart(chart_HempyBucket2);
    //Generate Fan speed chart
    var chartBuilder = chartsSheet.newChart();
    var chart_FanSpeed = chartBuilder
        .setPosition(120, 1, 0, 0)
        .setChartType(Charts.ChartType.AREA)
        .setNumHeaders(1)
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("IFan_Speed").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .addRange(logSheet.getRange(1, logHeader.createTextFinder("EFan_Speed").matchEntireCell(true).findNext().getColumn(), logSheet.getLastRow(), 1))
        .setOption('title', 'Fan speed')
        .setOption('width', 800)
        .setOption('height', 600)
        .setOption('vAxes', { 0: { viewWindow: { min: 0, max: 2 }, gridlines: { count: 3 } } })
        .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 })
        .build();
    chartsSheet.insertChart(chart_FanSpeed);
    
    */
}
/*
var tempAndHumidityChart = chartsSheet.getCharts()[1];
var tempAndHumidityChart_updated = tempAndHumidityChart
.modify()
.clearRanges()
.addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("InternalTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("ExternalTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("WaterTemp").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("InternalHumidity").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("ExternalHumidity").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.build();
chartsSheet.updateChart(tempAndHumidityChart_updated);

var powerUsageChart = chartsSheet.getCharts()[2];
var powerUsageChart_updated = powerUsageChart
.modify()
.clearRanges()
.addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("Power").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("Energy").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("Light1_Brightness").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.build();
chartsSheet.updateChart(powerUsageChart_updated);

var phAndLightStatusChart = chartsSheet.getCharts()[3];
var phAndLightStatusChart_updated = phAndLightStatusChart
.modify()
.clearRanges()
.addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("PH").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("Light1_Status").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("WaterLevel").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.build();
chartsSheet.updateChart(phAndLightStatusChart_updated);

var pressureAndWaterLevelChart = chartsSheet.getCharts()[4];
var pressureAndWaterLevelChart_updated = pressureAndWaterLevelChart
.modify()
.clearRanges()
.addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("Pressure").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("AeroSprayPressure").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.build();
chartsSheet.updateChart(pressureAndWaterLevelChart_updated);


var fanChart = chartsSheet.getCharts()[0];
var fanChart_updated = fanChart
.modify()
.clearRanges()
.addRange(logSheet.getRange(1,headers.createTextFinder("LogDate").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("InternalFan").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.addRange(logSheet.getRange(1,headers.createTextFinder("ExhaustFan").matchEntireCell(true).findNext().getColumn(),logSheet.getLastRow(),1))
.build();
chartsSheet.updateChart(fanChart_updated);
*/
//}

function ClearCharts(){
  var chartsSheet = SpreadsheetApp.getActive().getSheetByName("Charts");
  var charts = chartsSheet.getCharts();
  for (var i in charts) {
    chartsSheet.removeChart(charts[i]);
  }
}

function UpdateCharts() {
  ClearCharts();  
  var columns = GetNamedRangeValues("Columns");
  var charts = GetNamedRangeValues("Charts").filter(function (row) {
    return typeof row[0] == "number";  //filtering out blank rows based on the second column: Name of the chart)
  });
  LogToConsole("Generating charts...",true,0);
  
  for(var i = 0; i < charts.length;i++){
    if(Debug) LogToConsole("Generating chart " + charts[i][1] + " [" + charts[i][2] + "]",true,3);
            
    chartsSheet = SpreadsheetApp.getActive().getSheetByName("Charts");
    var chartBuilder = chartsSheet.newChart();    
    var columnsToInclude = columns.filter(function (row) {
      return row[6] == charts[i][1];  //filtering out blank rows based on first column (Contains the order number of the chart)
    });
    
    chartBuilder.addRange(GetLogColumnRange("LogDate"));
    var seriesType = {};
    for(var j = 0; j < columnsToInclude.length; j++)
    {   
      chartBuilder.addRange(GetLogColumnRange(columnsToInclude[j][0])); 
      seriesType[j] = { type: columnsToInclude[j][7], targetAxisIndex: j };
    }   
    
    console.log(seriesType);
    chartBuilder.setOption('series', seriesType);
        
    chartBuilder
    .setPosition(1 + i*29, 1, 0, 0)
    .setChartType(GetChartType(charts[i][2]))
    .setNumHeaders(1)
    .setOption('title', charts[i][1])
    .setOption('width', 800)
    .setOption('height', 600)
   // .setOption("useFirstColumnAsDomain", true) //ColAasLabels
    //.setOption("applyAggregateData",0) //AggregateColA
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
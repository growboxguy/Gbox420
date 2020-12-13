function UpdateCharts() {
  GetNamedRangeValues("Columns", true);  ///< Force a refresh of the Columns named range
  UpdateOverviewChart();
  UpdateChartsTab();
}

function ClearCharts(sheet) {
  LogToConsole("Clearing old charts in " + sheet.getName() + "...", true, 0);
  var charts = sheet.getCharts();
  for (var i in charts) {
    if (Debug) LogToConsole("Removing chart " + i, true, 1);
    sheet.removeChart(charts[i]);
  }
}

function UpdateChartsTab() {
  LogToConsole("Generating charts...", true, 0);
  chartsSheet = SpreadsheetApp.getActive().getSheetByName("Charts");
  ClearCharts(chartsSheet);

  var columns = GetNamedRangeValues("Columns");
  var charts = GetNamedRangeValues("Charts").filter(function (row) {
    return typeof row[charts_orderColumn] == "number" && row[charts_orderColumn] > 0;  ///< Get rows with an order number of 1 or above
  });

  for (var i = 0; i < charts.length; i++) {
    if (Debug) LogToConsole("Generating chart " + charts[i][charts_titleColumn] + " [" + charts[i][charts_typeColumn] + "]", true, 3);
    var chartBuilder = chartsSheet.newChart();
    var columnsToInclude = columns.filter(function (row) {
      return row[columns_chartColumn] == charts[i][charts_titleColumn];  ///< Selecting the columns to include in the chart based on Settings tab - Columns section- Show on Chart column
    });

    chartBuilder.addRange(GetLogColumnRange("LogDate", GetSettingsValue("Chart point limit")));
    var seriesType = {};
    for (var j = 0; j < columnsToInclude.length; j++) {
      chartBuilder.addRange(GetLogColumnRange(columnsToInclude[j][columns_keyColumn], GetSettingsValue("Chart point limit")));
      seriesType[j] = { areaOpacity: 0.1, type: columnsToInclude[j][columns_seriesColumn], labelInLegend: columnsToInclude[j][columns_friendlyNameColumn], targetAxisIndex: columnsToInclude[j][columns_targetAxisColumn] };
    }
    if (Debug) LogToConsole(seriesType, true, 3);

    chartBuilder
      .setOption('title', charts[i][1])
      .setOption('series', seriesType)
      .setPosition(1 + i * 29, 1, 0, 0)
      .setChartType(GetChartType(charts[i][2]))

    ApplyStandardFormatting(chartBuilder);
    chartsSheet.insertChart(chartBuilder.build());
  }
}

function UpdateOverviewChart() {
  LogToConsole("Generating overview chart...", true, 0);
  statusSheet = SpreadsheetApp.getActive().getSheetByName("Status");
  ClearCharts(statusSheet);

  var columns = GetNamedRangeValues("Columns");

  if (Debug) LogToConsole("Generating overview chart", true, 3);
  var chartBuilder = statusSheet.newChart();
  var columnsToInclude = columns.filter(function (row) {
    return row[columns_overviewColumn] == true;  ///< Selecting the columns to include in the chart based on Settings tab - Columns section- Show on Overview column
  });

  chartBuilder.addRange(GetLogColumnRange("LogDate", GetSettingsValue("Chart point limit")));
  var seriesType = {};
  for (var j = 0; j < columnsToInclude.length; j++) {
    chartBuilder.addRange(GetLogColumnRange(columnsToInclude[j][columns_keyColumn], GetSettingsValue("Chart point limit")));
    seriesType[j] = { areaOpacity: 0.1, type: columnsToInclude[j][columns_seriesColumn], labelInLegend: columnsToInclude[j][columns_friendlyNameColumn], targetAxisIndex: columnsToInclude[j][columns_targetAxisColumn] };
  }
  if (Debug) LogToConsole(seriesType, true, 3);

  chartBuilder
    .setOption('title', "Overview - " + SpreadsheetApp.getActive().getRangeByName("LastReportTime").getDisplayValue())
    .setOption('series', seriesType)
    .setOption('vAxes.0.logScale', true)
    .setPosition(10, 4, 0, 0)
    .setChartType(Charts.ChartType.COMBO)

  ApplyStandardFormatting(chartBuilder, seriesType);
  statusSheet.insertChart(chartBuilder.build());
}

function ApplyStandardFormatting(chartBuilder, seriesType) {
  chartBuilder
    .setNumHeaders(1)
    .setOption('width', 800)
    .setOption('height', 600)
    //.setOption('vAxis.ticks', [0,1])
    .setOption('selectionMode', 'multiple')
    .setOption('tooltip', { trigger: 'selection' })
    .setOption('aggregationTarget', 'series')
    .setOption('focusTarget', 'category')
    .setOption('hAxis', { viewWindowMode: 'pretty' })
    .setOption('vAxis', { viewWindowMode: 'pretty' })
    //.setOption('vAxis', {maxValue:2000, viewWindow: {max: 2000}} )
    .setOption('legend', { position: 'top', textStyle: { fontSize: 14 } })
    //.setOption('explorer', { keepInBounds: true,maxZoomOut: 20.0,maxZoomIn: 8.0})    
    //.setOption('hAxis', {title: "DateTime"})
    //.setOption('vAxes', {0: {title: "Left side - vertical axis 0"},1: {title: "Right side - vetical axis 1"}})    
    // .setOption("useFirstColumnAsDomain", true) //ColAasLabels
    //.setOption("applyAggregateData",0) //AggregateColA
    // .setOption('series', {
    //   0: { type: 'line', color: 'orange', targetAxisIndex: 0,hasAnnotations: true,dataLabel: 'value',dataLabelPlacement: 'outsideEnd',dataLabel: "value" },
    //    1: { type: 'line', color: 'green', targetAxisIndex: 1,hasAnnotations: true,dataLabel: 'value',dataLabelPlacement: 'outsideEnd',dataLabel: "value" }
    // })
    .setOption('hAxis', { slantedText: true, 'slantedTextAngle': 30 });
}

function GetChartType(name)  ///< Translates the chart type text to an actual chart type
{
  switch (name) {
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
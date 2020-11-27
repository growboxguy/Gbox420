/**
 * Converter
 *
 * Library of spreadsheet range conversion functions to return cell contents as strings, applying the spreadsheet formats defined for the cell(s).
 *
 * <pre>
 * Usage:
 *         var converter = Converter.init(ss.getSpreadsheetTimeZone(),
 *                                        ss.getSpreadsheetLocale());
 *         // Get formats from range
 *         var numberFormats = range.getNumberFormats();
 *         for (row=0;row<data.length;row++) {
 *           for (col=0;col<data[row].length;col++) {
 *             // Get formatted data
 *             var cellText = converter.convertCell(data[row][col],numberFormats[row][col]);
 *             Logger.log(cellText);
 *           }
 *         }    
 * </pre>
 *
 * Copyright (c) 2014, David Bingham, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

var thisInstance_ = this;

/**
 * Utility function to test object class.
 * Note: does not work on Google Apps Script service classes.
 *
 * Example: Logger.log(toClass_.call(someday));  logs [object Date]
 * if (objIsClass_(someday,"Date")) ...
 * 
 * from http://javascript.info/tutorial/type-detection
 */
var toClass_ = {}.toString;
function objIsClass_(object, className) {
  return (toClass_.call(object).indexOf(className) !== -1);
}

/**
 * Initialize Converter to use a specific time zone and locale. This is optional, as
 * the library will use the script Session settings by default. Those defaults may
 * be different than the Spreadsheet settings, which could result in incorrect
 * dates and times.
 *
 * Caveat: localization stubbed for future development.
 *
<pre>
Usage:
&nbsp;  var values = Converter.init(ss.getSpreadsheetTimeZone(),
&nbsp;                              ss.getSpreadsheetLocale())
&nbsp;                        .convertRange(range);
</pre>
 *
 * @param  {String}  tzone  Timezone to use for date/time interpretation
 * @param  {String}  locale Locale to use for default date/time/currency.
 *
 * @return {Converter}      this object, for chaining
 */
function init(tzone, locale) { // Constructor
  thisInstance_.tzone = tzone || thisInstance_.tzone || Session.getScriptTimeZone();
  thisInstance_.locale = locale || thisInstance_.locale || Session.getActiveUserLocale();  // For future localization of numbers, times, dates.
  return thisInstance_;
}


/**
 * Performs conversion of range values according to range formats.
 * 
<pre>
Usage:
&nbsp;  var values = Converter.convertRange(range);
</pre>
 * 
 *
 * @param  {Object}  range     Spreadsheet range to convert
 *
 * @return {Array}             2-D array of Strings. e.g.
<pre>
&nbsp; [["Stock", "Company", "Shares", "Value", "% Port", "% Goal", "C. Price"],
&nbsp;  ["AAPL", "Apple Inc.", "200", "$19596.00", "15.73%", "15.00%", "$97.98"],
&nbsp;  ["AMZN", "Amazon.com, Inc.", "25", "$8340.75", "6.70%", "15.00%", "$333.63"],
&nbsp;  ["Totals", "", "", "$124571.28", "100.00%", "100.00%", ""]]
</pre>
 */
function convertRange(range) {
  // Must have 1 parameter that is a range object
  if (arguments.length !== 1 || Object.keys(range).indexOf("getValues") === -1)
    throw new Error('Invalid parameter(s)');

  // Read range contents
  var data = range.getValues();

  // Get formats from range
  var numberFormats = range.getNumberFormats();

  // Build output array
  var result = [];

  // Populate rows
  for (row = 0; row < data.length; row++) {
    result[row] = [];
    for (col = 0; col < data[row].length; col++) {
      // Get formatted data
      result[row][col] = convertCell(data[row][col], numberFormats[row][col]);
    }
  }

  //debugger;

  return result;
}

function test_convertRange_() {
  var ss = SpreadsheetApp.openById('1ujI8lmwj1SBiNA1Sg99RXYOECEIbDGAFCnR4ISRCjuE');
  var range = ss.getActiveSheet().getDataRange();

  // Get ready to convert data
  var converter = init(ss.getSpreadsheetTimeZone(),
    ss.getSpreadsheetLocale());
  var vals = converter.convertRange(range);
  debugger;
}

/**
 * Return a string containing an HTML table representation
 * of the given range, preserving style settings.
 * 
<pre>
Usage:
&nbsp;  var values = Converter.convertRange2html(range);
</pre>
 * 
 * @param {Range} range Spreadsheet range to render as HTML
 * 
 * @returns {String}    HTML version of range, e.g.
<pre>
&nbsp; &lt;table cellspacing...>
&nbsp;  &lt;colgroup>
&nbsp;   &lt;col width="62">
&nbsp;   &lt;col width="150">
&nbsp;  &lt;/colgroup>
&nbsp;  &lt;tbody>
&nbsp;   &lt;tr style="height: 21px;">
&nbsp;    &lt;td style="...>Stock&lt;/td>
&nbsp;    &lt;td style="...>Company&lt;/td>
&nbsp;   &lt;/tr>
&nbsp;   &lt;tr style=3D"height: 21px;">
&nbsp;    &lt;td style="...>AAPL&lt;/td>
&nbsp;    &lt;td style="...>Apple Inc.&lt;/td>
&nbsp;   &lt;/tr>
&nbsp;  &lt;/tbody>
&nbsp; &lt;/table>
</pre>
 */
function convertRange2html(range) {
  var ss = range.getSheet().getParent();
  var sheet = range.getSheet();
  startRow = range.getRow();
  startCol = range.getColumn();
  lastRow = range.getLastRow();
  lastCol = range.getLastColumn();

  // Get ready to convert data
  var converter = thisInstance_.init();

  // Read table contents
  var data = range.getValues();

  // Get css style attributes from range
  var fontColors = range.getFontColors();
  var backgrounds = range.getBackgrounds();
  var fontFamilies = range.getFontFamilies();
  var fontSizes = range.getFontSizes();
  // getFontLines() ignores strike-through if cell also uses underline
  // https://code.google.com/p/google-apps-script-issues/issues/detail?id=4200
  var fontLines = range.getFontLines();
  var fontStyles = range.getFontStyles();
  var fontWeights = range.getFontWeights();
  var horizontalAlignments = range.getHorizontalAlignments();
  var verticalAlignments = range.getVerticalAlignments();
  var mergedRanges = range.getMergedRanges();

  // https://code.google.com/p/google-apps-script-issues/issues/detail?id=4187
  // Reported widths and heights can be incorrect, with "default" values.
  // If we read GAS defaults (120 wide, 17 high) replace with sheets defaults
  // (100 wide, 21 high). Will be wrong sometimes, but not with defaults!
  // Get column widths in pixels
  var colWidths = [];
  var tableWidth = 0;
  for (var col = startCol; col <= lastCol; col++) {
    colWidths.push(120 == sheet.getColumnWidth(col) ? 100 : sheet.getColumnWidth(col));
    tableWidth += colWidths[colWidths.length - 1];
  }
  // Get Row heights in pixels
  var rowHeights = [];
  for (var row = startRow; row <= lastRow; row++) {
    rowHeights.push(17 == sheet.getRowHeight(row) ? 21 : sheet.getRowHeight(row));
  }

  // Get formats from range
  var numberFormats = range.getNumberFormats();

  // Future consideration...
  //var wraps = range.getWraps();

  // Build HTML Table, with inline styling for each cell
  // Default cell styling appears in table or row, so only minimal overrides need to be given for each cell
  var tableFormat = 'cellspacing="0" cellpadding="0" dir="ltr" border="1" style="width:TABLEWIDTHpx;table-layout:fixed;font-size:9pt;font-family:arial,sans,sans-serif;border-collapse:collapse;border:1px solid #ccc;font-weight:normal;color:black;background-color:white;text-align:right;text-decoration:none;font-style:normal;"';

  var html = ['<table ' + tableFormat + '>'];
  // Column widths appear outside of table rows
  html.push('<colgroup>');
  for (col = 0; col < colWidths.length; col++) {
    html.push('<col width=XXX>'.replace('XXX', colWidths[col]))
  }
  html.push('</colgroup>');
  html.push('<tbody>');

  // Populate rows
  for (row = 0; row < data.length; row++) {
    html.push('<tr style="height:XXXpx;vertical-align:bottom;">'.replace('XXX', rowHeights[row]));
    for (col = 0; col < data[row].length; col++) {
      // Get formatted data
      var cellText = converter.convertCell(data[row][col], numberFormats[row][col], true);

      var style = 'style="'
        + 'padding:1px 2px; '
        + 'color:XXX;'.replace('XXX', fontColors[row][col].replace('general-', '')).replace('color:black;', '')
        + 'font-family:XXX;'.replace('XXX', fontFamilies[row][col]).replace('font-family:arial,sans,sans-serif;', '')
        + 'font-size:XXXpt;'.replace('XXX', fontSizes[row][col]).replace('font-size:10pt;', '')
        + 'font-weight:XXX;'.replace('XXX', fontWeights[row][col]).replace('font-weight:normal;', '')
        + 'background-color:XXX;'.replace('XXX', backgrounds[row][col]).replace('background-color:white;', '')
        + 'text-align:XXX;'.replace('XXX', horizontalAlignments[row][col]
          .replace('general-', '')
          .replace('general', 'center')).replace('text-align:right;', '')
        + 'vertical-align:XXX;'.replace('XXX', verticalAlignments[row][col]).replace('vertical-align:bottom;', '')
        + 'text-decoration:XXX;'.replace('XXX', fontLines[row][col]).replace('text-decoration:none;', '')
        + 'font-style:XXX;'.replace('XXX', fontStyles[row][col]).replace('font-style:normal;', '')
        + 'border:1px solid black;'  // Need this, to override caja-guest td border-bottom
        + 'overflow:hidden;'
        + '"';

      var thisRow = range.getRow() + row;
      var thisCol = range.getColumn() + col;
      var rcSpan = "";
      var isTdPartOfRange = false;

      for (var i = 0; i < mergedRanges.length; i++) {
        var currentMergedRange = mergedRanges[i];

        var currentMergedRangeBoundaries = {
          top: currentMergedRange.getRow(),
          bottom: currentMergedRange.getRow() + currentMergedRange.getNumRows() - 1,
          left: currentMergedRange.getColumn(),
          right: currentMergedRange.getColumn() + currentMergedRange.getNumColumns() - 1
        };

        if ((thisRow == currentMergedRangeBoundaries.top && thisCol == currentMergedRangeBoundaries.left)) {
          // top left cell of range
          if (currentMergedRange.getNumRows() > 0) {
            rcSpan = " rowspan='" + currentMergedRange.getNumRows() + "'";
          }
          if (currentMergedRange.getNumColumns() > 0) {
            rcSpan += " colspan='" + currentMergedRange.getNumColumns() + "'";
          }
        } else if ((thisRow >= currentMergedRangeBoundaries.top && thisRow <= currentMergedRangeBoundaries.bottom) && (thisCol >= currentMergedRangeBoundaries.left && thisCol <= currentMergedRangeBoundaries.right)) {
          // falls in range
          isTdPartOfRange = true;
          break;
        }
      }
      if (!isTdPartOfRange) {
        html.push('<td XXX SPAN>'.replace('SPAN', rcSpan).replace('XXX', style)
          + String(cellText)
          + '</td>');
      }
    }
    html.push('</tr>');
  }
  html.push('</tbody>');
  html.push('</table>');

  //debugger;
  //return '<!--StartFragment--><meta name="generator" content="Sheets"><style type="text/css"><!--td {border: 1px solid #ccc;}br {mso-data-placement:same-cell;}--></style><table cellspacing="0" cellpadding="0" dir="ltr" border="1" style="table-layout:fixed;font-size:13px;font-family:arial,sans,sans-serif;border-collapse:collapse;border:1px solid #ccc"><colgroup><col width="100"><col width="155"><col width="100"></colgroup><tbody><tr style="height:21px;"><td style="padding:2px 3px 2px 3px;vertical-align:bottom;background-color:#ffff00;border-top:1px solid #000000;border-right:1px solid #000000;border-bottom:1px solid #000000;border-left:1px solid #000000;" data-sheets-value="[null,2,&quot;100px wide&quot;]">100px wide</td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;background-color:#000000;border-top:1px solid #000000;border-right:1px solid #000000;border-bottom:1px solid #000000;font-family:georgia;font-size:140%;font-style:italic;color:#00ffff;" data-sheets-value="[null,2,&quot;Blue/Black&quot;]">Blue/Black</td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-top:1px solid #000000;border-right:1px solid #000000;border-bottom:1px solid #000000;text-align:right;" data-sheets-value="[null,3,null,2]">2</td></tr><tr style="height:21px;"><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;border-left:1px solid #000000;text-align:right;" data-sheets-value="[null,3,null,2.3]">2.3</td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;font-family:courier new,monospace;text-decoration:underline line-through;" data-sheets-value="[null,2,&quot;155 px wide&quot;]">155 px wide</td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;text-align:right;" data-sheets-value="[null,3,null,41839]" data-sheets-numberformat="[null,5]" data-sheets-formula="=today()">7/19/2014</td></tr><tr style="height:30px;"><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;border-left:1px solid #000000;font-weight:bold;color:#ff0000;text-align:center;" data-sheets-value="[null,3,null,41822]" data-sheets-numberformat="[null,5,&quot;M/d/yyyy&quot;,1]">7/2/2014</td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;"></td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;font-family:georgia;font-weight:bold;" data-sheets-value="[null,2,&quot;sadf&quot;]">sadf</td></tr><tr style="height:50px;"><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;border-left:1px solid #000000;text-decoration:underline line-through;vertical-align:top;text-align:right;" data-sheets-value="[null,2,&quot;asr&quot;]">asr</td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;font-size:360%;vertical-align:bottom;" data-sheets-value="[null,2,&quot;asdf&quot;]">asdf</td><td style="padding:2px 3px 2px 3px;vertical-align:bottom;border-right:1px solid #000000;border-bottom:1px solid #000000;vertical-align:middle;text-align:center;" data-sheets-value="[null,2,&quot;100 px wide&quot;]">100 px wide</td></tr></tbody></table><!--EndFragment-->'

  return html.join('').replace('TABLEWIDTH', tableWidth);
}


/**
 * Performs conversion of cell contents according to their spreadsheet format.
 *
<pre>
Usage:
&nbsp;  Logger.log( Converter.convertCell(1234.56,"0.00E+00") );
&nbsp;  ... logs "1.23E+3"
</pre>
 *
 * @param  {Object}  cellText  Contents of a cell
 * @param  {String}  format    Spreadsheet format to use in conversion
 * @param  {Boolean} htmlReady (optional) Set true if strings should be html-friendly.
 *                             Default is false, output is plain text.
 *
 * @return {String}            Formatted string. May contain HTML, depending on
 *                             htmlReady.
 */
function convertCell(cellText, format, htmlReady) {
  // Must have 2 or 3 parameters, format must be string
  if (arguments.length < 2 || !objIsClass_(format, "String"))
    throw new Error('Invalid parameter(s)');
  htmlReady = htmlReady || false;
  thisInstance_.init(); // Ensure instance variables are set

  if (cellText === null) return '';  // Not much to do with blank cells - just return an empty string

  // Treat all dates & times the same; we can adapt the spreadsheet formats
  if (objIsClass_(cellText, "Date")) {
    return (convertDateTime_(cellText, format));
  }

  // Numbers come in many flavours; which do we have?
  if (objIsClass_(cellText, "Number")) {
    // General - Not so much a format, more of a guideline.
    if (format === "0.###############" || format === '') {
      if (Math.abs(cellText) >= 1000000000000010)
        return convertExponential_(cellText, 5);  // Overflow - automatic exponential, 5 fraction digits
      else
        return String(cellText);
    }
    // Padded decimal numbers
    // Interpret Default number format as a padded decimal
    if (format === '@') format = '0.###############';
    // Padded decimal numbers; zero or more of # and/or 0 and optional ',' 
    // separators, optionally followed by a radix '.' and one or more of # and/or 0.
    // Also need to allow for misplaced ',' in fraction.
    var re = /^([#0,]+)([\.]?)([#0,]*)$/
    var paddedDecimal = re.test(format);
    if (paddedDecimal) {
      var thous = format.match(/,/) ? ',' : '';      // Check for thousand separators, remember
      format = format.replace(/,/g, '');              // and remove them
      var parts = format.match(re);  // Parts[1] is integer part, parts[2] is radix (null if none), parts[3] is fraction
      var whole = parts[1];
      var wholeMin = whole.replace(/[^0]/g, '').length;  // minimum digits in whole part expressed by count of zeros
      var wholeMax = whole.length;                      // max digits in whole is length of zeros & #
      var fract = parts[3];
      var fractMin = fract.replace(/[^0]/g, '').length;  // min digits in frac expressed by count of zeros
      var fractMax = fract.length;                      // max digits in frac is length of zeros & #
      return convertPadded_(cellText, fractMax, fractMin, wholeMin, thous);
    }
    // Currency
    if (format.indexOf('$') !== -1) {
      var options = { htmlReady: htmlReady };
      // find out position of currency symbol
      if (format.slice(-1) === "]") options.symLoc = "after";
      // and what the symbol is - the default $ will be handled by the converter,
      // here we are looking for the symbol mapper, e.g. [$€], and isolating the
      // target currency symbol, e.g. €, along with any included text or punctuation.
      var matches = format.match(/\[\$(.*?)\]/);
      if (matches) options.symbol = matches[1];
      // find the fraction precision
      matches = format.match(/\.(0*?)($|[^0])/);
      var fract = matches ? matches[1].length : 0;
      // are brackets in use?
      matches = format.match(/\(.*\)/);
      if (matches) options.negBrackets = true;
      /* how about coloring negatives?
      *  Color# (where # is replaced by a number between 1-56 to choose from a different variety of colors)
      *  ... this is problematic. Should find out what the RGB of those 56 colors are,
      *  and map them. In the mean time, can result in browser console messages.
      */
      matches = format.match(/;\[(.*?)\]/);
      if (matches) options.negColor = matches[1];
      // Then call the currency converter
      return convertCurrency_(cellText, fract, options);
    }
    // Percent
    if (format.indexOf('%') !== -1) {
      var matches = format.match(/\.(0*?)%/);
      var fract = matches ? matches[1].length : 0;     // Fractional part
      return convertPercent_(cellText, fract);
    }
    // Exponentials
    var expon = format.match(/\.(0*?)E\+/);
    if (expon) {
      //var fract = format.match(/\.(0*?)E\+/)[1].length;  // Fractional part
      var fract = expon[1].length;  // Fractional part
      return convertExponential_(cellText, fract);
    }
    // Fraction
    if (format.indexOf('?\/?') !== -1) {
      matches = format.match(/(\?*?)\//);
      var precision = matches ? matches[1].length : 1;     // Fractional part
      return convertFraction_(cellText, precision);
    }
    if (this[format]) {                                    // TODO: kill off, then stop calling stand-alone converters
      return converter_[format](cellText);
    }
    else {
      Logger.log("Unsupported format '" + format + "', cell='" + cellText + "'");
      return cellText;
    }
  }
  // No previous condition met, cell contains a string.
  var result = String(cellText);
  // Sanitize string if output is for html
  if (htmlReady) result = result.replace(/ /g, "&nbsp;").replace(/</g, "&lt;").replace(/\n/g, "<br>");
  return result;
}

function convertDateTime_(date, format) {
  // The 'general' format for dates is blank
  if ('' == format) format = 'M/d/yyyy'; // TODO: Should be getSpreadsheetLocale() based
  // Translate spreadsheet date format elements to SimpleDateFormat
  // http://docs.oracle.com/javase/7/docs/api/java/text/SimpleDateFormat.html
  // From the documentation for the "TEXT" spreadsheet function...
  // "mm for the month of the year as two digits or the number
  // of minutes in a time. Month will be used unless this code
  // is provided with hours or seconds as part of a time." 
  if (format.match(/am\/pm/i) == null) {
    format = format.replace(/h/g, 'H');   // Hour of day, 0-23
  }

  // Check for elapsed time  
  if (format.indexOf("[") !== -1)
    format = updFormatElapsedTime_(date, format);

  var jsFormat = format
    .replace(/am\/pm|AM\/PM/, 'a') // Am/pm marker
    .replace('dddd', 'EEEE')       // Day name in week (long)
    .replace('ddd', 'EEE')         // Day name in week (short)
    .replace(/S/g, 's')            // In Sheets, upper & lower s means Seconds
    .replace(/D/g, 'd')            // In Sheets, upper & lower d means Day
    .replace(/M/g, 'm')            // In Sheets, upper & lower m are the same, what matters is quantity & neighbors
    .replace(/([hH]+)"*(.)"*(m+)/g, tempMinute_) // ... so find "minutes" around "hours"
    .replace(/(m+)"*(.)"*(s+)/g, tempMinute_)    // ... or "seconds", and change to 'b' temporarily
    .replace('mmmmm', '"@"MMM"@"') // first letter in month - google-ism?
    .replace(/m/g, "M")            // All remaining "m"s are months, so M for SimpleDateFormat
    .replace(/b/g, 'm')            // reassert temporary minutes
    .replace(/0+/, 'S')            // Milliseconds are 0 in Sheets, upper S in SimpleDateFormat
    .replace(/"/g, "'")            // Change double to single quotes on filler
  var result = Utilities.formatDate(
    date,
    thisInstance_.tzone,
    jsFormat)
    .replace(/@.*@/g, firstChOfMonth_);   // Tidy first char in month, in post-processing
  return result;
}

/**
 * Replace all occurences of m with b. To be used as a function parameter
 * for String.replace(). Helper for convertDateTime_.
 *
 * @param {string} match  Regex match containing 'm's and other stuff
 * @returns {string}      replacement for match.
 */
function tempMinute_(match) {
  return match.replace(/m/g, 'b');
}

/**
 * Return first character of month. Helper for convertDateTime_.
 *
 * @param {string} match  Regex match, formatted @Month@
 * @returns {string}      replacement for match.
 */
function firstChOfMonth_(match) {
  return match.charAt(1)
}

/**
 * Replace elapsed-time signatures in the given format with SimpleDateFormat
 * compatible versions. SimpleDateFormat understands elapsed Hours (H), but
 * not minutes or seconds, so we replace elapsed minutes & seconds with
 * calculated values.
 */
function updFormatElapsedTime_(date, format) {
  // For elapsed time, we are interested in the time since midnight.
  var elapsedMs = getMsSinceMidnight_(date);

  // Generate elapsed seconds & minutes, just in case. While we could optimize these
  // operations to be performed only when needed, the tests are as expensive.
  // Check for elapsed second signature, determine its length for padding.
  var matches = format.match(/\[([sS]+)\]/);
  var pad = matches ? matches[1].length : 1;
  var elapsedSec = convertPadded_(Math.floor(elapsedMs / 1000), 0, 0, pad);
  // Check for elapsed minute signature, determine its length for padding.
  matches = format.match(/\[([mM]+)\]/);
  pad = matches ? matches[1].length : 1;
  var elapsedMin = convertPadded_(Math.floor(elapsedMs / 60000), 0, 0, pad);
  //var matches = format.match(/\[([^\]]+)\]/g); // Regex finds all elapsed time notations
  var format = format.replace(/\[([hH]+)\]/, elapsedHours_)
    .replace(/\[([mM]+)\]/, elapsedMin)
    .replace(/\[([sS]+)\]/, elapsedSec)
  return format;
}

/**
 * Replace all occurences of the elapsed hours pattern (e.g. "[h]")
 * with 'H', and remove braces. To be used as a function parameter
 * for String.replace(). Helper for convertDateTime_.
 *
 * @param {string} match  Regex match containing '[h]'s and other stuff
 * @returns {string}      replacement for match.
 */
function elapsedHours_(match) {
  return match.replace(/[hH]/g, 'H').replace(/[\[\]]/g, '');
}

/**
 * Calculate elapsed time since midnight. Helper for updFormatElapsedTime_.
 * From http://stackoverflow.com/a/10946213/1677912.
 *
 * @param {Date} d        time to test
 * @returns {Number}      milliseconds elapsed since midnight
 */
function getMsSinceMidnight_(d) {
  var e = new Date(d);
  return d - e.setHours(0, 0, 0, 0);
}

/**
 * Return string representation of a padded decimal number.
 *
 * @param {number}   num        Number to be converted
 * @param {number}   fractMax   Max digits in fraction (round)
 * @param {number}   fractMin   Min digits in fraction (pad)
 * @param {number}   wholeMin   Min digits in whole, default 1 (pad)
 * @param {char}     thous      Thousand separator char, blank default
 */
function convertPadded_(num, fractMax, fractMin, wholeMin, thous) {
  fractMin = fractMin || 0; // Set defaults for optional parameters
  wholeMin = wholeMin || 1;
  thous = thous || '';
  var numStr = String(1 * Utilities.formatString("%.Xf".replace('X', String(fractMax)), num));
  var parts = numStr.split('.');
  var whole = pad0_(parts[0], wholeMin, true);
  var frac = pad0_((parts.length > 1) ? parts[1] : '', fractMin);
  var thouGroups = /(\d+)(\d{3})/;
  while (thous && thouGroups.test(whole)) {
    whole = whole.replace(thouGroups, '$1' + thous + '$2');
  }
  var result = whole + (frac ? ('.' + frac) : '');
  return result;
}

/**
 * Pad an integer with leading or trailing zeros.
 *
 * @param {number}  num      Number to pad
 * @param {number}  width    Final width of padded number
 * @param {Boolean} leading  (optional) true for leading zeros,
 *                           default is false for trailing zeros
 */
function pad0_(num, width, left) {
  var num = String(num);
  // Check whether input is already wide enough
  if (num.length >= width) return num;
  var bunchazeros = '0000000000000000000000000000000000000';
  if (left) {
    var result = (bunchazeros + num).substr(-width);
  } else {
    result = (num + bunchazeros).substr(0, width);
  }
  return result;
}

function convertExponential_(num, fract) { return num.toExponential(fract).replace('e', 'E'); }

function convertPercent_(num, fract) { return Utilities.formatString("%.Xf%".replace('X', String(fract)), 100 * num); }


/**
 *  Options: an optional object with optional properties...
 *    symbol {string} default '$'
 *    symLoc {'before','after','none'} default before
 *    negBrackets {boolean} default false
 *    negColor {string} color for negative numbers
 */
function convertCurrency_(num, fract, options) {
  options = options || {};
  var result = "#RESULT#";
  var symbol = options.symbol ? options.symbol : '$';
  if (!options.symLoc || options.symLoc === 'before') {
    result = symbol + "#RESULT#";
  }
  else if (options.symLoc === 'after') {
    result = "#RESULT#" + symbol;
  }
  else {
    // no symbol
  }
  if (num < 0) {
    num = -num;
    if (options.negBrackets) {
      result = "(" + result + ")";
    }
    else {
      result = '-' + result;
    }
  }
  if (options.negColor && options.htmlReady) {
    result = ("<span style=\"color:XXX;\">" + result + "</span>").replace("XXX", options.negColor.toLowerCase());
  }
  num = convertPadded_(num, fract);
  return result.replace("#RESULT#", num);
}

//  "# ?/?", "# ??/??"
function convertFraction_(num, precision) {
  if (!thisInstance_.fracEst) thisInstance_.fracEst = new FractionEstimator_();
  var sign = (num < 0) ? -1 : 1;
  num = sign * num;
  var whole = Math.floor(num);
  //  var whole = String(num).match(/(.*?)\./)[1]+' ';
  var frac = num % 1; // introduces small rounding errors
  var result = ((whole === 0) ? '' : String(sign * whole) + ' ') + thisInstance_.fracEst.estimate(frac, precision);
  return result
}

/********************************************************************************************/

// Stand-alone converters, left-overs from a by-gone era. TODO: eliminate these
var converter_ = {};

// TODO: this is just so similar to currency, some refactoring would take care of it.
// should break out negBracket & color identification to be general
converter_["#,##0.00;(#,##0.00)"] = function (num) {
  if (num > 0) {
    var result = 'XXX';
  }
  else {
    num = -num;
    result = '(XXX)';
  }
  return result.replace('XXX', Utilities.formatString("%.2f", num));
}


/********************************************************************************************/

/**
 * A fraction estimator to provide fraction strings that are a close 
 * representations of given decimal values.
 *
 * To restrict outcomes to "friendly fractions" (i.e. with easily-
 * read denominators), estimates are found by identifying them
 * in a list.
 *
 * Caveat: Construction of a new estimator for fractions with 2 or more significant digit
 * denominators is S.L.O.W. Subsequent estimates are very quick, O(Ln(N)).
 *
 * <pre>
 * Usage:
 *         var fracEst = new FractionEstimator_();
 *         var value = 0.56;
 *         var precision = 2; // 2 digit denominator
 *         var frac = fracEst.estimate(value,precision);  // "14/25"
 * </pre>
 */
function FractionEstimator_() {   // constructor
  this.fracList = {};  // Object holds lists of acceptable fractions
}


/**
 * Get a string containing a fraction estimate for the given
 * value, with indicated precision of denominator.
 *
 * @param {number} value     Number to be estimated, 0 < value < 1
 * @param {number} precision # digits in denominator, 1 (default) or 2
 *
 * @return {String}          e.g. "14/25"
 */
FractionEstimator_.prototype.estimate = function (value, precision) {
  if (1 <= value || 0 > value) throw new Error('invalid fraction, 0 < fraction < 1');
  precision = precision || 1;
  if (precision > 2) throw new Error('beyond max precision');

  var list = this.fracList_(precision);  // Get a handle on list of acceptable fractions

  // Use bisection to find first value equal to or larger than value
  var lo = 0, hi = list.length - 1;
  while (lo < hi) {
    var mid = (lo + hi) >> 1;
    if (value < list[mid].val) hi = mid;
    else lo = mid + 1;
  }
  // pick the closer of the found 'lo', and the element before that
  if (Math.abs(list[lo - 1].val - value) < Math.abs(list[lo].val - value))
    var frac = list[lo - 1].frac;
  else
    frac = list[lo].frac;
  return frac;
}

// Return acceptable fraction list for given precision, build if needed.
FractionEstimator_.prototype.fracList_ = function (precision) {
  if (!this.fracList[precision]) {
    var max = Math.pow(10, precision);
    var list = [];
    for (var denom = 2; denom < max; denom++) {
      for (var nom = 1; nom < denom; nom++) {
        var dec = nom / denom;
        if (!list[dec]) list[dec] = Utilities.formatString("%u/%u", nom, denom);
        if (!(denom % 2)) nom++; // skip even/even, since they would reduce
        // Same case for any factors... but not worth the cycles to calculate
      }
    }
    var a = Object.keys(list).sort();
    this.fracList[precision] = [];
    for (var i = 0; i < a.length; i++) {
      this.fracList[precision].push({ "val": parseFloat(a[i]), "frac": list[a[i]] });
    }
  }
  return this.fracList[precision];
}


function test_Frac_() {
  var fracEst = new FractionEstimator_();
  var value = 0.56;
  var precision = 2; // 1 digit denominator
  var frac = fracEst.estimate(value, precision);
  debugger;
}
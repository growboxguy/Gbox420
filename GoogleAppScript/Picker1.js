//Custom dialog
function prompt(){
  var ui = SpreadsheetApp.getUi();
  var html = HtmlService
  .createTemplateFromFile("index")
  .evaluate()
  .setWidth(270)
  .setHeight(250);
  ui.showModalDialog(html, "Column color");
  
};


// Creates an import or include function so files can be added 
// inside the main index.
function include(filename){
  return HtmlService.createHtmlOutputFromFile(filename)
    .getContent();
};

//Receives data from prompt or sidebar and updates the first cell.
function receiveData(data){
  if(data[0].indexOf("Select") !== -1){
    SpreadsheetApp.getUi().alert("You need to select a color!");
    return;
  };
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var sheet = ss.getSheetByName("test");
  var cell = sheet.getRange("A1");
  cell.setValue(data[1])
      .setBackground(data[0])
      .setFontColor(data[2])
      .setFontSize(16);
};
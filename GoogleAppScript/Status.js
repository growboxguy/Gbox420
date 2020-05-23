/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Updates the Status sheet

function UpdateStatus(Log)
{ 
  console.log("Updating Status sheet...");
  var Status = GetNamedRange("Status");
  
  var newStatus = []; 
  for (var i = 0; i < Status.length; i++) {   
    newStatus.push([Status[i][0],null]);  //pre-fill with null values
  } 

  LogToConsole("Blank status: " + newStatus, true, 1);    
  var nextRow = getLastRowInRange(newStatus);
    
  //Add all Key + Data pairs to the Status sheet from the received Log JSON  
  var Components = Object.getOwnPropertyNames(Log);  
  for (var i = 0; i < Components.length; i++) {
    var Properties = Object.getOwnPropertyNames(Log[Components[i]]);    
    for (var j = 0; j < Properties.length; j++) {
      var key = Components[i] + '_' + Properties[j]; 
     
      var match = -1;      
      for (var k = 0; k < newStatus.length; k++) {
        LogToConsole(newStatus[k][0] + " vs " + key,false,2); 
        if(newStatus[k][0] == key){
        match = k;  //Trying to find Status entry with the relayed component name
        break;
        }
      }
      LogToConsole("",true,0);
      LogToConsole(key + " log column matched: " + match,true,3);
      if(match != -1)
      {
        try{
          newStatus[match][1] = GetFriendlyValue(key,Log[Components[i]][Properties[j]]);   //Update matched row's second column
        }
        catch(e){
          LogToConsole(e,true,1);
        }
      }
      else{
        try{
          newStatus.push([key,GetFriendlyValue(key,Log[Components[i]][Properties[j]])]); //Create a new row   
          nextRow++;
        }
        catch(e){
          LogToConsole(e,true,1);
        }
      }
    }
  }
  LogToConsole("Writing latest log row...",false,2);
  LogToConsole(newStatus,true,1);
  SpreadsheetApp.getActive().get.getRange(2,1,newStatus.length,2).setValues(newStatus);
  //SpreadsheetApp.getActive().setNamedRange("statusKeys", 'Status!A2:B');

}
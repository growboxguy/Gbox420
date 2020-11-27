#pragma once

#include "420Common.h"
#include "420Module.h"

class PHSensor : virtual public Common
{
public:
  PHSensor(const __FlashStringHelper *Name, Module *Parent, Settings::PHSensorSettings *DefaultSettings);
  void refresh_FiveSec();
  void report();
  float getPH(bool ReturnAverage = true);
  char *getPHText(bool ReturnAverage = true);

private:
protected:
  Module *Parent;
  uint8_t *Pin;
  RollingAverage *PH;
  float *Intercept;
  float *Slope;
  void updatePH(bool ShowRaw);
  void setSlope(float Value);
  void setIntercept(float Value);
};

///WEBSITE COMPONENT
/*
<div class="card" style="width:90%">
  <h1>PHSen1</h1>
  <table class="tg">
    <tr>
      <th class="headerRows"><span style="font-weight:bold">PH</span></th>			
    </tr>
    <tr>
      <td id="PHSensor1_PH" class="dataRows"></td>
    </tr>
  </table>	
</div>

<div class="card" style="width:90%">
  <h1>Calibration</h1>				
  <b>PH sensor </b><button id="PHSensor1_ReadRaw" type="button" style="margin-top:0px" title="Displays the analog PH value for a calibration point">Read raw</button>
  <form>
    Slope:<input style="width: 80px;" name="PHSensor1_Slope" type="text"/> 
    Intercept:<input style="width: 80px;" name="PHSensor1_Intercept" type="text"/>
    <input type="submit" value="Set">										
  </form>  			
</div>

*/
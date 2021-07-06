echo off
echo Removing hard links from: %CD%

del .\SerialLog.cpp > nul 2>&1
del .\SerialLog.h > nul 2>&1
del .\Settings.cpp > nul 2>&1

del .\Webpages\Settings.html > nul 2>&1

del .\src\Modules\HempyModule.h > nul 2>&1 
del .\src\Modules\HempyModule.cpp > nul 2>&1
del .\src\Modules\AeroModule_NoTank.h > nul 2>&1 
del .\src\Modules\AeroModule_NoTank.cpp > nul 2>&1
del .\src\Modules\AeroModule_Tank.h > nul 2>&1 
del .\src\Modules\AeroModule_Tank.cpp > nul 2>&1
del .\src\Modules\ReservoirModule.h > nul 2>&1 
del .\src\Modules\ReservoirModule.cpp > nul 2>&1

del .\src\Modules_Web\420Common_Web.h > nul 2>&1
del .\src\Modules_Web\420Common_Web.cpp > nul 2>&1
del .\src\Modules_Web\420Module_Web.h > nul 2>&1
del .\src\Modules_Web\420Module_Web.cpp > nul 2>&1
del .\src\Modules_Web\HempyModule_Web.h > nul 2>&1 
del .\src\Modules_Web\HempyModule_Web.cpp > nul 2>&1
del .\src\Modules_Web\AeroModule_Web.h > nul 2>&1 
del .\src\Modules_Web\AeroModule_Web.cpp > nul 2>&1
del .\src\Modules_Web\ReservoirModule_Web.h > nul 2>&1 
del .\src\Modules_Web\ReservoirModule_Web.cpp > nul 2>&1
del .\src\Modules_Web\MainModule_Web.h > nul 2>&1 
del .\src\Modules_Web\MainModule_Web.cpp > nul 2>&1
del .\src\Modules_Web\DevModule_Web.h > nul 2>&1 
del .\src\Modules_Web\DevModule_Web.cpp > nul 2>&1
del .\src\Modules_Web\DEVModule_Web.h > nul 2>&1 
del .\src\Modules_Web\DEVModule_Web.cpp > nul 2>&1
del .\src\Modules_Web\Hempy_Standalone.h > nul 2>&1 
del .\src\Modules_Web\Hempy_Standalone.cpp > nul 2>&1
del .\src\Modules_Web\Hempy_Standalone.h > nul 2>&1 
del .\src\Modules_Web\Hempy_Standalone.cpp > nul 2>&1

del .\src\Helpers.cpp > nul 2>&1
del .\src\Helpers.h > nul 2>&1
del .\src\WirelessCommands_Aero.h > nul 2>&1
del .\src\WirelessCommands_Dev.h > nul 2>&1
del .\src\WirelessCommands_Hempy.h > nul 2>&1
del .\src\WirelessCommands_Reservoir.h > nul 2>&1

del .\src\Components_Web\420Common_Web.h > nul 2>&1
del .\src\Components_Web\DHTSensor_Web.h > nul 2>&1
del .\src\Components_Web\DHTSensor_Web.cpp > nul 2>&1
del .\src\Components_Web\HempyBucket_Web.h > nul 2>&1
del .\src\Components_Web\HempyBucket_Web.cpp > nul 2>&1
del .\src\Components_Web\Sound_Web.h > nul 2>&1
del .\src\Components_Web\Sound_Web.cpp > nul 2>&1
del .\src\Components_Web\WaterPump_Web.h > nul 2>&1
del .\src\Components_Web\WaterPump_Web.cpp > nul 2>&1
del .\src\Components_Web\WasteReservoir_Web.h > nul 2>&1
del .\src\Components_Web\WasteReservoir_Web.cpp > nul 2>&1
del .\src\Components_Web\WeightSensor_Web.h > nul 2>&1
del .\src\Components_Web\WeightSensor_Web.cpp > nul 2>&1
del .\src\Components_Web\AirPump_Web.h > nul 2>&1
del .\src\Components_Web\AirPump_Web.cpp > nul 2>&1
del .\src\Components_Web\Fan_Web.h > nul 2>&1
del .\src\Components_Web\Fan_Web.cpp > nul 2>&1
del .\src\Components_Web\LightSensor_Web.h > nul 2>&1
del .\src\Components_Web\LightSensor_Web.cpp > nul 2>&1
del .\src\Components_Web\Lights_Web.h > nul 2>&1
del .\src\Components_Web\Lights_Web.cpp > nul 2>&1
del .\src\Components_Web\PowerSensorV3_Web.h > nul 2>&1
del .\src\Components_Web\PowerSensorV3_Web.cpp > nul 2>&1

del .\src\Components\420Common.h > nul 2>&1
del .\src\Components\420Common.cpp > nul 2>&1
del .\src\Components\420Module.h > nul 2>&1
del .\src\Components\420Module.cpp > nul 2>&1
del .\src\Components\Aeroponics.h > nul 2>&1
del .\src\Components\Aeroponics.cpp > nul 2>&1
del .\src\Components\Aeroponics_NoTank.h > nul 2>&1
del .\src\Components\Aeroponics_NoTank.cpp > nul 2>&1
del .\src\Components\Aeroponics_Tank.h > nul 2>&1
del .\src\Components\Aeroponics_Tank.cpp > nul 2>&1
del .\src\Components\AirPump.h > nul 2>&1
del .\src\Components\AirPump.cpp > nul 2>&1
del .\src\Components\DHTSensor.h > nul 2>&1
del .\src\Components\DHTSensor.cpp > nul 2>&1
del .\src\Components\DistanceSensor.h > nul 2>&1
del .\src\Components\DistanceSensor.cpp > nul 2>&1
del .\src\Components\Fan.h > nul 2>&1
del .\src\Components\Fan.cpp > nul 2>&1
del .\src\Components\Fan_PWM.h > nul 2>&1
del .\src\Components\Fan_PWM.cpp > nul 2>&1
del .\src\Components\HempyBucket.h > nul 2>&1
del .\src\Components\HempyBucket.cpp > nul 2>&1
del .\src\Components\Lights.h > nul 2>&1
del .\src\Components\Lights.cpp > nul 2>&1
del .\src\Components\LightSensor.h > nul 2>&1
del .\src\Components\LightSensor.cpp > nul 2>&1
del .\src\Components\PressurePump.h > nul 2>&1
del .\src\Components\PressurePump.cpp > nul 2>&1
del .\src\Components\PressureSensor.h > nul 2>&1
del .\src\Components\PressureSensor.cpp > nul 2>&1
del .\src\Components\PHSensor.h > nul 2>&1
del .\src\Components\PHSensor.cpp > nul 2>&1
del .\src\Components\PowerSensor.h > nul 2>&1
del .\src\Components\PowerSensor.cpp > nul 2>&1
del .\src\Components\PowerSensorV3.h > nul 2>&1
del .\src\Components\PowerSensorV3.cpp > nul 2>&1
del .\src\Components\PressureSensor.h > nul 2>&1
del .\src\Components\PressureSensor.cpp > nul 2>&1
del .\src\Components\Sound.h > nul 2>&1
del .\src\Components\Sound.cpp > nul 2>&1
del .\src\Components\Switch.h > nul 2>&1
del .\src\Components\Switch.cpp > nul 2>&1
del .\src\Components\Switch_PWM.h > nul 2>&1
del .\src\Components\Switch_PWM.cpp > nul 2>&1
del .\src\Components\TDSSensor.h > nul 2>&1
del .\src\Components\TDSSensor.cpp > nul 2>&1
del .\src\Components\WaterLevelSensor.h > nul 2>&1
del .\src\Components\WaterLevelSensor.cpp > nul 2>&1
del .\src\Components\WaterPump.h > nul 2>&1
del .\src\Components\WaterPump.cpp > nul 2>&1
del .\src\Components\WasteReservoir.h > nul 2>&1
del .\src\Components\WasteReservoir.cpp > nul 2>&1
del .\src\Components\WaterTempSensor.h > nul 2>&1
del .\src\Components\WaterTempSensor.cpp > nul 2>&1
del .\src\Components\WeightSensor.h > nul 2>&1
del .\src\Components\WeightSensor.cpp > nul 2>&1
del .\HardLinkSRC_OFF.sh > nul 2>&1

echo Cleaning up empty folders...
for /f "delims=" %%d in ('dir /s /b /ad ^| sort /r') do rd "%%d" > nul 2>&1
echo Done

(goto) 2>nul & del "%~f0"
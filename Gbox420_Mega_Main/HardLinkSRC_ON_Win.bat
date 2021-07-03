echo off
echo "------------------------------------------"
echo "Processing %CD%"
echo "------------------------------------------"

echo "Creating unlink command"
mklink /h .\HardLinkSRC_OFF.sh ..\HardLinkSRC_OFF.sh
mklink /h .\HardLinkSRC_OFF_Win.bat ..\HardLinkSRC_OFF_Win.bat

echo "Creating folders"
mkdir .\src
mkdir .\Webpages
mkdir .\src\Components
mkdir .\src\Components_Web
mkdir .\src\Modules_Web

echo "Creating hard links"
mklink /h .\SerialLog.cpp ..\src\SerialLog_Mega.cpp
mklink /h .\SerialLog.h ..\src\SerialLog_Mega.h
mklink /h .\Settings.cpp ..\src\Settings_Mega.cpp

mklink /h .\Webpages\Logs.html ..\src\Webpages\Logs.html
mklink /h .\Webpages\Settings.html ..\src\Webpages\Settings.html

mklink /h .\src\Helpers.cpp ..\src\Helpers.cpp
mklink /h .\src\Helpers.h ..\src\Helpers.h
mklink /h .\src\WirelessCommands_Aero.h ..\src\WirelessCommands_Aero.h
mklink /h .\src\WirelessCommands_Hempy.h ..\src\WirelessCommands_Hempy.h
mklink /h .\src\WirelessCommands_Reservoir.h ..\src\WirelessCommands_Reservoir.h

mklink /h .\src\Modules_Web\420Module_Web.h ..\src\Modules_Web\420Module_Web.h
mklink /h .\src\Modules_Web\420Module_Web.cpp ..\src\Modules_Web\420Module_Web.cpp
mklink /h .\src\Modules_Web\MainModule_Web.h ..\src\Modules_Web\MainModule_Web.h
mklink /h .\src\Modules_Web\MainModule_Web.cpp ..\src\Modules_Web\MainModule_Web.cpp
mklink /h .\src\Modules_Web\AeroModule_Web.h ..\src\Modules_Web\AeroModule_Web.h
mklink /h .\src\Modules_Web\AeroModule_Web.cpp ..\src\Modules_Web\AeroModule_Web.cpp
mklink /h .\src\Modules_Web\HempyModule_Web.h ..\src\Modules_Web\HempyModule_Web.h
mklink /h .\src\Modules_Web\HempyModule_Web.cpp ..\src\Modules_Web\HempyModule_Web.cpp
mklink /h .\src\Modules_Web\ReservoirModule_Web.h ..\src\Modules_Web\ReservoirModule_Web.h
mklink /h .\src\Modules_Web\ReservoirModule_Web.cpp ..\src\Modules_Web\ReservoirModule_Web.cpp

mklink /h .\src\Components_Web\420Common_Web.h ..\src\Components_Web\420Common_Web.h
mklink /h .\src\Components_Web\AirPump_Web.h ..\src\Components_Web\AirPump_Web.h
mklink /h .\src\Components_Web\AirPump_Web.cpp ..\src\Components_Web\AirPump_Web.cpp
mklink /h .\src\Components_Web\DHTSensor_Web.h ..\src\Components_Web\DHTSensor_Web.h
mklink /h .\src\Components_Web\DHTSensor_Web.cpp ..\src\Components_Web\DHTSensor_Web.cpp
mklink /h .\src\Components_Web\Fan_Web.h ..\src\Components_Web\Fan_Web.h
mklink /h .\src\Components_Web\Fan_Web.cpp ..\src\Components_Web\Fan_Web.cpp
REM mklink /h .\src\Components_Web\Fan_PWM_Web.h ..\src\Components_Web\Fan_PWM_Web.h
REM mklink /h .\src\Components_Web\Fan_PWM_Web.cpp ..\src\Components_Web\Fan_PWM_Web.cpp
mklink /h .\src\Components_Web\Lights_Web.h ..\src\Components_Web\Lights_Web.h
mklink /h .\src\Components_Web\Lights_Web.cpp ..\src\Components_Web\Lights_Web.cpp
mklink /h .\src\Components_Web\LightSensor_Web.h ..\src\Components_Web\LightSensor_Web.h
mklink /h .\src\Components_Web\LightSensor_Web.cpp ..\src\Components_Web\LightSensor_Web.cpp
REM mklink /h .\src\Components_Web\PowerSensor_Web.h ..\src\Components_Web\PowerSensor_Web.h
REM mklink /h .\src\Components_Web\PowerSensor_Web.cpp ..\src\Components_Web\PowerSensor_Web.cpp
mklink /h .\src\Components_Web\PowerSensorV3_Web.h ..\src\Components_Web\PowerSensorV3_Web.h
mklink /h .\src\Components_Web\PowerSensorV3_Web.cpp ..\src\Components_Web\PowerSensorV3_Web.cpp
mklink /h .\src\Components_Web\Sound_Web.h ..\src\Components_Web\Sound_Web.h
mklink /h .\src\Components_Web\Sound_Web.cpp ..\src\Components_Web\Sound_Web.cpp

mklink /h .\src\Components\420Common.h ..\src\Components\420Common.h
mklink /h .\src\Components\420Common.cpp ..\src\Components\420Common.cpp
mklink /h .\src\Components\420Module.h ..\src\Components\420Module.h
mklink /h .\src\Components\420Module.cpp ..\src\Components\420Module.cpp
mklink /h .\src\Components\AirPump.h ..\src\Components\AirPump.h
mklink /h .\src\Components\AirPump.cpp ..\src\Components\AirPump.cpp
mklink /h .\src\Components\DHTSensor.h ..\src\Components\DHTSensor.h
mklink /h .\src\Components\DHTSensor.cpp ..\src\Components\DHTSensor.cpp
mklink /h .\src\Components\Fan.h ..\src\Components\Fan.h
mklink /h .\src\Components\Fan.cpp ..\src\Components\Fan.cpp
REM mklink /h .\src\Components\Fan_PWM.h ..\src\Components\Fan_PWM.h
REM mklink /h .\src\Components\Fan_PWM.cpp ..\src\Components\Fan_PWM.cpp
mklink /h .\src\Components\Lights.h ..\src\Components\Lights.h
mklink /h .\src\Components\Lights.cpp ..\src\Components\Lights.cpp
mklink /h .\src\Components\LightSensor.h ..\src\Components\LightSensor.h
mklink /h .\src\Components\LightSensor.cpp ..\src\Components\LightSensor.cpp
REM mklink /h .\src\Components\PowerSensor.h ..\src\Components\PowerSensor.h
REM mklink /h .\src\Components\PowerSensor.cpp ..\src\Components\PowerSensor.cpp
mklink /h .\src\Components\PowerSensorV3.h ..\src\Components\PowerSensorV3.h
mklink /h .\src\Components\PowerSensorV3.cpp ..\src\Components\PowerSensorV3.cpp
mklink /h .\src\Components\Sound.h ..\src\Components\Sound.h
mklink /h .\src\Components\Sound.cpp ..\src\Components\Sound.cpp

REM echo "Done"
REM pause
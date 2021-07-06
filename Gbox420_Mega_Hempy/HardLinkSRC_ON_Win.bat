echo off
echo "------------------------------------------"
echo "Processing %CD%"
echo "------------------------------------------"

echo "Creating unlink command"
mklink /h .\HardLinkSRC_OFF.sh ..\HardLinkSRC_OFF.sh
mklink /h .\HardLinkSRC_OFF_Win.bat ..\HardLinkSRC_OFF_Win.bat

echo "Creating folders"
mkdir .\src
mkdir .\src\Components
mkdir .\src\Components_Web
mkdir .\src\Modules_Web
mkdir .\Webpages > nul 2>&1

echo "Creating hard links"
mklink /h .\SerialLog.cpp ..\src\SerialLog_Mega.cpp
mklink /h .\SerialLog.h ..\src\SerialLog_Mega.h
mklink /h .\Settings.cpp ..\src\Settings_Mega.cpp
mklink /h .\src\Helpers.cpp ..\src\Helpers.cpp
mklink /h .\src\Helpers.h ..\src\Helpers.h

mklink /h .\Webpages\Settings.html ..\src\Webpages\Settings.html
del .\Webpages\Main.html > nul 2>&1
mklink /h .\Webpages\Main.html ..\src\Webpages\Main.html

mklink /h .\src\Modules_Web\420Module_Web.h ..\src\Modules_Web\420Module_Web.h
mklink /h .\src\Modules_Web\420Module_Web.cpp ..\src\Modules_Web\420Module_Web.cpp
mklink /h .\src\Modules_Web\Hempy_Standalone.h ..\src\Modules_Web\Hempy_Standalone.h
mklink /h .\src\Modules_Web\Hempy_Standalone.cpp ..\src\Modules_Web\Hempy_Standalone.cpp

mklink /h .\src\Components_Web\420Common_Web.h ..\src\Components_Web\420Common_Web.h
mklink /h .\src\Components_Web\DHTSensor_Web.h ..\src\Components_Web\DHTSensor_Web.h
mklink /h .\src\Components_Web\DHTSensor_Web.cpp ..\src\Components_Web\DHTSensor_Web.cpp
mklink /h .\src\Components_Web\HempyBucket_Web.h ..\src\Components_Web\HempyBucket_Web.h
mklink /h .\src\Components_Web\HempyBucket_Web.cpp ..\src\Components_Web\HempyBucket_Web.cpp
mklink /h .\src\Components_Web\Sound_Web.h ..\src\Components_Web\Sound_Web.h
mklink /h .\src\Components_Web\Sound_Web.cpp ..\src\Components_Web\Sound_Web.cpp
mklink /h .\src\Components_Web\WaterPump_Web.h ..\src\Components_Web\WaterPump_Web.h
mklink /h .\src\Components_Web\WaterPump_Web.cpp ..\src\Components_Web\WaterPump_Web.cpp
mklink /h .\src\Components_Web\WasteReservoir_Web.h ..\src\Components_Web\WasteReservoir_Web.h
mklink /h .\src\Components_Web\WasteReservoir_Web.cpp ..\src\Components_Web\WasteReservoir_Web.cpp
mklink /h .\src\Components_Web\WeightSensor_Web.h ..\src\Components_Web\WeightSensor_Web.h
mklink /h .\src\Components_Web\WeightSensor_Web.cpp ..\src\Components_Web\WeightSensor_Web.cpp

mklink /h .\src\Components\420Common.h ..\src\Components\420Common.h
mklink /h .\src\Components\420Common.cpp ..\src\Components\420Common.cpp
mklink /h .\src\Components\420Module.h ..\src\Components\420Module.h
mklink /h .\src\Components\420Module.cpp ..\src\Components\420Module.cpp
mklink /h .\src\Components\DHTSensor.h ..\src\Components\DHTSensor.h
mklink /h .\src\Components\DHTSensor.cpp ..\src\Components\DHTSensor.cpp
mklink /h .\src\Components\Sound.h ..\src\Components\Sound.h
mklink /h .\src\Components\Sound.cpp ..\src\Components\Sound.cpp
mklink /h .\src\Components\HempyBucket.h ..\src\Components\HempyBucket.h
mklink /h .\src\Components\HempyBucket.cpp ..\src\Components\HempyBucket.cpp
mklink /h .\src\Components\Switch.h ..\src\Components\Switch.h
mklink /h .\src\Components\Switch.cpp ..\src\Components\Switch.cpp
mklink /h .\src\Components\Switch_PWM.h ..\src\Components\Switch_PWM.h
mklink /h .\src\Components\Switch_PWM.cpp ..\src\Components\Switch_PWM.cpp
mklink /h .\src\Components\WaterPump.h ..\src\Components\WaterPump.h
mklink /h .\src\Components\WaterPump.cpp ..\src\Components\WaterPump.cpp
mklink /h .\src\Components\WasteReservoir.h ..\src\Components\WasteReservoir.h
mklink /h .\src\Components\WasteReservoir.cpp ..\src\Components\WasteReservoir.cpp
mklink /h .\src\Components\WeightSensor.h ..\src\Components\WeightSensor.h
mklink /h .\src\Components\WeightSensor.cpp ..\src\Components\WeightSensor.cpp

REM echo "Done"
REM pause
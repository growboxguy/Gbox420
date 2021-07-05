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
mkdir .\src\Modules

echo "Creating hard links"
mklink /h .\SerialLog.cpp ..\src\SerialLog_Nano.cpp
mklink /h .\SerialLog.h ..\src\SerialLog_Nano.h
mklink /h .\Settings.cpp ..\src\Settings_Nano.cpp
mklink /h .\src\Modules\HempyModule.cpp ..\src\Modules\HempyModule.cpp
mklink /h .\src\Modules\HempyModule.h ..\src\Modules\HempyModule.h

mklink /h .\src\Helpers.cpp ..\src\Helpers.cpp
mklink /h .\src\Helpers.h ..\src\Helpers.h
mklink /h .\src\WirelessCommands_Hempy.h ..\src\WirelessCommands_Hempy.h

mklink /h .\src\Components\420Common.h ..\src\Components\420Common.h
mklink /h .\src\Components\420Common.cpp ..\src\Components\420Common.cpp
mklink /h .\src\Components\420Module.h ..\src\Components\420Module.h
mklink /h .\src\Components\420Module.cpp ..\src\Components\420Module.cpp
mklink /h .\src\Components\HempyBucket.h ..\src\Components\HempyBucket.h
mklink /h .\src\Components\HempyBucket.cpp ..\src\Components\HempyBucket.cpp
mklink /h .\src\Components\Sound.h ..\src\Components\Sound.h
mklink /h .\src\Components\Sound.cpp ..\src\Components\Sound.cpp
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
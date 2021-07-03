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
mklink /h .\src\Modules\ReservoirModule.cpp ..\src\Modules\ReservoirModule.cpp
mklink /h .\src\Modules\ReservoirModule.h ..\src\Modules\ReservoirModule.h

mklink /h .\src\Helpers.cpp ..\src\Helpers.cpp
mklink /h .\src\Helpers.h ..\src\Helpers.h
mklink /h .\src\WirelessCommands_Reservoir.h ..\src\WirelessCommands_Reservoir.h

mklink /h .\src\Components\420Common.h ..\src\Components\420Common.h
mklink /h .\src\Components\420Common.cpp ..\src\Components\420Common.cpp
mklink /h .\src\Components\420Module.h ..\src\Components\420Module.h
mklink /h .\src\Components\420Module.cpp ..\src\Components\420Module.cpp
mklink /h .\src\Components\Sound.h ..\src\Components\Sound.h
mklink /h .\src\Components\Sound.cpp ..\src\Components\Sound.cpp
mklink /h .\src\Components\DHTSensor.h ..\src\Components\DHTSensor.h
mklink /h .\src\Components\DHTSensor.cpp ..\src\Components\DHTSensor.cpp
mklink /h .\src\Components\PHSensor.h ..\src\Components\PHSensor.h
mklink /h .\src\Components\PHSensor.cpp ..\src\Components\PHSensor.cpp
mklink /h .\src\Components\TDSSensor.h ..\src\Components\TDSSensor.h
mklink /h .\src\Components\TDSSensor.cpp ..\src\Components\TDSSensor.cpp
mklink /h .\src\Components\WaterTempSensor.h ..\src\Components\WaterTempSensor.h
mklink /h .\src\Components\WaterTempSensor.cpp ..\src\Components\WaterTempSensor.cpp
mklink /h .\src\Components\WeightSensor.h ..\src\Components\WeightSensor.h
mklink /h .\src\Components\WeightSensor.cpp ..\src\Components\WeightSensor.cpp

REM echo "Done"
REM pause
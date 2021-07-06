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
mkdir .\Webpages

echo "Creating hard links"
mklink /h .\SerialLog.cpp ..\src\SerialLog_Mega.cpp
mklink /h .\SerialLog.h ..\src\SerialLog_Mega.h
mklink /h .\Settings.cpp ..\src\Settings_Mega.cpp
mklink /h .\src\Helpers.cpp ..\src\Helpers.cpp
mklink /h .\src\Helpers.h ..\src\Helpers.h
mklink /h .\src\WirelessCommands_Dev.h ..\src\WirelessCommands_Dev.h

mklink /h .\Webpages\Settings.html ..\src\Webpages\Settings.html

mklink /h .\src\Modules_Web\420Module_Web.h ..\src\Modules_Web\420Module_Web.h
mklink /h .\src\Modules_Web\420Module_Web.cpp ..\src\Modules_Web\420Module_Web.cpp
mklink /h .\src\Modules_Web\DevModule_Web.h ..\src\Modules_Web\DevModule_Web.h
mklink /h .\src\Modules_Web\DevModule_Web.cpp ..\src\Modules_Web\DevModule_Web.cpp

mklink /h .\src\Components_Web\420Common_Web.h ..\src\Components_Web\420Common_Web.h
mklink /h .\src\Components_Web\DHTSensor_Web.h ..\src\Components_Web\DHTSensor_Web.h
mklink /h .\src\Components_Web\DHTSensor_Web.cpp ..\src\Components_Web\DHTSensor_Web.cpp
mklink /h .\src\Components_Web\Sound_Web.h ..\src\Components_Web\Sound_Web.h
mklink /h .\src\Components_Web\Sound_Web.cpp ..\src\Components_Web\Sound_Web.cpp

mklink /h .\src\Components\420Common.h ..\src\Components\420Common.h
mklink /h .\src\Components\420Common.cpp ..\src\Components\420Common.cpp
mklink /h .\src\Components\420Module.h ..\src\Components\420Module.h
mklink /h .\src\Components\420Module.cpp ..\src\Components\420Module.cpp
mklink /h .\src\Components\DHTSensor.h ..\src\Components\DHTSensor.h
mklink /h .\src\Components\DHTSensor.cpp ..\src\Components\DHTSensor.cpp
mklink /h .\src\Components\Sound.h ..\src\Components\Sound.h
mklink /h .\src\Components\Sound.cpp ..\src\Components\Sound.cpp

REM echo "Done"
REM pause
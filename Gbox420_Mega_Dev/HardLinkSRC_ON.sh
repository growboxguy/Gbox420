echo "This script is for developement only"
echo "Protects shared Gbox420 and Gbox420 Nano Components from going out of sync by hard linking the source files"
echo "------------------------------------------"

echo "Creating unlink command"
ln ../HardLinkSRC_OFF.sh ./

echo "Creating folders"
mkdir ./src
mkdir ./Webpages
mkdir ./src/Components
mkdir ./src/Modules_Web

echo "Creating hard links"
ln ../Gbox420_Mega/Settings.cpp ./
ln ../Gbox420_Mega/SerialLog.cpp ./
ln ../Gbox420_Mega/SerialLog.h ./

ln ../Gbox420_Mega/Webpages/Settings.html ./Webpages/
ln ../Gbox420_Mega/Webpages/Logs.html ./Webpages/

ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/WirelessCommands_Dev.h ./src


ln ../src/Modules_Web/420Common_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/420Common_Web.h ./src/Modules_Web
ln ../src/Modules_Web/420Module_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/420Module_Web.h ./src/Modules_Web
ln ../src/Modules_Web/DevModule_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/DevModule_Web.h ./src/Modules_Web


ln ../src/Components/420Common.h ./src/Components
ln ../src/Components/420Common.cpp ./src/Components
ln ../src/Components/420Module.h ./src/Components
ln ../src/Components/420Module.cpp ./src/Components
ln ../src/Components/Sound.h ./src/Components
ln ../src/Components/Sound.cpp ./src/Components
ln ../src/Components/DHTSensor.h ./src/Components
ln ../src/Components/DHTSensor.cpp ./src/Components

echo "Done, press enter to exit"
read fakeinput 


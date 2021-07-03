echo "------------------------------------------"
echo "Processing "$PWD
echo "------------------------------------------"

echo "Creating unlink command"
ln ../HardLinkSRC_OFF.sh ./
ln ../HardLinkSRC_OFF_Win.bat ./

echo "Creating folders"
mkdir ./src
mkdir ./src/Components
mkdir ./src/Components_Web
mkdir ./src/Modules_Web
mkdir ./Webpages

echo "Creating hard links"
ln ../src/Settings_Mega.cpp ./Settings.cpp
ln ../src/SerialLog_Mega.h ./SerialLog.h
ln ../src/SerialLog_Mega.cpp ./SerialLog.cpp
ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/WirelessCommands_Dev.h ./src

ln ../src/Webpages/Logs.html ./Webpages/
ln ../src/Webpages/Settings.html ./Webpages/

ln ../src/Modules_Web/420Module_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/420Module_Web.h ./src/Modules_Web
ln ../src/Modules_Web/DevModule_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/DevModule_Web.h ./src/Modules_Web

ln ../src/Components_Web/420Common_Web.h ./src/Components_Web
ln ../src/Components_Web/DHTSensor_Web.h ./src/Components_Web
ln ../src/Components_Web/DHTSensor_Web.cpp ./src/Components_Web
ln ../src/Components_Web/Sound_Web.h ./src/Components_Web
ln ../src/Components_Web/Sound_Web.cpp ./src/Components_Web

ln ../src/Components/420Common.h ./src/Components
ln ../src/Components/420Common.cpp ./src/Components
ln ../src/Components/420Module.h ./src/Components
ln ../src/Components/420Module.cpp ./src/Components
ln ../src/Components/DHTSensor.h ./src/Components
ln ../src/Components/DHTSensor.cpp ./src/Components
ln ../src/Components/Sound.h ./src/Components
ln ../src/Components/Sound.cpp ./src/Components

#echo "Done, press enter to exit"
#read fakeinput
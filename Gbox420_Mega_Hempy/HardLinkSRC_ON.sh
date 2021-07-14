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
mkdir -p ./Webpages 

echo "Creating hard links"
ln ../src/Settings_Mega.cpp ./Settings.cpp
ln ../src/SerialLog_Mega.cpp ./SerialLog.cpp
ln ../src/SerialLog_Mega.h ./SerialLog.h
ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src

ln ../src/Webpages/Settings.html ./Webpages/
unlink ./Webpages/Main.html > /dev/null 2>&1
ln ../src/Webpages/Main.html ./Webpages/
unlink ./Webpages/Hempy_Lite/Main.html > /dev/null 2>&1
ln ../src/Webpages/Main.html ./Webpages/Hempy_Lite/

ln ../src/Modules_Web/420Module_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/420Module_Web.h ./src/Modules_Web
ln ../src/Modules_Web/Hempy_Standalone.cpp ./src/Modules_Web
ln ../src/Modules_Web/Hempy_Standalone.h ./src/Modules_Web

ln ../src/Components_Web/420Common_Web.h ./src/Components_Web
ln ../src/Components_Web/DHTSensor_Web.h ./src/Components_Web
ln ../src/Components_Web/DHTSensor_Web.cpp ./src/Components_Web
ln ../src/Components_Web/HempyBucket_Web.h ./src/Components_Web
ln ../src/Components_Web/HempyBucket_Web.cpp ./src/Components_Web
ln ../src/Components_Web/Sound_Web.h ./src/Components_Web
ln ../src/Components_Web/Sound_Web.cpp ./src/Components_Web
ln ../src/Components_Web/WaterPump_Web.h ./src/Components_Web
ln ../src/Components_Web/WaterPump_Web.cpp ./src/Components_Web
ln ../src/Components_Web/WasteReservoir_Web.h ./src/Components_Web
ln ../src/Components_Web/WasteReservoir_Web.cpp ./src/Components_Web
ln ../src/Components_Web/WeightSensor_Web.h ./src/Components_Web
ln ../src/Components_Web/WeightSensor_Web.cpp ./src/Components_Web

ln ../src/Components/420Common.h ./src/Components
ln ../src/Components/420Common.cpp ./src/Components
ln ../src/Components/420Module.h ./src/Components
ln ../src/Components/420Module.cpp ./src/Components
ln ../src/Components/DHTSensor.h ./src/Components
ln ../src/Components/DHTSensor.cpp ./src/Components
ln ../src/Components/Sound.h ./src/Components
ln ../src/Components/Sound.cpp ./src/Components
ln ../src/Components/HempyBucket.h ./src/Components
ln ../src/Components/HempyBucket.cpp ./src/Components
ln ../src/Components/Switch.h ./src/Components
ln ../src/Components/Switch.cpp ./src/Components
ln ../src/Components/Switch_PWM.h ./src/Components
ln ../src/Components/Switch_PWM.cpp ./src/Components
ln ../src/Components/WaterPump.h ./src/Components
ln ../src/Components/WaterPump.cpp ./src/Components
ln ../src/Components/WasteReservoir.h ./src/Components
ln ../src/Components/WasteReservoir.cpp ./src/Components
ln ../src/Components/WeightSensor.h ./src/Components
ln ../src/Components/WeightSensor.cpp ./src/Components

#echo "Done, press enter to exit"
#read fakeinput 

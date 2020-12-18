echo "This script is for developement only"
echo "Protects shared Gbox420 and Gbox420 Nano Components from going out of sync by hard linking the source files"
echo "------------------------------------------"

echo "Creating unlink command"
ln ../HardLinkSRC_OFF.sh ./

echo "Creating folders"
mkdir ./src
mkdir ./src/Components
mkdir ./src/Components_Web
mkdir ./src/Modules_Web

echo "Creating hard links"
ln ../Gbox420_Mega/Settings.cpp ./
ln ../Gbox420_Mega/SerialLog.cpp ./
ln ../Gbox420_Mega/SerialLog.h ./

ln ../Gbox420_Mega/Webpages/Logs.html ./Webpages/
ln ../Gbox420_Mega/Webpages/Settings.html ./Webpages/

ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/RollingAverage.cpp ./src
ln ../src/RollingAverage.h ./src
ln ../src/WirelessCommands_Aero.h ./src
ln ../src/WirelessCommands_Hempy.h ./src
ln ../src/WirelessCommands_Reservoir.h ./src

ln ../src/Modules_Web/MainModule_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/MainModule_Web.h ./src/Modules_Web
ln ../src/Modules_Web/AeroModule_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/AeroModule_Web.h ./src/Modules_Web
ln ../src/Modules_Web/HempyModule_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/HempyModule_Web.h ./src/Modules_Web
ln ../src/Modules_Web/ReservoirModule_Web.cpp ./src/Modules_Web
ln ../src/Modules_Web/ReservoirModule_Web.h ./src/Modules_Web

ln ../src/Components/420Common.h ./src/Components
ln ../src/Components/420Common.cpp ./src/Components
ln ../src/Components/420Module.h ./src/Components
ln ../src/Components/420Module.cpp ./src/Components
ln ../src/Components/AirPump.h ./src/Components
ln ../src/Components/AirPump.cpp ./src/Components
ln ../src/Components/Sound.h ./src/Components
ln ../src/Components/Sound.cpp ./src/Components
ln ../src/Components/DHTSensor.h ./src/Components
ln ../src/Components/DHTSensor.cpp ./src/Components
ln ../src/Components/Lights.h ./src/Components
ln ../src/Components/Lights.cpp ./src/Components
ln ../src/Components/LightSensor.h ./src/Components
ln ../src/Components/LightSensor.cpp ./src/Components
ln ../src/Components/Fan.h ./src/Components
ln ../src/Components/Fan.cpp ./src/Components
ln ../src/Components/PowerSensor.h ./src/Components
ln ../src/Components/PowerSensor.cpp ./src/Components
ln ../src/Components/PowerSensorV3.h ./src/Components
ln ../src/Components/PowerSensorV3.cpp ./src/Components

ln ../src/Components_Web/420Common_Web.h ./src/Components_Web
ln ../src/Components_Web/420Common_Web.cpp ./src/Components_Web
ln ../src/Components_Web/420Module_Web.h ./src/Components_Web
ln ../src/Components_Web/420Module_Web.cpp ./src/Components_Web
ln ../src/Components_Web/AirPump_Web.h ./src/Components_Web
ln ../src/Components_Web/AirPump_Web.cpp ./src/Components_Web
ln ../src/Components_Web/Sound_Web.h ./src/Components_Web
ln ../src/Components_Web/Sound_Web.cpp ./src/Components_Web
ln ../src/Components_Web/DHTSensor_Web.h ./src/Components_Web
ln ../src/Components_Web/DHTSensor_Web.cpp ./src/Components_Web
ln ../src/Components_Web/Lights_Web.h ./src/Components_Web
ln ../src/Components_Web/Lights_Web.cpp ./src/Components_Web
ln ../src/Components_Web/LightSensor_Web.h ./src/Components_Web
ln ../src/Components_Web/LightSensor_Web.cpp ./src/Components_Web
ln ../src/Components_Web/Fan_Web.h ./src/Components_Web
ln ../src/Components_Web/Fan_Web.cpp ./src/Components_Web
ln ../src/Components_Web/PowerSensor_Web.h ./src/Components_Web
ln ../src/Components_Web/PowerSensor_Web.cpp ./src/Components_Web
ln ../src/Components_Web/PowerSensorV3_Web.h ./src/Components_Web
ln ../src/Components_Web/PowerSensorV3_Web.cpp ./src/Components_Web

echo "Done, press enter to exit"
read fakeinput 


echo "------------------------------------------"
echo "Processing "$PWD
echo "------------------------------------------"

echo "Creating unlink command"
ln ../HardLinkSRC_OFF.sh ./

echo "Creating folders"
mkdir ./src
mkdir ./src/Components
mkdir ./src/Components_Web
mkdir ./src/Modules

echo "Creating hard links"
ln ../Gbox420_Nano/SerialLog.cpp ./
ln ../Gbox420_Nano/SerialLog.h ./
ln ../Gbox420_Nano/Settings.cpp ./
ln ../src/Modules/ReservoirModule.cpp ./src/Modules
ln ../src/Modules/ReservoirModule.h ./src/Modules

ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/WirelessCommands_Reservoir.h ./src

ln ../src/Components_Web/420Common_Web.h ./src/Components_Web

ln ../src/Components/420Common.h ./src/Components
ln ../src/Components/420Common.cpp ./src/Components
ln ../src/Components/420Module.h ./src/Components
ln ../src/Components/420Module.cpp ./src/Components
ln ../src/Components/Sound.h ./src/Components
ln ../src/Components/Sound.cpp ./src/Components
ln ../src/Components/DHTSensor.h ./src/Components
ln ../src/Components/DHTSensor.cpp ./src/Components
ln ../src/Components/PHSensor.h ./src/Components
ln ../src/Components/PHSensor.cpp ./src/Components
ln ../src/Components/TDSSensor.h ./src/Components
ln ../src/Components/TDSSensor.cpp ./src/Components
ln ../src/Components/WaterTempSensor.h ./src/Components
ln ../src/Components/WaterTempSensor.cpp ./src/Components
ln ../src/Components/WeightSensor.h ./src/Components
ln ../src/Components/WeightSensor.cpp ./src/Components

#echo "Done, press enter to exit"
#read fakeinput
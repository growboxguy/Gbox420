echo "------------------------------------------"
echo "Processing "$PWD
echo "------------------------------------------"

echo "Creating unlink command"
ln ../HardLinkSRC_OFF.sh ./
ln ../HardLinkSRC_OFF_Win.bat ./

echo "Creating folders"
mkdir ./src
mkdir ./src/Components
mkdir ./src/Modules

echo "Creating hard links"
ln ../src/SerialLog_Nano.h ./SerialLog.h
ln ../src/SerialLog_Nano.cpp ./SerialLog.cpp
ln ../src/Settings_Nano.cpp ./Settings.cpp
ln ../src/Modules/HempyModule.h ./src/Modules
ln ../src/Modules/HempyModule.cpp ./src/Modules

ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/WirelessCommands_Hempy.h ./src

ln ../src/Components/420Common.h ./src/Components
ln ../src/Components/420Common.cpp ./src/Components
ln ../src/Components/420Module.h ./src/Components
ln ../src/Components/420Module.cpp ./src/Components
ln ../src/Components/HempyBucket.h ./src/Components
ln ../src/Components/HempyBucket.cpp ./src/Components
ln ../src/Components/Sound.h ./src/Components
ln ../src/Components/Sound.cpp ./src/Components
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
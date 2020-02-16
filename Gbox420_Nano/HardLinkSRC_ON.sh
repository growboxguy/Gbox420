echo "This script is for developement only"
echo "Protects shared Gbox420 and Gbox420 Nano Components from going out of sync by hard linking the source files"
echo "------------------------------------------"

echo "Creating folders"
mkdir ./src
mkdir ./src/Components

echo "Creating hard links"
ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/RollingAverage.h ./src
ln ../src/RollingAverage.cpp ./src

ln ../src/Components/420Common.h ./src/Components
ln ../src/Components/420Common.cpp ./src/Components
ln ../src/Components/420Module.h ./src/Components
ln ../src/Components/420Module.cpp ./src/Components
ln ../src/Components/DHTSensor.h ./src/Components
ln ../src/Components/DHTSensor.cpp ./src/Components
ln ../src/Components/PHSensor.h ./src/Components
ln ../src/Components/PHSensor.cpp ./src/Components
ln ../src/Components/Sound.h ./src/Components
ln ../src/Components/Sound.cpp ./src/Components
ln ../src/Components/WaterLevelSensor.h ./src/Components
ln ../src/Components/WaterLevelSensor.cpp ./src/Components
ln ../src/Components/WaterTempSensor.h ./src/Components
ln ../src/Components/WaterTempSensor.cpp ./src/Components
ln ../src/Components/WeightSensor.h ./src/Components
ln ../src/Components/WeightSensor.cpp ./src/Components

echo "Done, press enter to exit"
read fakeinput 


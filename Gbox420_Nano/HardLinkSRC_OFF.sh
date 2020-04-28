echo "This script is for developement only"
echo "Removes the hard links to source files of components shared by Gbox420 and Gbox420 Nano"
echo "------------------------------------"
echo "Removing hard links to Gbox420 src folder"

unlink ./src/Components/HempyModule.h 
unlink ./src/Components/HempyModule.cpp

unlink ./src/Helpers.cpp
unlink ./src/Helpers.h
unlink ./src/RollingAverage.h
unlink ./src/RollingAverage.cpp

unlink ./src/Components/420Common.h
unlink ./src/Components/420Common.cpp
unlink ./src/Components/420Module.h
unlink ./src/Components/420Module.cpp
unlink ./src/Components/DHTSensor.h
unlink ./src/Components/DHTSensor.cpp
unlink ./src/Components/HempyBucket.h
unlink ./src/Components/HempyBucket.cpp
unlink ./src/Components/PHSensor.h
unlink ./src/Components/PHSensor.cpp
unlink ./src/Components/Sound.h
unlink ./src/Components/Sound.cpp
unlink ./src/Components/WaterLevelSensor.h
unlink ./src/Components/WaterLevelSensor.cpp
unlink ./src/Components/WaterPump.h
unlink ./src/Components/WaterPump.cpp
unlink ./src/Components/WaterTempSensor.h
unlink ./src/Components/WaterTempSensor.cpp
unlink ./src/Components/WeightSensor.h
unlink ./src/Components/WeightSensor.cpp

echo "Cleaning up empty folders"
find . -type d -empty -delete

echo "Done, press enter to exit"
read fakeinput 

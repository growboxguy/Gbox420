echo "This script is for developement only"
echo "Mounts shared components from the Gbox420 sketch"
echo "------------------------------------------"
echo "Creating mounting point for Modules"
mkdir ./src
mkdir ./src/Components
sudo mount -o bind ../src/Components ./src/Components

echo "Creating hard links"
ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/RollingAverage.h ./src
ln ../src/RollingAverage.cpp ./src
ln ../src/Settings.h ./src

echo "Done, press enter to exit"
read fakeinput 


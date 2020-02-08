echo "This script is for developement only"
echo "Mounts shared components from the Gbox420 sketch"
echo "------------------------------------------"
echo "Creating mounting point for Modules"
mkdir ./src
mkdir ./src/Modules
sudo mount -o bind ../src/Modules ./src/Modules

echo "Creating hard links"
ln ../src/Helpers.cpp ./src
ln ../src/Helpers.h ./src
ln ../src/RollingAverage.h ./src
ln ../src/RollingAverage.cpp ./src
ln ../src/Settings.h ./src

read -p "Done, press enter to exit"


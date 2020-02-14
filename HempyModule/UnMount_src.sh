echo "This script is for developement only"
echo "------------------------------------"
echo "Removing mounting point for Modules"
sudo umount ./src/Modules
sudo umount ./src/Components

echo "Removing hard links"
unlink ./src/Helpers.cpp
unlink ./src/Helpers.h
unlink ./src/RollingAverage.h
unlink ./src/RollingAverage.cpp
unlink ./src/Settings.h

echo "Cleaning up empty folders"
find . -type d -empty -delete

echo "Done, press enter to exit"
read fakeinput 

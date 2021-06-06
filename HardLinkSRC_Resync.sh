echo "This script is for developement only"
echo "Recreates the hard links to source files under the Gbox420/src folder"
echo "------------------------------------"
echo "Removing hard links to src folder"

(cd ./Gbox420_Mega_Dev ; sh ./HardLinkSRC_OFF.sh)
(cd ./Gbox420_Mega_Main ; sh ./HardLinkSRC_OFF.sh)
(cd ./Gbox420_Nano_Aero_NoTank ; sh ./HardLinkSRC_OFF.sh)
(cd ./Gbox420_Nano_Aero_Tank ; sh ./HardLinkSRC_OFF.sh)
(cd ./Gbox420_Nano_Hempy ; sh ./HardLinkSRC_OFF.sh)
(cd ./Gbox420_Nano_Reservoir ; sh ./HardLinkSRC_OFF.sh)

echo "------------------------------------"
echo "Creating hard links to src folder"

(cd ./Gbox420_Mega_Dev ; sh ./HardLinkSRC_ON.sh)
(cd ./Gbox420_Mega_Main ; sh ./HardLinkSRC_ON.sh)
(cd ./Gbox420_Nano_Aero_NoTank ; sh ./HardLinkSRC_ON.sh)
(cd ./Gbox420_Nano_Aero_Tank ; sh ./HardLinkSRC_ON.sh)
(cd ./Gbox420_Nano_Hempy ; sh ./HardLinkSRC_ON.sh)
(cd ./Gbox420_Nano_Reservoir ; sh ./HardLinkSRC_ON.sh)

echo "------------------------------------"
echo "Done, press enter to exit"
read fakeinput 

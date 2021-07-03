echo off
echo "This script is for developement only"
echo "Recreates the hard links to source files under the Gbox420/src folder"
echo "------------------------------------"
echo "Removing hard links to src folder"

cd ./Gbox420_Mega_Dev & call HardLinkSRC_OFF_Win.bat & cd..
cd ./Gbox420_Mega_Main & call HardLinkSRC_OFF_Win.bat & cd..
cd ./Gbox420_Mega_Hempy & call HardLinkSRC_OFF_Win.bat & cd..
cd ./Gbox420_Nano_Aero_NoTank & call HardLinkSRC_OFF_Win.bat & cd..
cd ./Gbox420_Nano_Aero_Tank & call HardLinkSRC_OFF_Win.bat & cd..
cd ./Gbox420_Nano_Hempy & call HardLinkSRC_OFF_Win.bat & cd..
cd ./Gbox420_Nano_Reservoir & call HardLinkSRC_OFF_Win.bat & cd..

echo "------------------------------------"
echo "Removed links, press enter to continue re-creating the links "
pause
echo "------------------------------------"
echo "Creating hard links to src folder"

cd ./Gbox420_Mega_Dev & call HardLinkSRC_ON_Win.bat & cd..
cd ./Gbox420_Mega_Main & call HardLinkSRC_ON_Win.bat & cd..
cd ./Gbox420_Mega_Hempy & call HardLinkSRC_ON_Win.bat & cd..
cd ./Gbox420_Nano_Aero_NoTank & call HardLinkSRC_ON_Win.bat & cd..
cd ./Gbox420_Nano_Aero_Tank & call HardLinkSRC_ON_Win.bat & cd..
cd ./Gbox420_Nano_Hempy & call HardLinkSRC_ON_Win.bat & cd..
cd ./Gbox420_Nano_Reservoir & call HardLinkSRC_ON_Win.bat & cd..

echo "------------------------------------"
echo "Done, press enter to exit"
pause

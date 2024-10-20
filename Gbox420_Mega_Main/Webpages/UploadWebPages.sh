echo "------------------------------------"
echo "Uploading web site files to ESP-link... "

curl -F 'files[]=@./Main.html' -F 'files[]=@./Hempy.html' -F 'files[]=@./Settings.html' http://192.168.9.101/web-server/upload

echo "Done, please Reload the ESP-link website for the changes to apply. If the new tabs are not visible Reset ESP-link from the Debug tab"
echo "Press enter to exit"
read fakeinput

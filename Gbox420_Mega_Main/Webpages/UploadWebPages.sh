echo "------------------------------------"
echo "Uploading web site files to ESP-link... "

curl -F 'files[]=@./Main.html' -F 'files[]=@./Hempy.html' -F 'files[]=@./Settings.html' http://192.168.9.101/web-server/upload

echo "Done, please Reset ESP-link from the Debug tab for the new tabs to appear."
echo "Press enter to exit"
read fakeinput

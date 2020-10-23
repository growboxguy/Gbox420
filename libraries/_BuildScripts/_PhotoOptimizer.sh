echo "Target folder name?"
read foldername
if [ ! -d "$foldername" ]; then
    mkdir ./"$foldername"
    ls ./_VipeTargets| cat -n | while read n f; do mv "./_VipeTargets/$f" "./$foldername/$(printf "%04d" $n).jpg"; done
    for i in ./"$foldername"/*.jpg; do echo "Processing $i"; exiftool -all= "$i"; done
    for i in ./"$foldername"/*.jpg; do echo "Processing $i"; exiftool -geotag= "$i"; done
    for i in ./"$foldername"/*.jpg_original; do rm "$i"; done

    parallel convert '{}' '{.}.jpg' ::: *.png
    jpegoptim -m 50 -s *

    echo "Finished. Press enter to exit."    
    read fakeinput
else
echo "Folder already exists, restarting"
exec "./_CleanCommands.sh"
fi


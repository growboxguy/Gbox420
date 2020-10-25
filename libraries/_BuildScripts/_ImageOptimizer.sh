echo "Image optimizer for web pages"

read -p "Source folder (default: ./_Source):" SOURCE
SOURCE=${SOURCE:-_Source}
SOURCE="./$SOURCE"
read -p "Target folder (default: ./_Destionation): " DESTINATION
DESTINATION=${DESTINATION:-_Destination}
DESTINATION="./$DESTINATION"
read -p "Tag images (Output: TAG_imgage.jpg): " TAG

if [ ! -d "$DESTINATION" ]; then
    echo "Creating $DESTINATION ..."
    mkdir "$DESTINATION"
fi
if [ ! -d "$DESTINATION/original" ]; then
    echo "Creating $DESTINATION/original ..."
    mkdir "$DESTINATION/original"
fi

echo "Converting PNG files to JPG..."
for i in $SOURCE/*.png ; do [ -f "$i" ] || continue; convert "$i" "${i%.*}.jpg" ; done

echo "Storing original images in $DESTINATION/original :"
ls $SOURCE| cat -n | while read n f; do cp "$SOURCE/$f" "$DESTINATION/original/${TAG}_$f"; done
echo "Moving images to $DESTINATION :"
ls $SOURCE| cat -n | while read n f; do cp "$SOURCE/$f" "$DESTINATION/${TAG}_$(printf "%04d" $n).jpg"; done
#ls $SOURCE| cat -n | while read n f; do mv "$SOURCE/$f" "$DESTINATION/${TAG}_$(printf "%04d" $n).jpg"; done

echo "Stripping EXIF data :"
for i in $DESTINATION/$TAG*.jpg; do echo "Processing $i"; exiftool -overwrite_original -all= "$i"; done
for i in $DESTINATION/$TAG*.jpg; do echo "Processing $i"; exiftool -overwrite_original -geotag= "$i"; done
#for i in ./"$DESTINATION"/*.jpg_original; do rm "$i"; done

echo "Changing resolution..."
for i in $DESTINATION/$TAG*.jpg; do convert $i -resize '1280x>' $i; done

echo "Optimising JPG..."
jpegoptim -m 70 -s $DESTINATION/$TAG*.jpg
echo
echo "Finished. Press enter to exit."    
read fakeinput



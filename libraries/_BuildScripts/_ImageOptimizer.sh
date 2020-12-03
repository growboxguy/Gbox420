echo "Image optimizer for web pages"

read -p "Source folder (default: ./_Source):" SOURCE
SOURCE=${SOURCE:-_Source}
SOURCE="./$SOURCE"
read -p "Target folder (default: ./_Destionation): " DESTINATION
DESTINATION=${DESTINATION:-_Destination}
DESTINATION="./$DESTINATION"
read -p "Tag file names (Output: TAG_image.jpg): " TAG

if [ ! -d "$DESTINATION" ]; then
    echo "Creating $DESTINATION ..."
    mkdir "$DESTINATION"
fi
if [ ! -d "$DESTINATION/original_$TAG" ]; then
    echo "Creating $DESTINATION/original_$TAG ..."
    mkdir "$DESTINATION/original_$TAG"
fi

echo "Storing original images in $DESTINATION/original_$TAG..."
ls $SOURCE | cat -n | while read n f; do mv "$SOURCE/$f" "$DESTINATION/original_$TAG/${TAG}_$f"; done

echo "Converting PNG files to JPG..."
for i in `ls $DESTINATION/original_$TAG/ | grep -i .png`; do convert "$i" "${i%.*}.jpg" ; done

echo "Copying JPG images to $DESTINATION..."
ls $DESTINATION/original_$TAG/ | grep -i .jpg | cat -n | while read n f; do cp "$DESTINATION/original_$TAG/$f" "$DESTINATION/${TAG}_$n.jpg" ; done

echo "Stripping EXIF data..."
for i in `ls $DESTINATION/ | grep -i $TAG.*.jpg`; do echo "  Processing EXIF - $i"; exiftool -overwrite_original -all= "$DESTINATION/$i"; done
#for i in $DESTINATION/$TAG*.jpg; do echo "Processing $i"; exiftool -overwrite_original -geotag= "$i"; done

echo "Changing resolution..."
for i in `ls $DESTINATION/ | grep -i $TAG.*.jpg`; do echo "  Resizing - $i"; convert "$DESTINATION/$i" -resize '1280x>' "$DESTINATION/$i"; done

echo "Optimising JPG..."
jpegoptim -m 80 -s $DESTINATION/$TAG*

echo
echo "Finished. Press enter to exit."    
read fakeinput



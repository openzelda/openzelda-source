#!/bin/bash

enginedir=/Users/user/Source/openzelda/engine
resourcesdir=/Users/user/Source/openzelda/resources
output_dir=/Users/user/Source/

rm -R $output_dir/OpenZelda.app
unzip -q $enginedir/custom/OpenZelda.app.zip -d $output_dir

mkdir -p $output_dir/OpenZelda.app/Contents/share/openzelda-2.0/

cp $resourcesdir/bin/openzelda $output_dir/OpenZelda.app/Contents/MacOS/
cp $enginedir/custom/Info.plist $output_dir/OpenZelda.app/Contents/
cp $enginedir/custom/openzelda-player.icns $output_dir/OpenZelda.app/Contents/Resources/
cp $resourcesdir/share/openzelda-2.0/portal.css $output_dir/OpenZelda.app/Contents/share/openzelda-2.0/
cp $resourcesdir/share/openzelda-2.0/button-border.png $output_dir/OpenZelda.app/Contents/share/openzelda-2.0/





function do_strip {
    tp=$(file -b --mime-type "$1")

    if [ "$tp" != "application/octet-stream" ]; then
        return
    fi

    name=$(mktemp -t bundle)
    st=$(stat -f %p "$1")
    
    strip -o "$name" -S "$1"
    mv -f "$name" "$1"

    chmod "$st" "$1"
    chmod u+w "$1"
}


echo "Strip debug symbols from bundle binaries"

# Strip debug symbols
for i in $(find -E $output_dir"OpenZelda.app/Contents/Frameworks" -type f -regex '.*\.(so|dylib)$'); do
    do_strip "$i"
done


do_strip $output_dir"OpenZelda.app/Contents/MacOS/openzelda"





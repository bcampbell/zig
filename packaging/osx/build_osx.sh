#!/bin/bash

# NOTE: THIS SCRIPT NOT CURRENT OR WORKING!!!
# left in purely for reference, an example of creating an OSX app bundle.
exit(1)

set -e

DESTDIR=`pwd`/build
echo "building in $DESTDIR"
bundle=$DESTDIR/zig.app

extralibs="/usr/local/opt/libpng/lib/libpng16.16.dylib"



build_icon() {
    infile="$1"
    outfile="$2"
    t=$(mktemp -d /tmp/zig.XXXXXX.iconset)

    cp $infile $t/icon_512x512.png
    convert $infile -resize 16x16\!  $t/icon_16x16.png
    convert $infile -resize 32x32\!  $t/icon_16x16@2x.png
    convert $infile -resize 32x32\!  $t/icon_32x32.png
    convert $infile -resize 64x64\!  $t/icon_32x32@2x.png

    iconutil --convert icns --output $outfile $t
    rm -r $t
}



pushd src >/dev/null

echo "set up bundle structure..."
mkdir -p $bundle
mkdir -p $bundle/Contents
mkdir -p $bundle/Contents/MacOS
mkdir -p $bundle/Contents/Resources
mkdir -p $bundle/Contents/Frameworks

echo "build icon..."
build_icon icon/icon.png $bundle/Contents/Resources/zig.icns

echo "compile..."
make release
cp zig $bundle/Contents/MacOS
cp osx/Info.plist $bundle/Contents/

echo "install/patch extra dylibs..."
# copy the extra libs into the bundle and patch them up
for e in $extralibs
do
    l=`basename $e`
    cp "$e" "$bundle/Contents/Frameworks/"
    chmod ug+w "$bundle/Contents/Frameworks/$l"
    # fix up the dylib refs in the executable
    install_name_tool \
        -change $e \
        @rpath/$l \
        $bundle/Contents/MacOS/zig

    # fix up the self-reference in the dylib itself
    install_name_tool -id @rpath/$l $bundle/Contents/Frameworks/$l
done

echo "add frameworks..."
cp -r ~/Library/Frameworks/SDL2.framework $bundle/Contents/Frameworks/
cp -r ~/Library/Frameworks/SDL2_mixer.framework $bundle/Contents/Frameworks/


# TODO check versionnumber in Info.plist


echo "add data files..."
# add in the data files
cp -r data $bundle/Contents/Resources/

#echo "build dmg..."
#hdiutil create -ov -fs HFS+ -srcfolder $bundle $DESTDIR/zig.dmg

popd >/dev/null
echo "done."


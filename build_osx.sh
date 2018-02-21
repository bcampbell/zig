#!/bin/bash

set -e

bundle=osx/zig.app

extralibs="libpng12.0.dylib"

mkdir -p $bundle
mkdir -p $bundle/Contents
mkdir -p $bundle/Contents/MacOS
mkdir -p $bundle/Contents/Resources
mkdir -p $bundle/Contents/Frameworks

make clean depend all OSX=1 RELEASE=1
cp zig $bundle/Contents/MacOS
cp osx/Info.plist $bundle/Contents/

# copy the extra libs into the bundle and patch them up
for e in $extralibs
do
cp "/usr/local/lib/$e" "$bundle/Contents/Frameworks/"

install_name_tool \
    -id @executable_path/../Frameworks/$e \
    $bundle/Contents/Frameworks/$e
done

cp -r /Library/Frameworks/SDL.framework $bundle/Contents/Frameworks/
cp -r /Library/Frameworks/SDL_mixer.framework $bundle/Contents/Frameworks/


# TODO check versionnumber in Info.plist


# add in the data files
for f in $datafiles
do
    cp data/$f $bundle/Contents/Resources/
done



# fix up the dylib refs in the executable

for e in $extralibs
do
    install_name_tool \
        -change /usr/local/lib/$e \
        @executable_path/../Frameworks/$e \
        $bundle/Contents/MacOS/zig
done


hdiutil create -ov -fs HFS+ -srcfolder $bundle osx/zig.dmg


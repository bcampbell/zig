#/bin/bash
set -e

# windows build checklist:
#
# 32-bit build?
# runs from explorer?
# cheats turned off?
# binaries stripped?
# soak test (with and without -flatout)
# check sounds sound right


#dlls="libpng16-16.dll SDL2.dll SDL2_mixer.dll libwinpthread-1.dll"


#for our custom sdl_mixer build
SITE=~/site32
export PKG_CONFIG_PATH=$SITE/lib/pkgconfig:$PKG_CONFIG_PATH

# BUILD zig

t=`mktemp -d`
dest=$t/Zig
mkdir $dest
echo $dest


t=`mktemp -d`
dest=$t/Zig
mkdir $dest
echo $dest

pushd src >/dev/null
make clean
make -j4 release
strip zig.exe
cp zig.exe $dest/
cp -r data $dest/
popd >/dev/null

#cp `which $dlls` $dest/


# zip it
cd $t
z=zig_windows_$(date +%Y-%d-%m).zip
zip -r $z $(basename $dest)
echo done


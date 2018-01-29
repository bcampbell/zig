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


dest=`mktemp -d`
echo $dest

pushd src >/dev/null
make clean
make -j4 release
strip zig.exe
cp zig.exe $dest/
cp -r data $dest/
popd >/dev/null

#cp `which $dlls` $dest/


echo $dest




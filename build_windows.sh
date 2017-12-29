#/bin/bash
set -e

dlls="libpng16-16.dll SDL2.dll SDL2_mixer.dll libwinpthread-1.dll"


dest=`mktemp -d`
echo $dest

pushd src >/dev/null
make clean
make -j4
cp zig.exe $dest/
cp -r data $dest/
popd >/dev/null

cp `which $dlls` $dest/


echo $dest




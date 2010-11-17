#!/bin/sh
set -e

ISCC="c:/Program Files/Inno Setup 5/iscc.exe"
datestring=`date +"%Y%m%d"`

# demo version
#outname=`date +"zigdemo-setup$datestring.exe"`

make clean all WIN32=1 RELEASE=1 CRIPPLED=1
"$ISCC" zigdemo.iss
mv "Output/setup.exe" "Output/zigdemo-setup.exe"


# full version

make clean all WIN32=1 RELEASE=1
"$ISCC" zig.iss
mv "Output/setup.exe" "Output/zig-setup.exe"

echo "done."


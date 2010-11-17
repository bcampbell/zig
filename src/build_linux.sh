#!/bin/bash
set -e


function prepdir()
{
	out=$1

	mkdir -p $out
	mkdir -p $out/data

	cp data/*.png data/*.raw $out/data/
	cp levels.txt $out/
}


# demo version
echo demo...
demoout=/tmp/zigdemo
prepdir $demoout 

make clean all RELEASE=1 CRIPPLED=1 >/dev/null
cp zig $demoout/zigdemo

pushd /tmp >/dev/null
tar -zcvf zigdemo.tar.gz zigdemo
popd >/dev/null


# full version
echo full...
fullout=/tmp/zig
prepdir $fullout

make clean all RELEASE=1 >/dev/null
cp zig $fullout/zig

pushd /tmp >/dev/null
tar -zcvf zig.tar.gz zig
popd >/dev/null

echo "done."


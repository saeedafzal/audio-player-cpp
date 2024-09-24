#!/usr/bin/env bash

set -euo pipefail

##
# Check if raylib and raygui already exist
##
if [ -f raylib/include/raylib.h ] && [ -f raylib/lib/libraylib.a ] && [ -f raygui/include/raygui.h ]; then
    echo The raylib and raygui library seems to exist.
    exit 0
fi

##
# Cleanup
##
rm -rf r raylib

##
# Get and build raylib
##
echo Cloning raylib repository.
git clone --depth=1 --recursive https://github.com/raysan5/raylib.git r

echo Building raylib.
cd r/src
make -j20

cd ../..
mkdir -p raylib/include raylib/lib
mv r/src/raylib.h r/src/raymath.h r/src/rlgl.h raylib/include/.
mv r/src/libraylib.a raylib/lib/.
rm -rf r

echo Built raylib!

##
# Get raygui (fork)
##
echo Cloning raygui fork.
git clone --depth=1 --recursive https://github.com/saeedafzal/raygui.git r

mkdir -p raygui/include
mv r/src/raygui.h raygui/include/.
rm -rf r

echo Moved raygui.h to raylib.
echo
echo
echo Done.

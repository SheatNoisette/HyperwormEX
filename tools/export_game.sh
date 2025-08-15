#!/bin/sh

set -e

GAME_NAME="HYPERWORM-EX"

# make sure build folder exists
mkdir -p build

(rm -rf build/* && cd build && cmake -DCMAKE_BUILD_TYPE=Release -DBACKEND=X11 .. && make)

echo
echo " * Stripping..."
strip -s build/PotetreVoxels
sstrip -z build/PotetreVoxels

echo " * Compressing..."
# xz --keep -e9 \
# 	--lzma2=lc=1,pb=0,lp=0,mf=bt4,mode=normal \
#     --stdout build/PotetreVoxels > build/game_compressed.xz
python tools/autovndh.py --xz --lzma -v -n \
    build/PotetreVoxels build/game_compressed.xz

echo
echo " * Adding shell script..."
rm -f build/$GAME_NAME
cat tools/launcher.sh build/game_compressed.xz > build/$GAME_NAME
chmod +x build/$GAME_NAME
rm -f build/game_compressed.xz

echo " * Zipping..."
rm -f $GAME_NAME-linux.zip
zip $GAME_NAME-linux.zip build/$GAME_NAME GAME_README.md

echo
echo -n "Final size : "
wc -c < build/$GAME_NAME

echo
echo "Enjoy $GAME_NAME!"

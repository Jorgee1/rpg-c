#!/bin/bash

NAME="rpg"

COMPILER=clang
ext=".c"

SDL_PATH=/usr/include/SDL2
SRC_PATH=src
OBJ_PATH=objs
BIN_PATH=bin
ASSET_PATH=asset

FLAGS="-lSDL2 -lSDL2_ttf -lSDL2_image -no-pie"
FILES=$(find $SRC_PATH/ -type f -name '*'$ext)

if [ ! -d $OBJ_PATH ]
then
    mkdir $OBJ_PATH
fi

for file in $FILES
do
    echo "Building $file"
    OBJ=$OBJ_PATH/$(basename ${file} $ext).o
    $COMPILER -c $file -o $OBJ -I$SDL_PATH -I$SRC_PATH -I$USER_INCLUDE/sdl-helper
    OBJS+="$OBJ "
done

echo "Building game"
if [ ! -d $BIN_PATH ]
then
    mkdir $BIN_PATH
else
    rm -rf $BIN_PATH
    mkdir $BIN_PATH
fi

$COMPILER $OBJS $FLAGS -o $BIN_PATH/$NAME $USER_LIB/sdl-helper.a

if [ -d $ASSET_PATH ]
then
    cp -r $ASSET_PATH $BIN_PATH
fi

echo "Done"
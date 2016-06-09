#!/usr/bin/env bash

rm -r build
mkdir build
cd build
cp -r ../../out/distr/* .
cp ../../../urho3D/build/lib/libUrho3D.a .
cmake ..
make -j2
rm *.*
rm -r CMakeFiles
rm Makefile
cp ../../../../../temp/run.exe .


!#/usr/bin/env bash

cd ../../urho3D/build
rm -r *
cd ..
./cmake_generic.sh build -DURHO3D_SAMPLES=0
cd build
make -j2


#!/usr/bin/env bash

cd build
rm -r *
cmake ..
make -j2
cd ..
cp -r ~/Urho3D/urho3D/bin/CoreData build
cp -r ~/Urho3D/urho3D/bin/Data build

#!/usr/bin/env bash

build
rm -r *
cp ../../../urho3D/build/lib/libUrho3D.a .
cp -r ../../../urho3D/bin/CoreData .
cp -r ../../../urho3D/bin/Data .
cmake ..
make -j2
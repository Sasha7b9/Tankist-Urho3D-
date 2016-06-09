#!/usr/bin/env bash

cd Tankist
git pull
cd ..
./stopTankist.sh
./buildTankist.sh
./runTankist.sh
./runUploader.sh




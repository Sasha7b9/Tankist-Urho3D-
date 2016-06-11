#!/usr/bin/env bash

cd Tankist
git pull
cd ..
cp ../../temp/TankistLauncher.exe /media/web/tankistwat/tankistwat/media/distr
./stopTankist.sh
./buildTankist.sh
./runTankist.sh




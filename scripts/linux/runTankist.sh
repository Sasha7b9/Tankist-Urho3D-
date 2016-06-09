#!/usr/bin/env bash

./stopTankist.sh
cd Tankist/server/build
./Tankist -server -port:30000 &

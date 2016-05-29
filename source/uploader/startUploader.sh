#!/usr/bin/env bash

killall -9 uploader.py
sudo fuser -k 1235/tcp

#python3 ./uploader.py
nohup python3 ./uploader.py 1>/dev/null 2>&1 &

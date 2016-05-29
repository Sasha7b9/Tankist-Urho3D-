#!/usr/bin/env bash

killall -9 uploader.py
nohup python3 ./uploader.py 1>/dev/null 2>&1 &

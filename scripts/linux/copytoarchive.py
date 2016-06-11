#!/usr/bin/env python

import os
import datetime

# Create archiv

date = datetime.datetime.now()

name = 'Tankist' + date.strftime("_%Y_%m_%d_%H_%M_%S") + '.tar.gz'

fullName = '/tmp/' + name


os.system('sudo tar -cvzf ' + fullName + ' /home/sasha7b9/Games/Tankist > /dev/null')


# Create script

script = open("copy.sh", "w")

script.write("#!/bin/sh\n")
script.write("open wdc-backup10.ispsystem.net\n")
script.write("user ftp4520838 c2677i3xAnDj\n")
script.write("binary\n")
script.write("put " + fullName + " /Archiv/" + name + "\n")
script.write("close\n")
script.write("quit\n")

script.close()

os.system("sudo chmod +x copy.sh")

# Execute script - copy archiv to ftp

command = "sudo cat copy.sh | ftp -n -v "

os.system(command)

# Delete temp files

os.system("sudo rm copy.sh")
os.system("sudo rm " + fullName)

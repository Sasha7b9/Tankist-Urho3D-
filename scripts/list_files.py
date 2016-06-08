#!/usr/bin/python3

import os
import sys
import binascii, zlib, struct

def findFiles(path):
    find_files = []
    for root, dirs, files in os.walk(path):
        find_files += [os.path.join(root, name) for name in files]
    return find_files

def CRC32_from_file(fileName):
    prev = 0
    for eachLine in open(fileName, "rb"):
        prev = zlib.crc32(eachLine, prev)
    return "%X"%(prev & 0xFFFFFFFF)


files = findFiles('.')

writeFile = open('files.txt', 'w')

for file in files:
    if file != '.\\files.txt' and file != './files.txt':
        text =  file + ' ' + CRC32_from_file(file) + ' ' + str(os.path.getsize(file))
        print(text)
        writeFile.write(text[2:] + '\n')

writeFile.close()
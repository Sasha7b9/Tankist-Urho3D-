import os
from struct import *

def NumberFromBytes(position, num):
    retValue = 0
    for pos in range (num - 1):
        retValue += data[position + pos] << (pos * 8)
    return str(retValue)

def StringFromBytes(position, num):
    retValue = ""
    for pos in range (num):
        retValue += chr(data[position + pos])
    return retValue

input = open("chinook.wav", "rb")
data = input.read()

print ("chunkId - " + StringFromBytes(0, 4))
print ("chunkSize - " + NumberFromBytes(4, 4))
print ("format - " + StringFromBytes(8, 4))
print ("subchunk1Id - " + StringFromBytes(12, 4))
print ("subchunk1Size - " + NumberFromBytes(16, 4))
print ("audioFormat - " + NumberFromBytes(20, 2))
print ("numChannels - " + NumberFromBytes(22, 2))
print ("sampleRate - " + NumberFromBytes(24, 4))
print ("byteRage - " + NumberFromBytes(28, 4))
print ("blockAlign - " + NumberFromBytes(32, 2))
print ("bitsPerSample - " + NumberFromBytes(34, 2))
print ("subchunk2Id - " + StringFromBytes(36, 4))
print ("subchunk2Size - " + NumberFromBytes(40, 4))


input.close()

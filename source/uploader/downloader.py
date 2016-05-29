import socket
import sys
import time

sock = socket.socket()
sock.connect(('188.120.239.61', 1235))

sock.send(bytes('version', 'UTF-8'))

data = sock.recv(1024)

print(data.decode('UTF-8'))

sock.send(bytes('get_size', 'UTF-8'))

data = sock.recv(1024)

size = int(data.decode('UTF-8'))

print(size)

received_bytes = 0

file = open('out.exe', 'wb')

sock.send(bytes('get_file', 'UTF-8'))

percents = 0;

startTime = time.time()

while received_bytes < size:
    data = sock.recv(1024)
    received_bytes += len(data)
    file.write(data)

    nowPercents = (int)(received_bytes / size * 100.0)

    if nowPercents != percents:
        curTime = time.time();
        percents = nowPercents
        speed = (curTime - startTime) / percents
        print(str(percents) + ' time = ' + str((int)(curTime - startTime)) + ' elapsed = ' + str((int)(speed * 100 - (curTime - startTime))))

file.close()

sock.close()

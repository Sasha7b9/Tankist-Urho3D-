import socket
import sys
import time
import os

sock = socket.socket()
sock.connect(('localhost', 1235))


# Get size files.txt

sock.send(bytes('get_list_files_size', 'UTF-8'))

data = sock.recv(1024)

size = int(data.decode('UTF-8'))


# Get files.txt

file = open('files_new.txt', 'wb')

sock.send(bytes('get_list_files', 'UTF-8'))

data = sock.recv(size);

file.write(data)

file.close()


# Get files

file = open('files_new.txt', 'r')

lines = file.readlines()

file.close()

for line in lines:

    l = line.split()
    l.reverse()

    size = (int)(l[0])

    name = l[2]

    if l.__len__() > 3:             # for spaces in names files
        name = l[3] + ' ' + l[2]

    print(name)

    cort = os.path.split(name)
    if cort[0] != '.':
        os.makedirs(cort[0], 0o777, True)
        print('make dir ' + cort[0])

    sock.send(bytes('get_file ' + name, 'UTF-8'))
    data = sock.recv(size)
    file = open(name, 'wb')
    file.write(data)
    file.close()


'''
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

'''

sock.close()

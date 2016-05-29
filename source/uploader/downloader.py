import socket
import sys

sock = socket.socket()
sock.connect(('localhost', 1235))

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

while received_bytes < size:
    data = sock.recv(1024)
    received_bytes += len(data)
    file.write(data)
    print(received_bytes / size * 100.0)

file.close()

sock.close()

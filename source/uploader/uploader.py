#!/usr/bin/python3

import socket
import sys
from _thread import *
from os import path

HOST = ''
PORT = 1235
PATH = '../../out/distr'

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

print('enter')

try:
    sock.bind((HOST, PORT))
except socket.error as msg:
    sys.exit()

sock.listen(1000)

def clientthread(conn):
    try:
        while True:
            data = bytes('', 'UTF-8')

            while not data:
                data = conn.recv(1024)
            command = data.decode('UTF-8')

            l = command.split()

            if l[0] == 'get_file_size':
                name = l[1]
                if l.__len__() > 2:     # for spaces in file names
                    name += ' ' + l[2]
                p = path.join(PATH, name)
                size = path.getsize(p)
                conn.sendall(bytes(str(size), 'UTF-8'))
                continue

            if l[0] == 'get_file':
                name = l[1]
                if l.__len__() > 2:     # for spaces in file names
                    name += ' ' + l[2]
                p = path.join(PATH, name)
                f = open(p, 'rb')
                dat = f.read(path.getsize(p))
                conn.sendall(dat)
                continue

            if l[0] == 'close_connection':
                conn.close()
                break


    except socket.error as msg:
        return

while True:
    print('wait')
    conn, addr = sock.accept()
    start_new_thread(clientthread, (conn,))
    print('start connection')

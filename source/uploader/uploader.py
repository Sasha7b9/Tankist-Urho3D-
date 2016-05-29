#!/usr/bin/python3

import socket
import sys
from _thread import *
from os import path

HOST = ''
PORT = 1235

PATH = '/media/web/tankistwat/tankistwat/media/distr/'
PATH_VER = PATH + '\\version.info'
PATH_FILE = PATH + '\\TankistWaTclient.exe'

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    sock.bind((HOST, PORT))
except socket.error as msg:
    sys.exit()

sock.listen(1000)

def clientthread(conn):
    while True:
        data = bytes('', 'UTF-8')

        while not data:
            data = conn.recv(1024)
        command = data.decode('UTF-8')
        print(command)

        if command == 'version':
            print('send version')
            f = open(PATH_VER, 'r')
            ver = f.read()
            conn.sendall(bytes(ver, 'UTF-8'))
            continue

        if command == 'get_size':
            size = path.getsize(PATH_FILE)
            conn.sendall(bytes(str(size), 'UTF-8'))
            continue

        if command == 'get_file':
            f = open(PATH_FILE, 'rb')

            dat = f.read(1024)
            while len(dat) != 0:
                conn.sendall(dat)
                dat = f.read(1024)

            conn.close()
            break

    print('close connection')

while True:
    conn, addr = sock.accept()
    start_new_thread(clientthread, (conn,))
    print('start connection')
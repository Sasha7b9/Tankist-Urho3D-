#!/usr/bin/python3

import socket
import sys
from _thread import *
from os import path

HOST = ''
PORT = 1235

PATH = '/media/web/tankistwat/tankistwat/media/distr/'
PATH_VER = '../version.info'
PATH_FILE = PATH + 'TankistInstaller.exe'

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

            if command == 'get_list_files_size':
                size = path.getsize('../../out/distr/files.txt')
                conn.sendall(bytes(str(size), 'UTF-8'))
                continue

            if command == 'get_list_files':
                f = open('../../out/distr/files.txt', 'rb')
                dat = f.read(path.getsize('../../out/distr/files.txt'))
                conn.sendall(dat)
                f.close
                continue

            l = command.split()

            if l[0] == 'get_file':
                name = l[1]
                if l.__len__() > 2:     # for spaces in file names
                    name += ' ' + l[2]
                p = path.join('../../out/distr', name)
                f = open(p, 'rb')
                dat = f.read(path.getsize(p))
                conn.sendall(dat)
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


    except socket.error as msg:
        return

while True:
    print('wait')
    conn, addr = sock.accept()
    start_new_thread(clientthread, (conn,))
    print('start connection')

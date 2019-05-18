# -*- coding: utf-8 -*-
import socket
from os import system as run_bash_command
from sys import argv
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
ipAddress = str(argv[1])
port = int(argv[2])
serverNumber = int(argv[3])
folderToSearch = str(argv[4])


print("Server ", str(serverNumber), " is listening on IP: ", ipAddress + " port: ", (port))

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sock = socket.socket()
sock.bind((ipAddress, port))
sock.listen(1)
conn, addr = sock.accept()
pattern = conn.recv(1024).decode("utf-8")
bashCommand = "grep -rnw \'" + folderToSearch + "\' -e \'" + pattern + "\' > tmpfile.txt"
run_bash_command(bashCommand)


#-------------------------------------------------------------------------------
f = open("tmpfile.txt", "rb")
l = f.read(1024)
while(l):
    conn.send(l)
    l = f.read(1024)

sock.close()

print("Server ", str(serverNumber), " sent his answer to the main server")
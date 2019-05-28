# -*- coding: utf-8 -*-
import socket
from os import system
from sys import argv

# Парсим адрес, порт, номер и директорию поиска главного сервера
ipAddress = str(argv[1])
port = int(argv[2])
serverNumber = int(argv[3])
folderToSearch = str(argv[4])


print "Сервер ", str(serverNumber), " запущен IP: ", ipAddress + " port: ", (port)

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sock = socket.socket() # создаём сокет
sock.bind((ipAddress, port)) # настраиваем адрес
sock.listen(1) # прослушиваем порт
conn, addr = sock.accept() # принимаем соединение
pattern = conn.recv(1024).decode("utf-8") # принимаем запрос
bashCommand = "grep -rnw \'" + folderToSearch + "\' -e \'" + pattern + "\' > tmpfile.txt"
os.system(bashCommand) # выполняем команду поиска


#-------------------------------------------------------------------------------
f = open("tmpfile.txt", "rb")
line = f.read(1024)
while(line):
    conn.send(line)
    line = f.read(1024)
sock.close()  # освобождаем сокет

print "Сервер ", str(serverNumber), " отправил ответ на основной сервер"
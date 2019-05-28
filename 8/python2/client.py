# -*- coding: utf-8 -*-
import socket
from sys import argv

# Парсим адрес и порт главного сервера
ipAddress = str(argv[1])
port = int(argv[2])

pattern = "abc"
 
conn = socket.socket() # создаём сокет
conn.connect((ipAddress, port)) # присоединяемся к основному серверу
conn.send(pattern.encode("utf-8")) # отправляем запрос на основной сервер


# Принимаем ответ от главного сервера
line = conn.recv(1024).decode("utf-8")
while (line):
    print(line)
    line = conn.recv(1024).decode("utf-8")

conn.close() # освобождаем сокет
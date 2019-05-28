# -*- coding: utf-8 -*-
import socket
from threading import Thread
from sys import argv

# Парсим адрес и порт главного сервера
ipAddress = str(argv[1])
port = int(argv[2])


# Поток присоединённый к одному из серверов
def findData(_ip, _port, _pattern, f):
    _ip = str(_ip)
    conn = socket.socket()
    print("Trying connect to ", _ip, _port)
    conn.connect((_ip, _port))
    conn.send(pattern)
    l = conn.recv(1024)
    while (l):
        f.write(l)
        l = conn.recv(1024)
    conn.close()
    

#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
sock = socket.socket() # создаём сокет
sock.bind((ipAddress, port)) # настраиваем адрес
sock.listen(1) # прослушиваем порт
conn_to_client, addr = sock.accept() # принимаем соединение
pattern = conn_to_client.recv(1024) # принимаем запрос


#-------------------------------------------------------------------------------

data=[]
threads=[]
f = open("answer.txt", "wb")
with open("servers.txt", 'r') as f_s:
    for line in f_s:
        data.append([ str(x) for x in line.split()])
    for i in data:
        t = Thread(target=findData, args=(str(i[0]), int(i[1]), pattern, f))
        t.start()
        threads.append(t)
        print(i)


# Закрываем дочерние потоки
for i in threads:
    i.join()

f.close()

f = open("answer.txt", "rb")
line = f.read(1024)
while(line):
    conn_to_client.send(line)
    line = f.read(1024)
f.close()
sock.close() # освобождаем сокет
print("Основной сервер отправил ответ клиенту")



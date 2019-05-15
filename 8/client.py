import socket
from sys import argv

ipAddress = str(argv[1])
port = int(argv[2])

pattern = "abc"

conn = socket.socket()
conn.connect((ipAddress, port))
conn.send(pattern.encode("utf-8"))

#-------------------------------------------------------------------------------
f = open("SentFile.txt", "w")
l = conn.recv(1024).decode("utf-8")
while (l):
    print(l)
    f.write(l)
    l = conn.recv(1024).decode("utf-8")
f.close()

conn.close()
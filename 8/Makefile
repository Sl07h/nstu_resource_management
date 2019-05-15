# Makefile for lab #8
 run:
	sleep 10s
	python3 server.py 127.0.0.1 8080 1 folder/file1 &
	sleep 1s
	python3 server.py 127.0.0.2 8080 2 folder/file2 &
	sleep 1s
	python3 server.py 127.0.0.3 8080 3 folder/file3 &
	sleep 1s
	python3 mainServer.py 127.0.0.255 8080 &
	sleep 1s
	python3 client.py 127.0.0.255 8080

 clean:
	lsof -ti :8080 | xargs --no-run-if-empty kill -9
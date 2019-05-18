# Makefile for lab #8
 run:
	sleep 10s
	python server.py 127.0.0.1 8080 1 folder &
	sleep 1s
	python server.py 127.0.0.2 8080 2 ../../lab5 &
	sleep 1s
	python server.py 127.0.0.3 8080 3 ~ &
	sleep 1s
	python mainServer.py 127.0.0.255 8080 &
	sleep 1s
	python client.py 127.0.0.255 8080

 clean:
	lsof -ti :8080 | xargs --no-run-if-empty kill -9
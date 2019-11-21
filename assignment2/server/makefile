all: server

server: server.o
	g++ server.o -o server
	rm server.o

server.o: server.cpp
	g++ -c server.cpp

clean:
	rm server
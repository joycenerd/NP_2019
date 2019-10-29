all: gethostbyname server

gethostbyname: gethostbyname.o
	g++ -o gethostbyname gethostbyname.o
	rm gethostbyname.o

gethostbyname.o: gethostbyname.cpp
	g++ -c gethostbyname.cpp

server: server.o
	g++ -o server server.o
	rm server.o

server.o: server.cpp
	g++ -c server.cpp

clean:
	rm gethostbyname
	rm server
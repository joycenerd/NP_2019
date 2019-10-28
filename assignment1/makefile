all: gethostbyname server

gethostbyname: gethostbyname.o
	g++ -o gethostbyname gethostbyname.o
	rm gethostbyname.o

gethostbyname.o: gethostbyname.cpp
	g++ -c gethostbyname.cpp

server: HttpGetPostMethod.o server.o
	g++ -o server server.o HttpGetPostMethod.o
	rm HttpGetPostMethod.o
	rm server.o

HttpGetPostMethod.o: HttpGetPostMethod.cpp
	g++ -c HttpGetPostMethod.cpp

clean:
	rm gethostbyname
	rm server
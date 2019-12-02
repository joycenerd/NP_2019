all: client

client: client.o
	g++ client.o -o client
	rm client.o

client.o: client.cpp
	g++ -c client.cpp

clean: client
	rm client
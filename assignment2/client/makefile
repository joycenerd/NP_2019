all: client

client: client.o
	gcc client.o -o client
	rm client.o

client.o: client.cpp
	gcc -c client.cpp

clean: client
	rm client
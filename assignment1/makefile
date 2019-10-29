all: gethostbyname

gethostbyname: gethostbyname.o
	g++ -o gethostbyname gethostbyname.o
	rm gethostbyname.o

gethostbyname.o: gethostbyname.cpp
	g++ -c gethostbyname.cpp

clean:
	rm gethostbyname
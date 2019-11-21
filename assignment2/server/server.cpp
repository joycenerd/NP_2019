#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#define MAX_CONN_LIMIT 30
using namespace std;

int main(int argc, char *argv[]) {

    int maxClient=MAX_CONN_LIMIT;
    int clientSocket[MAX_CONN_LIMIT],serverSocket,addressLength,maxSocketfd,socketfd;
    struct sockaddr_in address;
    fd_set readfdSet;

    // intialize all the client sockets
    for(int i=0;i<maxClient;i++) {
        clientSocket[i]=0;
    }

    // create server socket
    serverSocket=socket(AF_INET,SOCK_STREAM,0);
    if(serverSocket==0) {
        perror("socker failes");
        exit(EXIT_FAILURE);
    }
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(8000);

    // bind the socket to locathost port 8000
    if(bind(serverSocket, (struct sockaddr *)&address, sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // specify the maximum pending connections of server socket to 10
    if(listen(serverSocket,10)<0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connections
    addressLength=sizeof(address);
    puts("Waiting for connections...\n");

    while(true) {

        // initialize socket sets
        FD_ZERO(&readfdSet);
        FD_SET(serverSocket,&readfdSet);
        maxSocketfd=serverSocket;

        // add child sockets to the set
        for(int i=0;i<maxClient;i++) {
            socketfd=clientSocket[i];
            if(socketfd>0) {
                FD_SET(socketfd,&readfdSet);
            }
            if(socketfd>maxSocketfd) {
                maxSocketfd=socketfd;
            }
        }

    }
    
    return 0;
}
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#define MAX_LENGTH 8192
#define PORT 8000
using namespace std;


void func(int socketfd) {
    char buffer[MAX_LENGTH+1];
    for(;;) {
        bzero(buffer,sizeof(buffer));
        fgets(buffer,MAX_LENGTH,stdin);
        write(socketfd,buffer,sizeof(buffer));
        bzero(buffer,sizeof(buffer));
        read(socketfd,buffer,sizeof(buffer));
        printf("%s",buffer);
    }
    return;
}


int main() 
{ 
    int socketfd, connfd; 
    struct sockaddr_in serverAddress, client; 
  
    // socket create and veSrification 
    socketfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&serverAddress, sizeof(serverAddress)); 
  
    // assign IP, PORT 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
    serverAddress.sin_port = htons(PORT); 
  
    // connect the client socket to server socket
    char buffer[MAX_LENGTH+1];
    if (connect(socketfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else {
        printf("connected to the server..\n");
        int readMessage=read(socketfd,buffer,sizeof(buffer));
        buffer[readMessage]='\0';
        printf("%s",buffer);
    } 
  
    // function for chat 
    func(socketfd); 
  
    // close the socket 
    close(socketfd); 
} 
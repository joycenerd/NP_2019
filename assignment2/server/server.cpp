#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <iostream>
#define MAX_CONN_LIMIT 30
#define MAX_BUFF_SIZE 8192
#define PORT 8000
using namespace std;

typedef struct socketinfo{
  int fd;
  string ip;
  int port;
}SocketInfo;

typedef struct clientList {
  string username;
  string password;
  bool isOnline;
  int pos;
}ClientList;

ClientList clientInfo[7];

void init(SocketInfo activeClient[],int sizes){
  for(int i=0;i<sizes;i++) {
    activeClient[i].fd=0;
    activeClient[i].ip="\0";
    activeClient[i].port=0;
  }
  return;
}

// initialize all the client info
void initClientInfo() {
  clientInfo[0].username="hank"; clientInfo[0].password="1234"; clientInfo[0].isOnline=false; clientInfo[0].pos=-1;
  clientInfo[1].username="henry"; clientInfo[1].password="1234"; clientInfo[1].isOnline=false; clientInfo[1].pos=-1;
  clientInfo[2].username="joyce"; clientInfo[2].password="1234"; clientInfo[2].isOnline=false; clientInfo[2].pos=-1;
  clientInfo[3].username="edward"; clientInfo[3].password="1234"; clientInfo[3].isOnline=false; clientInfo[3].pos=-1;
  clientInfo[4].username="patty"; clientInfo[4].password="1234"; clientInfo[4].isOnline=false; clientInfo[4].pos=-1;
  clientInfo[5].username="helen"; clientInfo[5].password="1234"; clientInfo[5].isOnline=false; clientInfo[5].pos=-1;
}

bool searchUsername(char *buffer) {
  for(int i=0;i<6;i++) {
    string name=buffer;
    if(buffer==clientInfo[i].username && clientInfo[i].isOnline==false) return true;
  }
  return false;
}

string login(int socketfd) {
  char buffer[MAX_BUFF_SIZE + 1];
  bool isValidUsername;
  int readMessage;
  string username,greetingToUser;

  bzero(buffer,sizeof(buffer));
  readMessage=read(socketfd,buffer,MAX_BUFF_SIZE);
  buffer[readMessage]='\0';
  isValidUsername=searchUsername(buffer);
  while(!isValidUsername) {
    bzero(buffer,sizeof(buffer));
    send(socketfd,"Enter your username: ",strlen("Enter your username: "),0);
    read(socketfd,buffer,MAX_BUFF_SIZE);
    buffer[readMessage]='\0';
    isValidUsername=searchUsername(buffer);
  }
  username=buffer;

  bzero(buffer,sizeof(buffer));
  send(socketfd,"Enter your password: ",strlen("Enter your password: "),0);
  readMessage=read(socketfd,buffer,MAX_BUFF_SIZE);
  buffer[readMessage]='\0';
  while(strncmp(buffer,"1234",4)!=0) {
    bzero(buffer,sizeof(buffer));
    send(socketfd,"Enter your password: ",strlen("Enter your password: "),0);
    readMessage=read(socketfd,buffer,MAX_BUFF_SIZE);
    buffer[readMessage]='\0';
  }

  greetingToUser="Hello, "+username+"!\n";
  send(socketfd,greetingToUser.c_str(),strlen(greetingToUser.c_str()),0);

  return username;
}


void markOnline(int pos,string username) {
  for(int i=0;i<6;i++) {
    if(username==clientInfo[i].username) {
      clientInfo[i].isOnline=true;
      clientInfo[i].pos=pos;
    }
  }
}


int main(int argc, char *argv[]) {
  int maxClient = MAX_CONN_LIMIT;
  int clientSocket[MAX_CONN_LIMIT], serverSocket, addressLength, maxSocketfd,
      socketfd, activity, newSocket;
  int readMessage,usedSpace,opponentfd;
  struct sockaddr_in address;
  fd_set readfdSet;
  char buffer[MAX_BUFF_SIZE + 1];
  SocketInfo activeClient[maxClient];
  string username;

  // initialize all the client info
  initClientInfo();

  // intialize all the client sockets
  for (int i = 0; i < maxClient; i++) {
    clientSocket[i] = 0;
  }

  // create server socket
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == 0) {
    perror("socker failes");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // bind the socket to locathost port 8000
  if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // specify the maximum pending connections of server socket to 10
  if (listen(serverSocket, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // listen for incoming connections
  addressLength = sizeof(address);
  puts("Waiting for connections...\n");
  const char *message = "Hello World! It's server sending messages\nEnter your username: ";

  while (true) {

    // initialize socket sets
    FD_ZERO(&readfdSet);
    FD_SET(serverSocket, &readfdSet);
    maxSocketfd = serverSocket;

    // add child sockets to the set
    for (int i = 0; i < maxClient; i++) {
      socketfd = clientSocket[i];
      if (socketfd > 0) {
        FD_SET(socketfd, &readfdSet);
      }
      if (socketfd > maxSocketfd) {
        maxSocketfd = socketfd;
      }
    }

    // wait for connection, timeout=NULL -> wait indefinitely
    activity = select(maxSocketfd + 1, &readfdSet, NULL, NULL, NULL);

    // accept incoming connection
    if (FD_ISSET(serverSocket, &readfdSet)) {
      newSocket = accept(serverSocket, (struct sockaddr *)&address,
                         (socklen_t *)&addressLength);
      if (newSocket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
      printf("New connection\nsocket fd is: %d\nip is: %s\nport: %d\n",
             newSocket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

      // send message to new connection (client)
      if (send(newSocket, message, strlen(message), 0) != strlen(message)) {
        perror("send");
      }
      puts("Welcome message send successfully");

      username=login(newSocket);

      // add new socket to array of sockets
      for (int i = 0; i < maxClient; i++) {
        if (clientSocket[i] == 0) {
          clientSocket[i] = newSocket;
          printf("Adding to list of sockets as %d\n\n", i);
          markOnline(i,username);
          break;
        }
      }
    }
    

    // some IO operation on some other socket
    for (int i = 0; i < maxClient; i++) {
      socketfd = clientSocket[i];

      // see if the socket is closing
      if (FD_ISSET(socketfd, &readfdSet)) {
        readMessage = read(socketfd, buffer, MAX_BUFF_SIZE);
        if (readMessage == 0) {
          getpeername(socketfd, (struct sockaddr *)&address,
                      (socklen_t *)&addressLength);
          printf("Host disconnected\nfd: %d\nip: %s\nport: %d\n\n", socketfd,
                 inet_ntoa(address.sin_addr), ntohs(address.sin_port));
          close(socketfd);
          clientSocket[i] = 0;
        }
        // echo back the same message that came in
        else {
          buffer[readMessage] = '\0';
          // client ask to list all the other client
        }
      }
    }
  }
  return 0;
}
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
#define PORT 8002
using namespace std;

typedef struct socketinfo{
  int fd;
  string ip;
  int port;
}SocketInfo;

// List all the registered clients
typedef struct clientList {
  string username;
  string password;
  bool isOnline;
  int fd;
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
  clientInfo[0].username="hank"; clientInfo[0].password="1234"; clientInfo[0].isOnline=false; clientInfo[0].fd=-1;
  clientInfo[1].username="henry"; clientInfo[1].password="1234"; clientInfo[1].isOnline=false; clientInfo[1].fd=-1;
  clientInfo[2].username="joyce"; clientInfo[2].password="1234"; clientInfo[2].isOnline=false; clientInfo[2].fd=-1;
  clientInfo[3].username="edward"; clientInfo[3].password="1234"; clientInfo[3].isOnline=false; clientInfo[3].fd=-1;
  clientInfo[4].username="patty"; clientInfo[4].password="1234"; clientInfo[4].isOnline=false; clientInfo[4].fd=-1;
  clientInfo[5].username="helen"; clientInfo[5].password="1234"; clientInfo[5].isOnline=false; clientInfo[5].fd=-1;
}

// check if the new coming user exist and not online right now
bool searchUsername(char *buffer) {
  for(int i=0;i<6;i++) {
    string name=buffer;
    if(buffer==clientInfo[i].username && clientInfo[i].isOnline==false) return true;
  }
  return false;
}

// client login
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

void list(int socketfd) {
  string message="";
  message+="Current online users:\n";
  for(int i=0;i<6;i++) {
    if(clientInfo[i].isOnline==true) message+=("  "+clientInfo[i].username+"\n");
  }
  send(socketfd,message.c_str(),strlen(message.c_str()),0);
  return;
}

// Mark the new lgoin user online
void markOnline(int pos,string username) {
  for(int i=0;i<6;i++) {
    if(username==clientInfo[i].username) {
      clientInfo[i].isOnline=true;
      clientInfo[i].fd=pos;
    }
  }
  return;
}

void markOffline(int pos) {
  for(int i=0;i<6;i++) {
    if(pos==clientInfo[i].fd) {
      clientInfo[i].isOnline=false;
      clientInfo[i].fd=-1;
    }
  }
  return;
}

int checkOnline(string username) {
  for(int i=0;i<6;i++) {
    if(username==clientInfo[i].username) {
      return clientInfo[i].fd;
    }
  }
  return -1;
}

string getUsername(int socketfd) {
  for(int i=0;i<6;i++) {
    if(socketfd==clientInfo[i].fd) {
      return clientInfo[i].username;
    }
  }
  return "";
}

void inGame(int fd1,int fd2) {
  string user1=getUsername(fd1);
  string user2=getUsername(fd2);
  string board=user1+":O "+user2+":X\n ----- ----- -----\n|     |     |     |\n|  1  |  2  |  3  |\n|     |     |     |\n ----- ----- -----\n|     |     |     |\n|  4  |  5  |  6  |\n|     |     |     |\n ----- ----- -----\n|     |     |     |\n|  7  |  8  |  9  |\n|     |     |     |\n ----- ----- -----\n";
  send(fd1,board.c_str(),strlen(board.c_str()),0);
  send(fd2,board.c_str(),strlen(board.c_str()),0);
}
 
void challenge(int socketfd) {
  char buffer[MAX_BUFF_SIZE+1];
  int readMessage,pos;
  string username,message;

  send(socketfd,"Who do you want to challenge: ",strlen("Who do you want to challenge: "),0);
  readMessage=read(socketfd,buffer,MAX_BUFF_SIZE);
  buffer[readMessage]='\0';
  username=buffer;
  pos=checkOnline(username);
  if(pos==-1) {
    send(socketfd,"Unsuccessful\n",strlen("Unsuccessful\n"),0);
    return;
  }
  message=getUsername(socketfd)+" wants to challenge you [y/n]: ";
  send(pos,message.c_str(),strlen(message.c_str()),0);
  bzero(buffer,sizeof(buffer));
  readMessage=read(pos,buffer,MAX_BUFF_SIZE);
  buffer[readMessage]='\0';
  printf("%s\n",buffer);
  if(buffer[0]=='n') {
    message="";
    message=username+" rejected your invitation\n";
    send(socketfd,message.c_str(),strlen(message.c_str()),0);
    return;
  }
  inGame(socketfd,pos);
  return; 
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
      
      // New connection need to login
      username=login(newSocket);

      // add new socket to array of sockets
      for (int i = 0; i < maxClient; i++) {
        if (clientSocket[i] == 0) {
          clientSocket[i] = newSocket;
          printf("Adding to list of sockets as %d\n\n", i);
          markOnline(newSocket,username);
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
          markOffline(socketfd);
          close(socketfd);
          clientSocket[i] = 0;
        }
        // echo back the same message that came in
        else {
          buffer[readMessage] = '\0';
          if(strncmp(buffer,"list",strlen("list"))==0) {
            list(socketfd);
          }
          else if(strncmp(buffer,"challenge",strlen("challenge"))==0) {
            challenge(socketfd);
          }
        }
      }
    }
  }
  return 0;
}
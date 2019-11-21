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
#define MAX_CONN_LIMIT 30
#define MAX_BUFF_SIZE 8192
#define PORT 8000
using namespace std;

int main(int argc, char *argv[]) {

  int maxClient = MAX_CONN_LIMIT;
  int clientSocket[MAX_CONN_LIMIT], serverSocket, addressLength, maxSocketfd,
      socketfd, activity, newSocket;
  int readMessage;
  struct sockaddr_in address;
  fd_set readfdSet;
  char buffer[MAX_BUFF_SIZE + 1];

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
  const char *message = "Hello World! It's server sending messages\n";

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
      puts("Welcome message send successfully\n");

      // add new socket to array of sockets
      for (int i = 0; i < maxClient; i++) {
        if (clientSocket[i] == 0) {
          clientSocket[i] = newSocket;
          printf("Adding to list of sockets as %d\n", i);
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
          printf("Host disconnected\nip: %s\nport: %d\n",
                 inet_ntoa(address.sin_addr), ntohs(address.sin_port));
          close(socketfd);
          clientSocket[i] = 0;
        }
        // echo back the same message that came in
        else {
          buffer[readMessage] = '\0';
          send(socketfd, buffer, strlen(buffer), 0);
        }
      }
    }
  }
  return 0;
}
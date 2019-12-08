NP Assignment#2: Playing OX chess Using Network Programming
==

###### tags: `Network Projgramming`

## Part1: (Echo server version) Serverside

### Step1: Create the server socket and initialize client socket

1. Initialize all the client socket
2. Create server socket
3. Bind the socket to port 8000
4. Specify the mazimum pending connections of server socket to 10
5. listen for incoming connections

```cpp=
sock=socket(AF_INET,SOCK_STREAM,0);
address.sin_family=AF_INET;
address.sin_addr.s_addr=INADDR_ANY;
address.sin_port=htons(PORT);
bind(mysocket,(struct sockaddr *)&address,sizeof(address));
listen(mysocket,MAXIMUM_CONNECTIONS);
```

Opening a server socket as short: 1. define it's protocol, 2. connect port number 3. bind the socket with the protocol 4. listen and waiting for connection

References:

* [Socket Programming in C/C++: Handling multiple clients on server without multi threading](https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/)
* [Bind failed: Address already in use](https://stackoverflow.com/questions/15198834/bind-failed-address-already-in-use)

### Step2: Process the incoming connections

1. initialize socket descriptor set
2. add child (client) sockets to the set
3. wait for connection, timeout=NULL -> wait indefinitely

```cpp=
FD_ZERO(&fdset);
FD_SET(mysocket,&fdset);
activity=select(num_of_sockfd,&fdset,NULL,NULL,NULL);
```

Initialize the fdset by `FD_ZERO` and `FD_SET`. Also using `select()` instead of multi-thread for many processing many connections simultaneously. Inside `select()` set timeout to NULL to tell the server to wait indefinitely for new connections.

### Step3: Accept incoming connection

1. `accept()`
2. print new connection's information
3. send message to the new connection (client)
4. add new socket to array of sockets

```cpp=
sockfd=sccpet(mysocket,(struct sockaddr *)&address,(socklen_t *)&addressLength);
ip=inet_ntoa(address.sin_addr);
port=ntohs(address.sin_port);
send(sockfd,message,strlen(message),0);
```

Accepting a new connection is as simple as calling `accpet()`. `inet_ntoa`,`ntohs` can get the ip and port number of the incoming connections. After connected sending a message to the client to checkt the passway from server to client is functioning.

## Step4: IO operation on other sockets (not new connection) -> ECHO server

1. check if the socket is closing
2. echo back the same message that came in

```cpp=
FD_ISSET(sockfd,&fdset);
readMessage=read(sockfd,buffer,MAX_BUFF_SIZE);
getpeername(sockfd,(struct sockaddr *)&addressm(socklen_t *)&addressLength);
close(sockfd);
```

If the socket fd is part of the fdset then we will read the message from it. Since it's a echo server it will echo back the same message. If after reading the message and find out it is empty then we know the user probably enter terminal command so we close the connection.


## Part2: (Echo server version) Clientside

### Step1: Create client socket

1. socket creation and verification
2. assign IP and port

This step is basically same as creating server socket. **Note the port number you assigned here should be exactly the same as in the server**. In my case I set to 8000. Server and clients communicates through **port** that is why server and client should set to the same port, otherwise they can't communicate. You can think of it as a pipe.

References:
* [TCP Server-Client implementation in C](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)
* [exit](http://www.cplusplus.com/reference/cstdlib/exit/)
* [How to get telnet to localhost working?](https://askubuntu.com/questions/233464/how-to-get-telnet-to-localhost-working)
* [TCP echo client-server in C](https://gist.github.com/suyash/2488ff6996c98a8ee3a84fe3198a6f85)

### Step2: Connect to server

1. Connect the client socket to the server socket
2. if connect successful print out the welcome message from the server
3. **Echo server** part pass to a individual function
4. get the message from standard in wirte it to the pipe (socketfd) and read the return message from the pipe (from server) which should be the same message -> since our server is an echo server

The pipe mechanism is what I'm confused about in the beginning. Simple explanation is: client write -> server read -> server return the same message it just read by write -> client read -> client print

Refeences:
* [fgets](http://www.cplusplus.com/reference/cstdio/fgets/)
* [bzero()](https://www.mkssoftware.com/docs/man3/bzero.3.asp)
* [Removing trailing newline character from fgets() input](https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)

## Part3: Client login function

### Step1: Create and initialize a list of registered clients (server) 

1. Create a client list using array of structure
2. initialize the username, password, isOnline=false, position in the socket = -1

```cpp=
typedef struct clientList {
  string username;
  string password;
  bool isOnline;
  int fd;
}ClientList;
```

### Step2: New incoming connection to login (server)

1. Welcome message will also include the massage that the user should enter their username
2. Go to login function
3. if the user login successfully then add the new socket to array of sockets, change the status of the login user by modifying the client list

### Step3: Write a login function (server)
1. get the username input by the client
2. check if the username is valid and currently not online right now (one user can only login in one device at the same time)
3. if the username is invalid then keep asking the user to enter the username
4. if the user enter the correct username ask the user to enter it's password
5. if the user enter the password correctly then the user login successful, greeting message sent

### Step4: Mark the new login user online (server)

1. `isOnline=true`
2. `position=the index in the active client array` (easy to communicate with this client in the future)


## Part4: List all the users (server)

1. Detect if the user type **:list**
2. If `clientInfo[i].isOnline==true` then add the user to online user list
3. Send the list to the user who wants the data

## Part5: Playing game with other online users

### Step1: Send challenge invitation

1. Detect if the user type **:challenge**
2. Send invitation to the other user
3. the other user reject the invitation (no game)
4. start playing game if the other user accept the invitation

### Step2: Board game

1. send the original blank 3x3 board to both users
2. start from the first user (the person who sent invitation)
3. Choose the grid you want to add O/X
4. If you win in this step the game end
5. Or else it will continue until a person win the game or all 9 grids are full

## Part6: Logout

1. user logout -> `^c`
2. mark isOnline=false, fd=-1
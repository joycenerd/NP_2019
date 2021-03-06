#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFSIZE 300000
// 2000000

using namespace std;

// different filetype corresponding structure in web dev
struct {
  char *ext;
  char *filetype;
} extensions[] = {{(char *)"gif", (char *)"image/gif"},
                  {(char *)"jpg", (char *)"image/jpeg"},
                  {(char *)"jpeg", (char *)"image/jpeg"},
                  {(char *)"png", (char *)"image/png"},
                  {(char *)"zip", (char *)"image/zip"},
                  {(char *)"gz", (char *)"image/gz"},
                  {(char *)"tar", (char *)"image/tar"},
                  {(char *)"htm", (char *)"text/html"},
                  {(char *)"html", (char *)"text/html"},
                  {(char *)"exe", (char *)"text/plain"},
                  {(char *)"txt", (char *)"text/txt"},
                  {0, 0}};

const char *CLIENT_FOLDER = "../";

// handle the connection from client socket
int handleSocket(int fd) {
  ssize_t ret;
  static char buf[BUFSIZE + 1];
  int bufLen, i,j;
  char *fstr;

  // read the request from the browser
  ret = read(fd, buf, (size_t)BUFSIZE);

  if (ret == 0 || ret == -1) {
    perror("EROR: read\n");
    exit(3);
  }

  static char response[100];
  // Process POST request
  if(strncmp(buf,"POST ",5)==0 || strncmp(buf,"post ",5)==0){
    
    // get content body's boundary
    string boundary="\0";
    for(i=6;i<ret;i++) {
      if(strncmp(buf+i,"boundary=",strlen("boundary="))==0){
        break;
      }
    }
    i+=strlen("boundary=")+27;
    while(buf[i]!='\n'){
      boundary+=buf[i];
      i++;
    }

    // get content name
    const char *msgAttr="Content-Disposition: form-data; name=\"file\"; filename=";
    for(i=6;i<ret;i++) {
      if(strncmp(buf+i,msgAttr,strlen(msgAttr))==0){
        break;
      }
    }
    i+=strlen("Content-Disposition: form-data; name=\"file\"; filename=")+1;
    string myFileName="\0";
    while(buf[i]!='"'){
      myFileName+=buf[i];
      i++;
    }

    // get content type
    string contentType="\0";
    for(j=i;j<ret;j++) {
      if(strncmp(buf+j,"Content-Type: ",strlen("Content-Type: "))==0){
        break;
      }
    }
    while(buf[j]!='\n'){
      contentType+=buf[j];
      j++;
    }

    // write the content to file
    string wholefilePath="/Users/joycechin/NP_2019/assignment1/datastore/"+myFileName;
    int postFilefd = open(wholefilePath.c_str(), O_RDWR|O_CREAT,0666|O_TRUNC);
    if(postFilefd<=0) perror("can't open file\n");

    bool reachBoundary=false;
    int startIndex=j+3;
    int endIndex=ret-1;
    for(i=j;i<ret;i++){
      if(strncmp(buf+i,"-------------------------",strlen("-------------------------"))==0){
        reachBoundary=true;
        endIndex=i-1;
        break;
      }
    }
    //if(reachBoundary==true) cout <<"\n BOUNDARY HAHA" << endl;
    write(postFilefd,buf+startIndex,endIndex-startIndex+1);
    int num=1;

    while(ret>=0 && reachBoundary==false){
      //cout << "\n" << ret << endl;
      ret=read(fd,buf,(size_t)BUFSIZE);
      for(i=0;i<ret;i++){
        if(strncmp(buf+i,"-------------------------",strlen("-------------------------"))==0){
          reachBoundary=true;
          //cout << "\nSECOND BOUNDARY"<< endl;
          break;
        }
      }
      if(reachBoundary==true) write(postFilefd,buf,i);
      else write(postFilefd,buf,strlen(buf));
      //cout << "\nCHECK: " << ++num << endl;
    }
    close(postFilefd);

    // send response
    sprintf(buf, "HTTP/1.1 200 OK\r\n%s\r\n\r\n", contentType.c_str());
    write(fd, buf, strlen(buf));
    return 0;
  }

  if (ret > 0 && ret < BUFSIZE)
    buf[ret] = 0;
  else
    buf[0] = 0;

  // Process GET request
  if (strncmp(buf, "GET ", 4) == 0 || strncmp(buf, "get ", 4) == 0) {

    //printf("%s\n",buf);
    //for(i=0;i<strlen(buf);i++) printf("%c",buf[i]);

    for (i = 4; i < BUFSIZE; i++) {
      if (buf[i] == ' ') {
        buf[i] = 0;
        break;
      }
    }

    // read index.html
    for (int j = 0; j < i - 1; j++) {
      if (buf[j] == '.' && buf[j + 1] == '.') {
        perror("ERROR: upper directory\n");
        exit(3);
      }
    }

    if (strncmp(&buf[0], "GET /\0", 6) == 0 ||
        strncmp(&buf[0], "get /\0", 6) == 0) {
      strcpy(buf, "GET /index.html\0");
    }

    // recognize the filetype request by the client and open with the browser
    bufLen = strlen(buf);
    fstr = (char *)0;

    for (i = 0; extensions[i].ext != 0; i++) {
      int length = strlen(extensions[i].ext);
      if (strncmp(&buf[bufLen - length], extensions[i].ext, length) == 0) {
        fstr = extensions[i].filetype;
        break;
      }
    }

    if (fstr == 0) {
      fstr = extensions[i - 1].filetype;
    }

    int filefd = open(&buf[5], O_RDONLY);
    if (fd == -1)
      write(fd, "failed to open the file", strlen("failed to open the file"));
    sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type:%s\r\n\r\n", fstr);
    write(fd, buf, strlen(buf));

    ret = read(filefd, buf, (size_t)BUFSIZE);
    while (ret > 0) {
      write(fd, buf, ret);
      ret = read(filefd, buf, (size_t)BUFSIZE);
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  int listenfd, ret;
  static struct sockaddr_in serverAddr, clientAaddr;
  fd_set activefdSet, readfdSet;

  // go to the folder that has the .html file
  if (chdir(CLIENT_FOLDER) == -1) {
    printf("ERROR: Cannot change to directory %s\n", CLIENT_FOLDER);
    exit(4);
  }

  // create socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  if (listenfd < 0) {
    perror("ERROR: opening socket\n");
    exit(3);
  }

  cout << "listenfd: " << listenfd << endl;

  // setup the network connection

  /*struct sockaddr_in {
      sa_family_t    sin_family;  address family: AF_INET
      in_port_t      sin_port;    port in network byte order
      struct in_addr sin_addr;    internet address
  };*/

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(8000);

  // create and start listening to network
  ret = bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

  if (ret < 0) {
    perror("ERROR: opening listening\n");
    exit(3);
  }

  if (listen(listenfd, 1) < 0) {
    perror("ERROR: start listening to network\n");
    exit(3);
  }

  FD_ZERO(&activefdSet);
  FD_SET(listenfd, &activefdSet);

  while (1) {
    readfdSet = activefdSet;

    if (select(FD_SETSIZE, &readfdSet, NULL, NULL, NULL) < 0) {
      perror("ERROR: select");
      exit(EXIT_FAILURE);
    }

    // patrol all the socket in fd_set
    for (int i = 0; i < FD_SETSIZE; i++) {
      if (FD_ISSET(i, &readfdSet)) {
        if (i == listenfd) {
          socklen_t addrLength = sizeof(clientAaddr);
          int newfd =
              accept(listenfd, (struct sockaddr *)&clientAaddr, &addrLength);
          if (newfd < 0) {
            perror("ERROR: accept\n");
            exit(EXIT_FAILURE);
          }
          FD_SET(newfd, &activefdSet);
        }

        // execute the request from the client
        else {
          if (!handleSocket(i)) {
            close(i);
            FD_CLR(i, &activefdSet);
          }
        }
      }
    }
  }

  return 0;
}

//---------------------------183712095321194641521522749875
//-----------------------------183712095321194641521522749875
//---------------------------17740563249016825201933500408
//-----------------------------17740563249016825201933500408
//---------------------------723854912334645729131095810
//-----------------------------723854912334645729131095810
//---------------------------723854912334645729131095810
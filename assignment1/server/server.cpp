#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#define BUFSIZE 8192

using namespace std;

// different filetype corresponding structure in web dev
struct {
    char *ext;
    char *filetype;
} extensions[]={
    {(char *)"gif",(char *)"image/gif"},
    {(char *)"jpg",(char *)"image/jpeg"},
    {(char *)"jpeg",(char *)"image/jpeg"},
    {(char *)"png",(char *)"image/png"},
    {(char *)"zip",(char *)"image/zip"},
    {(char *)"gz",(char *)"image/gz"},
    {(char *)"tar",(char *)"image/tar"},
    {(char *)"htm",(char *)"text/html"},
    {(char *)"html",(char *)"text/html"},
    {(char *)"exe",(char *)"text/plain"},
    {0,0}
};

const char *CLIENT_FOLDER="../";


// handle the connection from client socket
int handleSocket(int fd) {
    ssize_t ret;
    static char buf[BUFSIZE+1];
    int bufLen,i;
    char *fstr;


    // read the request from the browser
    ret=read(fd,buf,(size_t)BUFSIZE);

    if(ret==0 || ret==-1){
        perror("EROR: read\n");
        exit(3);
    }

    if(ret>0 && ret<BUFSIZE) buf[ret]=0;
    else buf[0]=0;

    for(i=0;i<ret;i++) {
        if(buf[i]=='\r' || buf[i]=='\n') {
            buf[i]=0;
        }
    }


    // Process GET request
    if(strncmp(buf,"GET ",4)!=0 && strncmp(buf,"get ",4)!=0){
        perror("ERROR: GET\n");
        exit(3);
    }

    for(i=4;i<BUFSIZE;i++) {
        if(buf[i]==' ') {
            buf[i]=0;
            break;
        }
    }


    // read index.html
    for(int j=0;j<i-1;j++){
        if(buf[j]=='.' && buf[j+1]=='.') {
            perror("ERROR: upper directory\n");
            exit(3);
        }
    }

    if(strncmp(&buf[0],"GET /\0",6)==0 || strncmp(&buf[0],"get /\0",6)==0) {
        strcpy(buf,"GET /index.html\0");
    }


    // recognize the filetype request by the client and open with the browser
    bufLen=strlen(buf);
    fstr=(char *)0;

    for(i=0;extensions[i].ext!=0;i++) {
        int length=strlen(extensions[i].ext);
        if(strncmp(&buf[bufLen-length],extensions[i].ext,length)==0) {
            fstr=extensions[i].filetype;
            break;
        }
    }

    if(fstr==0) {
        fstr=extensions[i-1].filetype;
    }

    int filefd=open(&buf[5],O_RDONLY);
    if(fd==-1) write(fd,"failed to open the file",strlen("failed to open the file"));
    sprintf(buf,"HTTP/1.0 200 OK\r\nContent-Type:%s\r\n\r\n",fstr);
    write(fd,buf,strlen(buf));

    ret=read(filefd,buf,(size_t)BUFSIZE);
    while(ret>0){
        write(fd,buf,ret);
        ret=read(filefd,buf,(size_t)BUFSIZE);
    }

    return 0;
}

int main(int argc,char **argv) {
    int listenfd,ret;
    static struct sockaddr_in serverAddr,clientAaddr;
    fd_set activefdSet,readfdSet;


    // go to the folder that has the .html file
    if(chdir(CLIENT_FOLDER)==-1) {
        printf("ERROR: Cannot change to directory %s\n",CLIENT_FOLDER);
        exit(4);
    }


    // create socket
    listenfd=socket(AF_INET,SOCK_STREAM,0);

    if(listenfd<0){
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

    serverAddr.sin_family=AF_INET;
    serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    serverAddr.sin_port=htons(8080);

    // create and start listening to network
    ret=bind(listenfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));

    if(ret<0){
        perror("ERROR: opening listening\n");
        exit(3);
    }

    if(listen(listenfd,1)<0){
        perror("ERROR: start listening to network\n");
        exit(3);
    }

    FD_ZERO(&activefdSet);
    FD_SET(listenfd,&activefdSet);

    while(1) {
        readfdSet=activefdSet;

        if(select(FD_SETSIZE,&readfdSet,NULL,NULL,NULL)<0) {
            perror("ERROR: select");
            exit(EXIT_FAILURE);
        }

        // patrol all the socket in fd_set
        for(int i=0;i<FD_SETSIZE;i++) {
            if(FD_ISSET(i,&readfdSet)) {
                if(i==listenfd) {
                    socklen_t addrLength=sizeof(clientAaddr);
                    int newfd=accept(listenfd,(struct sockaddr *)&clientAaddr,&addrLength);
                    if(newfd<0){
                        perror("ERROR: accept\n");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(newfd,&activefdSet);
                }
                else {
                    continue;
                }
            }
        }
    }

    return 0;
}

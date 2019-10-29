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

int main(int argc,char **argv) {
    // go to the folder that has the .html file
    if(chdir(CLIENT_FOLDER)==-1) {
        printf("ERROR: Cannot change to directory %s\n",CLIENT_FOLDER);
        exit(4);
    }
}
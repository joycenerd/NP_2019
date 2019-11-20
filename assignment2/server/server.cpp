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
    int clientSocket[MAX_CONN_LIMIT];

    // intialize all the client sockets
    for(int i=0;i<maxClient;i++) {
        clientSocket[i]=0;
    }


}
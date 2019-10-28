#include <netdb.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
using namespace std;

// struct hostent
//struct hostent *gethostbyname(const char *name);

/* Inside hostent 
struct hostent {
    char    *h_name;               host name:www.google.com            
    char    **h_aliases;           aliase name of the host: google
    int     h_addrtype;            host ip addr type: ipv4(AF_INET) or ipv6(AF_INET6)
    int     h_length;              the length of the host ip addr
    char    **h_addr_list;         host ip addr, if want to print out please use inet_ntop()
    #define h_addr h_addr_list[0]
}; */

const string hostName1="www.baidu.com";
const int STR_LENGTH=32;

int main() {
    struct hostent *hostent1;
    char **pptr;
    char str[STR_LENGTH];
    hostent1=gethostbyname(hostName1.c_str());
    if(hostent1==nullptr) cout << "gethostbyname error for host: " << hostName1 << endl;
    else  {
        cout << "For " << hostName1 << ": " << endl;
        cout << "h_name: " << hostent1->h_name << endl;
        pptr=hostent1->h_aliases;
        for(;*pptr!=nullptr;pptr++) {
            cout << "h_aliases: " << *pptr << endl;
        }
        string addrType;
        if(hostent1->h_addrtype==AF_INET) addrType="IPv4";
        else if(hostent1->h_addrtype==AF_INET6) addrType="IPv6";
        else addrType="Unknown";
        cout << "IP Address type: " << addrType << endl;
        switch(hostent1->h_addrtype) {
            case AF_INET:
            case AF_INET6:
                pptr=hostent1->h_addr_list;
                for(;*pptr!=nullptr;pptr++) {
                    cout << "IP Address: " << inet_ntop(hostent1->h_addrtype,*pptr,str,sizeof(str)) <<endl;
                }
        }
        cout << "First address: " << inet_ntop(hostent1->h_addrtype,hostent1->h_addr,str,sizeof(str)) << endl;
    }
    return 0;
}



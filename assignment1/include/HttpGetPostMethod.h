#ifndef HTTPGETPOSTMETHOD_H
#define HTTPGETPOSTMETHOD_H
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <netdb.h>
#include <sstream>
#include <sys/select.h>
#include <sys/time.h> 
using namespace std;


class HttpGetPostMethod {
    public:
        HttpGetPostMethod();
        virtual ~HttpGetPostMethod();
        int HttpGet(string host, string path, string get_content); // http get request
        int HttpPost(string host, string path, string post_content); // http post request
        string get_request_return();
        string get_main_text();
        int get_return_status_code();

    protected:

    private:
        int return_status_code_;
        string request_return_;
        string main_text_;
        string HttpSocket(string host, string request_str);
        void AnalyzeReturn(void);
};

#endif
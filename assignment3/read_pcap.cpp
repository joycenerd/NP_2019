#include <iostream>
#include <cstdio>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
    if(argc!=3) {
        cout << "ERROR: fewer parameters provided" << endl;
        exit(1);
    }
    ifstream file(argv[2]);
    if(!file) {
        cout << "ERROR: cannot open the file\n" << endl;
        exit(1);
    }
    cout << argv[2] << " is opened" << endl;
    return 0;
}
#include <iostream>
#include "include/CSVWriter.h" 
using namespace std;

int main() {
    CSVWriter csv;
    csv.newRow() << "this" << "is" << "a" << "row";
    csv.newRow() << "this" << "is" << "another" << "row";
    csv << "yeee";
    cout << csv << endl;
    csv.writeToFile("test.csv");
}
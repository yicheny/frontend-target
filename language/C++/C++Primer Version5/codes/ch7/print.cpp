#include <iostream>

using std::cout;
using std::endl;
using std::string;

void print(int i){
    cout << "print int:" << i << endl;
}

void print(const string &s){
    cout << "print string:" << s << endl;
}

int main() {
//    void (*ppr)(const string &) = print;
//    ppr("hello!");
    print(1234);
    print("hello world!");
    return 0;
}
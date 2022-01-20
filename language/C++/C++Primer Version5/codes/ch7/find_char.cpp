#include <iostream>

using std::string;

string::size_type find_char(const string &s,char c){
    string::size_type i = 0;
    while(i != s.size() && s[i] != c){
        ++i;
    }
    return i;
}

int main() {
    std::cout << find_char("hello world!",'o') << std::endl;
    return 0;
}
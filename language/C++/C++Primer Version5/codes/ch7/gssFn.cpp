#include <iostream>

using std::cout;
using std::endl;
using std::string;

typedef string (*gssFn)(const string &,const string &);

string getShorterString(const string &s1,const string &s2){
    return (s1.size() > s2.size()) ? s2 : s1;
}

int main() {
    gssFn pf = getShorterString;

    string s1 = "11";
    string s2 = "222";
    cout << pf(s1,s2) << endl;
    return 0;
}
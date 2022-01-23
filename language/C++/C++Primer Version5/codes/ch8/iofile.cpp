#include <iostream>
#include <fstream>

using namespace std;

int main() {
    //注：使用CLion则默认根目录是cmake-build-debug-coverage
    string data;
    string inPath = "../test.txt";
    string outPath = "../out.txt";
    ifstream infile(inPath.c_str());
    ofstream outfile(outPath.c_str());
    while(getline(infile,data)){
        cout << data << endl;
        outfile << data << endl;
    }
    infile.close();
    outfile.close();
    return 0;
}
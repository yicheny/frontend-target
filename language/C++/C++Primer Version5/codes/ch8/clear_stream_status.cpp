#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main() {
    vector<string> files;
    files.emplace_back("../out.txt");
    files.emplace_back("../error.txt");
    files.emplace_back("../test.txt");
    vector<string>::const_iterator it = files.begin();
    string s;
    ifstream input;

    while(it != files.end()){
        input.open(it->c_str());
        if(!input){
            cout << "file read fail:" << it->c_str() << endl;
            input.close();//ifstream想要对新文件进行关联，必须关闭和现有文件的关联
            input.clear();//将当前流的所有状态值重设为有效状态
            ++it;
            continue;
        }
        while(input >> s)
            cout << s << endl;
        input.close();//ifstream想要对新文件进行关联，必须关闭和现有文件的关联
        ++it;
    }
    return 0;
}
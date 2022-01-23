#include <iostream>
//#include <Windows.h>

using namespace std;

int main() {
    std::locale::global(std::locale(""));
//    SetConsoleOutputCP(CP_UTF8);
    system("chcp 65001");
    cout << "中文测试！" << endl;
    return 0;
}

//中文乱码解决方案
//方案1
//添加system("chcp 65001");
//方案2
//2.1 #include <Windows.h>
//2.2 SetConsoleOutputCP(CP_UTF8);
//参考文档：https://blog.csdn.net/qq_44881664/article/details/120166258
#include <iostream>

int main() {
    //ca1是一个不带结束符NULL的字符数组，所以进行运算或打印时不可预料，因为获取时会指针会不停向下走，直到在内存中遇到NULL
    char ca1[] = {'C','+','+'};
    char ca2[] = {'C','+','+','\0'};//这里的'\0'实际上就是NULL
    //字面量写法自带NULL
    char ca3[] = "C++";
    const char *cp = "C++";
    char *cp1 = ca1;
    char *cp2 = ca2;

    std::cout << ca2 << std::endl;
    return 0;
}
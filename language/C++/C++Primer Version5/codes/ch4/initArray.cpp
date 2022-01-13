#include <iostream>

int main() {
    char cArr[] = "hello world!";
    std::cout<<cArr<<" " << sizeof(cArr) <<std::endl;

    int iArr[] = {1,2,3,4,5};
    std::cout<<sizeof(iArr) / sizeof(iArr[0])<<std::endl;

    //在全局声明默认值是0
    //在局部声明默认值随机【可以认为无初始化】
    int iArr2[10];
    std::cout<<*iArr2<< std::endl;
    return 0;
}
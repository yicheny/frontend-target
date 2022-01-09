#include <iostream>

//未知数目的输入
int main() {
    int sum=0,value;
    //注意，这里判断使用的是输入流的状态
    // 输入流异常的情况下，就会退出循环
    // 比如这里定义的是整数，如果输入非整数，比如任意一个字母，并回车， 就可以推出
    while(std::cin>>value){
        sum+=value;
    }
    std::cout<<"sum = " << sum <<std::endl;
    return 0;
}

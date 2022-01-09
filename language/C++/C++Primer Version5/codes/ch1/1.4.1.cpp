#include <iostream>

//使用while进行1到10的求和
int main() {
    int sum=0,val=1;
    while(val<=10){
        sum += val;
        ++val;
    }
    std::cout<<"sum of 1 to 10 inclusive is "<<sum<<std::endl;
    return 0;
}

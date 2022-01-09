#include <iostream>

//使用for进行1到10的求和
int main() {
    int sum=0;
    for(int i = 0; i <= 10; i++){
        sum+=i;
    }
    std::cout<<"sum of 1 to 10 inclusive is "<<sum<<std::endl;
    return 0;
}

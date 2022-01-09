#include <iostream>

//通过if实现自定义求和范围
int main() {
    std::cout<<"Enter two numbers:"<<std::endl;
    int v1,v2;
    std::cin>>v1>>v2;
    int upper=v1,lower=v2;
    if(v1<v2){
        upper = v2;
        lower = v1;
    }
    int sum=0;
    for(int i = lower; i <= upper; i++){
        sum+=i;
    }
    std::cout<<"sum of "<< lower << " to " << upper << " inclusive is "<<sum<<std::endl;
    return 0;
}

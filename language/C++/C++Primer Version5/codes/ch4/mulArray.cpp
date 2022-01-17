#include <iostream>

int main() {
    int ia[3][4] = {
            {1,2,3,4},
            {5,6,7,8},
            {9,10,11,12},
    };
    std::cout<<**ia<<std::endl;//1
    int (*ip)[4] = ia;
    std::cout<<**ip<<std::endl;//1
    ip = &ia[2];
    std::cout<<**ip<<std::endl;//9
    return 0;
}

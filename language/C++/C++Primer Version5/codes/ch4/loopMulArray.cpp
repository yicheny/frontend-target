#include <iostream>

int main() {
    int ia[3][4] = {
            {1,2,3,4},
            {5,6,7,8},
            {9,10,11,12},
    };

    typedef int int_array[4];

    for(int_array *p = ia; p != ia +3; ++p){
        for(int *q = *p; q != *p+4;++q){
            std::cout << *q << std::endl;
        }
    }
    return 0;
}

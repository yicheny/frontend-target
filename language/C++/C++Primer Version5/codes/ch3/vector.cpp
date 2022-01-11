#include <iostream>
#include <vector>

int main() {
    std::vector<unsigned long long> ivec;
    for(std::vector<unsigned long long>::size_type ix=0;ix!=10;++ix){
//        ivec[ix] = ix; //非法
        ivec.push_back(ix);
        //仅能对已存在的元素进行下标操作a
        ivec[ix] = 10-ix;
        std::cout<<ivec[ix]<<std::endl;
    }
    return 0;
}

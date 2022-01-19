#include <iostream>
#include <vector>

using std::vector;

vector<int>::const_iterator find_val(
        vector<int>::const_iterator beg,
        vector<int>::const_iterator end,
        int value,
        vector<int>::size_type &occurs
){
    auto res_iter = end;
    occurs = 0;
    for(;beg != end;++beg){
        if(*beg == value)
            res_iter = beg;
        ++occurs;
    }
    return res_iter;
}

int main() {
    vector<int> ivec;
    ivec.reserve(10);//分配最小存储长度
    for(int i=0;i<10;i++){
        ivec.push_back(i);
    }
    size_t ctr;

//    auto result = find_val(ivec.begin(),ivec.end(),3,ctr);

    std::cout << ivec[5] << std::endl;
    return 0;
}
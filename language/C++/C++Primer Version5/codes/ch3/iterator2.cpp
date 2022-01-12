#include <iostream>
#include <vector>

int main() {
    std::vector<int> ivec;
    ivec.push_back(1);
    ivec.push_back(2);
    ivec.push_back(3);

    for(auto iter = ivec.begin();iter!=ivec.end();++iter){
        *iter = 0;
    }

    std::cout << ivec[0] << std::endl;
    std::cout << ivec[1] << std::endl;
    std::cout << ivec[2] << std::endl;
    return 0;
}

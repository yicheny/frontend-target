#include <iostream>
#include <vector>

int main() {
    std::vector<int> ivec;
    ivec.push_back(1);
    ivec.push_back(2);
    ivec.push_back(3);

    auto bIter = ivec.begin();
    auto bIter2 = ivec.begin();
    auto eIter = ivec.end();
    std::cout << *bIter << std::endl;
    std::cout << *++bIter << std::endl;
    std::cout << *++bIter << std::endl;
    std::cout << *bIter2 << std::endl;
    std::cout << *eIter << std::endl;
    return 0;
}

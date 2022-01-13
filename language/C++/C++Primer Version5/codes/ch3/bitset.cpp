#include <iostream>
#include <bitset>

int main() {
    std::string s = "1100";
    std::bitset<8> bs(s);
    std::cout<<bs<<std::endl;
    return 0;
}
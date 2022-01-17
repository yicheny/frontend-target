#include <iostream>
#include <bitset>

int main() {
    //使用位集表示一次测验
    std::bitset<30> bitset_quiz1;
    bitset_quiz1.set(27);
    std::cout << bitset_quiz1 << std::endl;

    //使用整型
    unsigned long int_quiz1 = 1;
    int_quiz1 = int_quiz1 | 1UL<<27;
    std::cout << std::hex << int_quiz1 << std::endl;//16进制输出
    return 0;
}
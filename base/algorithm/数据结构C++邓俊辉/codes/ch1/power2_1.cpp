#include <iostream>

//前提：禁止超过1位的移位运算
//对任意非负整数n，计算幂2^n
__int64 power2(int n) {
   __int64 pow = 1;//2^0;
   while(0 < n--)
       pow <<= 1; //翻倍； x <<= n 相当于 x*(2^n)
   return pow;
}

int main() {
    std::cout << power2(10) << std::endl;
    return 0;
}
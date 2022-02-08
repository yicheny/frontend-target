#include <iostream>

inline __int64 sqr(__int64 a) { return a * a; }

//前提：禁止超过1位的移位运算
//对任意非负整数n，计算幂2^n
__int64 power2(int n) {
    if (0 == n) return 1;
    return (n&1) ? sqr(power2(n>>1)) << 1 : sqr(power2(n>>1));
}

int main() {
    std::cout << power2(10) << std::endl;
    return 0;
}
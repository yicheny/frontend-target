#include <iostream>

//迭代版【动态规划】
__int64 fib(int n) {
    __int64 f = 0, g = 1;
    while (0 < n--) {
        g += f;
        f = g - f;
    }
    return f;
}

int main() {
    std::cout << fib(100) << std::endl;
    return 0;
}

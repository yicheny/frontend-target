#include <iostream>

//斐波那契数列（二分递归）
__int64 fib(int n){
    return (2>n) ? (__int64) n : (fib(n-1) + fib(n-2));
}

int main() {
    std::cout << fib(10) << std::endl;
    return 0;
}

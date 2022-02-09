#include <iostream>

//线性迭代（制表法）
__int64 fib(int n, __int64& prev){
    if(0==n) {
        prev = 1;
        return 0;
    }else{
        __int64 prevPrev;
        prev = fib(n-1,prevPrev);
        return prevPrev + prev;
    }
}

__int64 fib(int, __int64&);
__int64 fib(int n){
    __int64 r;
    return fib(n,r);
}

int main() {
    std::cout << fib(100) << std::endl;
    return 0;
}

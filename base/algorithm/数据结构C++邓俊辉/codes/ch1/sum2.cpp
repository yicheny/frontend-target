#include <iostream>

//数组求和算法（线性递归版）
int sum(const int A[],int n){
    if(n<1) return 0;
    return sum(A,n-1) + A[n-1];
}

int main() {
    int arr[] = {1,2,3,4,5,6};
    std::cout << sum(arr,sizeof(arr)/sizeof(*arr)) << std::endl;
    return 0;
}

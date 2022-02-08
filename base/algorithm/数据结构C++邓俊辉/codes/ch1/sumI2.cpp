#include <iostream>

//数组求和算法（线性递归版）
int sumI(const int A[],int n){
    if(n<1) return 0;
    return sumI(A,n-1) + A[n-1];
}

int main() {
    int arr[] = {1,2,3,4,5,6};
    std::cout << sumI(arr,sizeof(arr)/sizeof(*arr)) << std::endl;
    return 0;
}

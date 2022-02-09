#include <iostream>

//数组求和算法（迭代版）
int sumI(const int A[],int n){
    int sum = 0;
    for(int i=0; i<n; i++)
        sum += A[i];
    return sum;
}

int main() {
    int arr[] = {1,2,3,4,5,6};
    std::cout << sumI(arr,sizeof(arr)/sizeof(*arr)) << std::endl;
    return 0;
}

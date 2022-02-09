#include <iostream>
#include "tools/print.h"


//多递归基实现
void reverse(int *, int, int);
void reverse(int *A, int n) {
    reverse(A, 0, n - 1);
}

void reverse(int *A, int lo, int hi) {
next:
    if (lo < hi) {
        std::swap(A[lo], A[hi]);
        lo++;hi--;
        goto next;
    }//else隐含终止条件
}


int main() {
    int arr[] = {1, 2, 3, 4, 5, 6};
    const int length = sizeof (arr)/sizeof (*arr);
    reverse(arr,length);
    array_print(arr,length);
    return 0;
}

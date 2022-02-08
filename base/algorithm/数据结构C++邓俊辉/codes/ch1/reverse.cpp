#include <iostream>
#include "tools/print.h"


//多递归基实现
void reverse(int *, int, int);
void reverse(int *A, int n) {
    reverse(A, 0, n - 1);
}

void reverse(int *A, int lo, int hi) {
    if (lo < hi) {
        std::swap(A[lo], A[hi]);
        reverse(A, lo + 1, hi - 1);
    }
}


int main() {
    int arr[] = {1, 2, 3, 4, 5, 6};
    const int length = sizeof (arr)/sizeof (*arr);
    reverse(arr,length);
    array_print(arr,length);
    return 0;
}

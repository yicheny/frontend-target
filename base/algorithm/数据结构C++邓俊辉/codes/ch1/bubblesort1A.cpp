#include <iostream>
#include "tools/print.h"


//冒泡排序算法（1A版本）
//n >= 0
void bubblesort1A(int A[],int n){
    bool sorted = false;
    while(!sorted){
        sorted = true;
        for(int i=1;i<n;i++){
            if(A[i-1] > A[i]){
                std::swap(A[i-1],A[i]);
                sorted = false;
            }
        }
        n--;//每次扫描，都可以保证这一次最后位的值是当次最大的，所以下一次不再需要扫描
    }
}

int main() {
    int list[] = {3,8,7,6,2,5,1,3,16,313,23,24,9};
    int l = sizeof(list)/sizeof(*list);
    bubblesort1A(list,l);
    array_print(list,l);
//    std::cout << list << std::endl;
    return 0;
}

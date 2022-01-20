#include <iostream>

using std::cout;
using std::endl;

void array_print(const int *arr, size_t size){
    for(size_t i=0;i<size;i++){
//      cout<< *arr++ << " ";
        cout<< arr[i] << " ";
    }
    cout << endl;
}

int main() {
    const int iarr[] = {1,2,3,4,5};
    size_t size = sizeof(iarr) / sizeof (iarr[0]);
    array_print(iarr,size);
    return 0;
}
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
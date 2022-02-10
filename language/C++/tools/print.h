#include <iostream>

using std::cout;
using std::endl;

template <typename T>
void array_print(T const arr[], size_t size){
    for(size_t i=0;i<size;i++){
//      cout<< *arr++ << " ";
        cout<< arr[i] << " ";
    }
    cout << endl;
}
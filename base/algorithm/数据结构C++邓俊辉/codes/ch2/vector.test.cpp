#include <iostream>
#include "tools/vector.h"

using std::cout;
using std::endl;

template <typename T>
void print(const Vector<T> vec, int size){
    for(size_t i=0;i<size;i++){
        cout<< vec.getByIndex(i) << " ";
    }
    cout << endl;
}

template <typename T>
void print(const T x){
    cout << x << endl;
}

int main(){
    const Vector<int> vec(100,3,0);
    print(vec,vec.size());
    return 0;
}
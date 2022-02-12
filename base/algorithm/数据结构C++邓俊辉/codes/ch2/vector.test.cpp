#include <iostream>
#include "tools/vector.h"

using std::cout;
using std::endl;

template<typename T>
void print(const Vector<T> vec) {
    for (size_t i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

template<typename T>
void print(const T x) {
    cout << x << endl;
}

int main() {
    Vector<int> vec(100, 2, 0);
    print(vec);

    int arr[] = {21, 22, 23, 24, 25};
    vec.copyFrom(arr, 1, 4);
    print(vec);

    Vector<int> vec2;
    vec2 = vec;
    print(vec2);
    return 0;
}
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

    int arr[] = {21, 22, 23, 24, 25,26,27,28};
    vec.copyFrom(arr, 1, 6);
    print(vec);

    Vector<int> vec2;
    vec2 = vec;
    print(vec2);

    vec.unsort();
    print(vec);

    vec.insert(100);
    vec.insert(101);
    vec.insert(102);
    vec.insert(102);
    vec.insert(100);
    vec.insert(102);
    print(vec);

    vec.remove(2,4);
    print(vec);

    vec.deduplicate();
    print(vec);

    vec.traverse(print);
    return 0;
}
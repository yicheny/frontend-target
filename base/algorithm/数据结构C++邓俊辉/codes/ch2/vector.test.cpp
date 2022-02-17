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

//函数类
//相对一般函数优势：
//1. 函数对象可以有自己的状态
//2. 函数对象有自己特有的类型
template <typename T>
struct Increase{
    //virtual operator
    //虚重载：如果不使用virtual关键字，调用派生类方法时，调用的是基类方法；使用虚函数，则会调用派生类方法
    virtual void operator()(T& e){
        e++;
    }
};

int main() {
    Vector<int> vec(100, 2, 0);
    print(vec);

    int arr[] = {21, 22, 23, 24, 25,26,27,28};
    vec.copyFrom(arr, 1, 6);
    print(vec);

    Vector<int> vec2;
    vec2 = vec;
    print(vec2);

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

    vec.remove(2,4);
    vec.traverse(print);

    Increase<int> increase;
    vec.traverse(increase);
    print(vec);

    print(vec.search(102));
    print(vec.search(10));
    print(vec.search(200));

    vec.unsort();
    print(vec);

    vec.sort();
    print(vec);
    return 0;
}
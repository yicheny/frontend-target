#ifndef FRONTEND_TARGET_PRINT_H
#define FRONTEND_TARGET_PRINT_H

#include <iostream>
#include <vector>

using namespace std;

template <typename T>
void print(const T x){
    cout << x << endl;
}

template <typename T>
void print(T const arr[], size_t size){
    for(size_t i=0;i<size;i++){
        cout<< arr[i] << " ";
    }
    cout << endl;
}

template <typename T>
void print(const vector<T> vec, size_t size){
    for(size_t i=0;i<size;i++){
        cout<< vec[i] << " ";
    }
    cout << endl;
}

#endif //FRONTEND_TARGET_PRINT_H
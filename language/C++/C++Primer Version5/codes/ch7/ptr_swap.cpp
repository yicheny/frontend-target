#include <iostream>

void ptr_swap(int *&v1, int *&v2){
    int *tmp = v2;
    v2 = v1;
    v1 = tmp;
}

int main() {
    int i = 1;
    int j = 2;
    int *pi = &i;
    int *pj = &j;
    ptr_swap(pi,pj);
    std::cout << *pi << std::endl;
    std::cout << *pj << std::endl;
    return 0;
}
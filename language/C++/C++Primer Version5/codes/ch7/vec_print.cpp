#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
typedef vector<int>::const_iterator vec_const_iter;

void vec_print(vec_const_iter beg,vec_const_iter end){
    while(beg != end){
        cout << *beg++ << " ";
    }
    cout << endl;
}

void vec_fill(vector<int> &vec, int size){
    for(int i=0;i<size;i++){
        vec.push_back(i);
    }
}

int main() {
    vector<int> ivec;
    vec_fill(ivec, 20);
    vec_print(ivec.begin(),ivec.end());
    return 0;
}
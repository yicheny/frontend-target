#include <iostream>

using std::cin;
using std::cout;

int main() {
    cin.tie(&cout);//将cin和cout关联起来
    std::ostream *old_tie = cin.tie();//此时是cout
    cin.tie(nullptr);//解除关联，当cin读入时cout不再刷新缓冲区
    cin.tie(&std::cerr);//关联cin和cerr--这不是好的做法
    cin.tie(nullptr);
    cin.tie(old_tie);//重新建立 cin 和 cout 之间的正常联系
    return 0;
}
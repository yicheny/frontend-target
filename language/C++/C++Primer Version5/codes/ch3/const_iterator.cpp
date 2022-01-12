#include <iostream>
#include <vector>

int main() {
    std::vector<std::string> ivec;
    ivec.emplace_back("hello");
    ivec.emplace_back("world");
    ivec.emplace_back("hero");

    for(std::vector<std::string>::const_iterator iter = ivec.begin();iter!=ivec.end();++iter){
//        *iter = "";//非法 const_iterator只读
        std::cout<<*iter<<std::endl;
    }

    //const iterator 和 const_iterator不同
    //通过const声明的迭代器不能修改本身的值，可以修改指向的值
    const std::vector<std::string>:: iterator citer = ivec.begin();
    *citer = "custom";//合法
//    citer = "custom";//非法
    return 0;
}
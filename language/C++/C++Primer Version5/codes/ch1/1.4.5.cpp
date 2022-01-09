#include <iostream>
#include "include/Sales_item.h"

//测试Sales_item.h
int main() {
    Sales_item book;
    std::cout<<"input book info:"<<std::endl;
    //0-201-70353-X 4 24.99
    std::cin>>book;
    std::cout<<"total result: "<<book<<std::endl;
    return 0;
}

#include <iostream>
#include "include/Sales_item.h"

//统计不同书籍
int main() {
    //0-201-70353-X 4 24.99

    Sales_item trans, total;
    if (std::cin >> total) {
        while (std::cin >> trans) {
            if (compareIsbn(total, trans)){
                total += trans;
            }else{
                std::cout << total << std::endl;
                total = trans;
            }
            std::cout << total << std::endl;
        }
    }else{
        std::cout << "no data?" << std::endl;
        return -1;
    }
    return 0;
}

#include <iostream>
#include <cstring>

int main() {
    const char *cp1 = "A string example";
    const char *cp2 = "A different string";
    std::cout << strcmp(cp1,cp2)  << std::endl;

    char largeStr[16+1+18+1];
    //设置count为17，复制cp1的16个字符，还包括结尾的NULL字符
    strncpy(largeStr,cp1,17);
    //strncat同理，设置count需要包含NULL
    strncat(largeStr," ",2);
    strncat(largeStr,cp2,19);
    std::cout<<largeStr<<std::endl;
    std::cout<<strlen(largeStr)<<std::endl;//16+1+18=35
    //过程中largeStr大小始终是36
    return 0;
}
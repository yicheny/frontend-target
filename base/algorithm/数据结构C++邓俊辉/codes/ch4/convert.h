//
// Created by yanglvfeng on 2022/2/24.
//

#ifndef C___PROJECT_CONVERT_H
#define C___PROJECT_CONVERT_H

#include "../DS/stack.h"


//写法1
//每一帧都需要记录参数S、n和base，并且每一次都需要创建digit[]
//增加时间和空间的消耗
/*//十进制数字转任意进制【最大16位】
void convert(Stack<char> &S, __int64 n, int base) {
    static char digit[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    if (0 < n) {
        convert(S, n / base, base);
        S.push(digit[n % base]);
    }
}*/


//写法2
//空间复杂度降至O(1)，同时避免了函数调用的开销
void convert(Stack<char> &S, __int64 n, int base) {
    static char digit[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    while (n > 0) {
        int remainder = (int) (n % base);
        S.push(digit[remainder]);
        n /= base;//更新为base的除商
    }
}

#endif //C___PROJECT_CONVERT_H

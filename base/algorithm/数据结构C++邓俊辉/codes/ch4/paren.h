#ifndef C___PROJECT_PAREN_H
#define C___PROJECT_PAREN_H

#include "../DS/stack.h"

//删除exp[lo,hi]不含括号的最长前缀、后缀
void trim(const char exp[], int &lo, int &hi) {
    while ((lo <= hi) && (exp[lo] != '(') && (exp[lo] != ')')) lo++;
    while ((lo <= hi) && (exp[hi] != '(') && (exp[hi] != ')')) hi--;
}

int divide(const char exp[], int lo, int hi) {
    int mi = lo;
    int crc = 1;
    while ((0 < crc) && (++mi < hi)) {
        if (exp[mi] == '(') crc++;
        if (exp[mi] == ')') crc--;
    }
    return mi;
}

//递归版--O(n^2)
bool paren(const char exp[], int lo, int hi) {
    trim(exp, lo, hi);
    if (lo > hi) return true;
    if (exp[lo] != '(') return false;
    if (exp[hi] != ')') return false;
    int mi = divide(exp, lo, hi);
    if (mi > hi) return false;
    return paren(exp, lo + 1, mi - 1) && paren(exp, mi + 1, hi);
}

//迭代版【使用栈】-- O(n)
bool paren_iterate(const char exp[], int lo, int hi) {
    Stack<char> S;
    for (int i = lo; i <= hi; i++) {
        switch (exp[i]) {
            case '(':
            case '[':
            case '{':
                S.push(exp[i]);
                break;
            case ')':
                if (S.empty() || ('(' != S.pop())) return false;
                break;
            case ']':
                if (S.empty() || ('[' != S.pop())) return false;
                break;
            case '}':
                if (S.empty() || ('}' != S.pop())) return false;
                break;
            default:
                break;
        }
    }
    return S.empty();
}

//迭代（我的实现）-- O(n)
bool match(const char exp[], int lo, int hi) {
    int mi = lo;
    int crc = 1;
    //0<cra 当左括号数量大于等于右括号时成立
    //++mi<hi 在[lo,hi)之内进行遍历
    while ((0 < crc) && (mi < hi)) {
        if (exp[mi] == '(') crc++;
        if (exp[mi] == ')') crc--;
        ++mi;
    }
    //终止的情况只有两种：
    //1. 右括号大于左括号，此时提前终止，因为肯定无法成立
    //2. 遍历正常结束，左括号数量依旧大于等于右括号
    //2.1 大于，则异常
    //2.2 等于，则相互匹配
    return mi == hi && crc == 1;
}

#endif //C___PROJECT_PAREN_H

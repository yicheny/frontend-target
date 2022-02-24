#ifndef C___PROJECT_PAREN_H
#define C___PROJECT_PAREN_H

//删除exp[lo,hi]不含括号的最长前缀、后缀
void trim (const char exp[],int& lo,int& hi){
    while((lo <= hi) && (exp[lo] != '(') && (exp[lo] != ')')) lo++;
    while((lo <= hi) && (exp[hi] != '(') && (exp[hi] != ')')) hi--;
}

int divide (const char exp[], int lo, int hi) {
    int mi = lo;
    int crc = 1;
    while((0<crc) && (++mi < hi)){
        if(exp[mi] == ')') crc--;
        if(exp[mi] == '(') crc++;
    }
    return mi;
}

#endif //C___PROJECT_PAREN_H

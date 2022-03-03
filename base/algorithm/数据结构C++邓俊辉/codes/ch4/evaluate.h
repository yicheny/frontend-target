//
// Created by yanglvfeng on 2022/3/3.
//

#ifndef C___PROJECT_EVALUATE_H
#define C___PROJECT_EVALUATE_H

#include "../DS/stack.h"

float evaluate(char* S,char*& RPN){
    Stack<float> opnd;
    Stack<char> optr;
    optr.push('\0');
    while(!optr.empty()){
        if(isdigit(*S)){
            readNumber(S,opnd);
            append(RPN,opnd.top());
        }else{
            switch (orderBetween(optr.top(),*S)) {
                case '<':
                    optr.push(*S);
                    S++;
                    break;
                case '=':
                    optr.pop();
                    S++;
                    break;
                case ">":{
                    char op = optr.pop();
                    append(RPN,op);
                    if('!' == op){
                        float pOpnd = opnd.pop();
                        opnd.push(calcu(op,pOpnd));
                    }else{
                        float pOpnd2 = opnd.pop();
                        float pOpnd1 = opnd.pop();
                        opnd.push(calc(pOpnd1,op,pOpnd2));
                    }
                    break;
                }
                default:
                    exit(-1);
            }
        }
    }
    return  opnd.pop();
}

#endif //C___PROJECT_EVALUATE_H

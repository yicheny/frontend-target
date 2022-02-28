#include "tools/utils/print.h"
#include "tools/DS/stack.h"
#include "tools/utils/convert.h"
#include "tools/utils/paren.h"

int main() {
    system("chcp 65001");

    Stack<int> stack;

    print("-----测试入栈-----");
    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);
    stack.push(5);
    stack.traverse(print);

    print("-----测试出栈-----");
    stack.pop();
    stack.pop();
    stack.traverse(print);

    print("-----测试top()-----");
    print(stack.top());

    print("-----测试convert()-----");
    Stack<char> stack1;
    convert(stack1, 16665, 16);
    stack1.traverse(print);

    print("-----测试match()-----");
    char chars[] = "123()1()2((3+4))890";
    int size = sizeof chars / sizeof *chars - 1;
    print(size);
    print(match(chars,0,size));

    print("-----测试paren()-----");
    print(paren(chars,0,size));

    print("-----测试paren_iterate()-----");
    print(paren_iterate(chars,0,size));
    return 0;
}
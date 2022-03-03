#include "tools/utils/print.h"
#include "tools/DS/stack.h"
#include "tools/DS/queue.h"
#include "tools/utils/convert.h"
#include "tools/utils/paren.h"

int main() {
    system("chcp 65001");

    print("-----测试栈-----");
    Stack<int> stack;

    print("-----测试push()-----");
    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);
    stack.push(5);
    stack.traverse(print);

    print("-----测试pop()-----");
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

    print("-----测试队列-----");
    Queue<int> queue;

    print("-----enqueue()-----");
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);
    queue.enqueue(5);
    queue.traverse(print);

    print("-----dequeue()-----");
    queue.dequeue();
    queue.traverse(print);

    print("-----front()-----");
    print(queue.front());
    return 0;
}
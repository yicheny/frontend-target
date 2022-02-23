#include "tools/print.h"
#include "tools/list.h"

int main() {
    system("chcp 65001");

    List<int> list;

    print("-----测试插入-----");
    list.insertAsFirst(1);
    list.insertS(list.first(),2);
    list.insertP(list.last(),3);
    list.traverse(print);

    print("-----测试无序查找-----");
    ListNodePosi(int) position = list.find(3);
    print(position);

    list.insertS(position,4);
    list.insertS(position,6);
    list.insertS(position,7);
    list.insertS(position,6);
    print("-----插入-----");
    list.traverse(print);

    print("-----无序去重------");
    list.deduplicate();
    list.traverse(print);

    print("-----测试排序-----");
    list.sort();
    list.traverse(print);

    print("-----选取最大值-----");
    print(list.selectMax()->data);
    return 0;
}
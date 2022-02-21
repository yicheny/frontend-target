#include <iostream>

typedef int Rank;
#define ListNodePosi(T) ListNode<T>* //列表节点位置

template <typename T>
struct ListNode{
    //成员
    T data;
    ListNodePosi(T) pred;//前驱
    ListNodePosi(T) succ;//后继

    //构造函数
    ListNode(){}
    ListNode(T e, ListNodePosi(T) p = NULL, ListNodePosi(T) s = NULL) : data(e),pred(p),succ(s){}

    //操作接口
    ListNodePosi(T) insertAsPred(T const& e);

    ListNodePosi(T) insertAsSucc(T const& e);
};
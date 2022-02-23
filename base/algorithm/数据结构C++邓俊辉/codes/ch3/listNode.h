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
    //参数：插入元素，前节点，后节点
    ListNode(T e, ListNodePosi(T) p = NULL, ListNodePosi(T) s = NULL) : data(e),pred(p),succ(s){}

    //操作接口
    ListNodePosi(T) insertAsPred(T const& e);

    ListNodePosi(T) insertAsSucc(T const& e);
};

template <typename T>
ListNodePosi(T)ListNode<T>::insertAsPred(const T &e) {
    //向前插入元素
    ListNodePosi(T) x = new ListNode(e,pred,this);
    //修改前驱、后继
    pred->succ = x;
    pred = x;
    return x;
}

template <typename T>
ListNodePosi(T)ListNode<T>::insertAsSucc(const T &e) {
    //向后插入元素
    ListNodePosi(T) x = new ListNode(e,this,succ);
    //修改前驱、后继
    succ->pred = x;
    succ = x;
    return x;
}
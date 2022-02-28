#ifndef FRONTEND_TARGET_LIST_H
#define FRONTEND_TARGET_LIST_H

#include "listNode.h"


template<typename T>
class List {
private:
    int _size;//规模
    ListNodePosi(T)header;//头哨兵
    ListNodePosi(T)trailer;//尾哨兵

protected:
    //初始化
    void init();

    //清除所有节点
    int clear();

    //复制-自位置p起的n项
    void copyNodes(ListNodePosi(T)p, int n);

    //归并
    void merge(ListNodePosi(T)&p, int n, List<T> &L, ListNodePosi(T)q, int m);

    //归并排序
    void mergeSort(ListNodePosi(T)&p, int n);

    //选择排序
    void selectionSort(ListNodePosi(T)p, int n);

    //插入排序
    void insertionSort(ListNodePosi(T)p, int n);

public:
    //---------------构造函数---------------
    // 默认
    List() {
        init();
    }

    //整体复制列表
    List(List<T> const &L);

    //复制列表的指定区间
    List(List<T> const &L, Rank r, int n);

    //复制当前列表指定区间
    List(ListNodePosi(T)p, int n);

    //---------------析构---------------
    ~List();

    //---------------只读访问接口---------------
    Rank size() const { return _size; }

    bool empty() const { return _size <= 0; }

    //重载--支持按秩访问（效率低）
    T &operator[](Rank r) const;

    //首节点位置
    ListNodePosi(T)first() const { return header->succ; }

    //末节点位置
    ListNodePosi(T)last() const { return trailer->pred; }

    //判断位置p是否合法
    bool valid(ListNodePosi(T)p) {
        return p && (trailer != p) && (header != p);
    }

    //是否已排序
    int disordered() const;

    //无序列表查找
    ListNodePosi(T)find(T const &e) const {
        return find(e, _size, trailer);
    }

    //e-命中元素
    //n-查找长度
    //p-起始位置
    ListNodePosi(T)find(T const &e, int n, ListNodePosi(T)p) const;

    //有序列表查找
    ListNodePosi(T)search(T const &e) const {
        return search(e, _size, trailer);
    }

    ListNodePosi(T)search(T const &e, int n, ListNodePosi(T)p) const;

    //选出最大者
    ListNodePosi(T)selectMax() {
        return selectMax(header->succ, _size);
    }

    ListNodePosi(T)selectMax(ListNodePosi(T)p, int n);

    //---------------可写访问接口---------------
    ListNodePosi(T)insertAsFirst(T const &e);

    ListNodePosi(T)insertAsLast(T const &e);

    //e作为指定点后继插入
    ListNodePosi(T)insertS(ListNodePosi(T)p, T const &e);

    //e作为指定点前驱插入
    ListNodePosi(T)insertP(ListNodePosi(T)p, T const &e);

    T remove(ListNodePosi(T)p);

    //列表归并
    void merge(List<T> &L) {
        merge(first(), _size, L, L.first(), L.size);
    }

    void sort(ListNodePosi(T)p, int n);

    void sort() {
        sort(first(), _size);
    }

    //无序去重
    int deduplicate();

    //有序去重
    int uniquify();

    //前后倒置
    void reverse();

    //遍历，依次执行visit操作（函数指针，只读或局部性修改）
    void traverse(void (*)(T &));

    //遍历，以此执行visit操作（函数对象，可全局性修改）
    template<typename VST>
    void traverse(VST &);

};

template<typename T>
void List<T>::init() {
    header = new ListNode<T>;
    trailer = new ListNode<T>;
    header->succ = trailer;
    header->pred = NULL;
    trailer->pred = header;
    trailer->succ = NULL;
    _size = 0;
}

template<typename T>
T &List<T>::operator[](Rank r) const {
    ListNodePosi(T)p = first();
    while (0 < r--) p = p->succ;
    return p->data;
}

template<typename T>
ListNodePosi(T)List<T>::find(T const &e, int n, ListNodePosi(T)p) const {
    while (0 < n--) {
        if (e == (p = p->pred)->data) return p;//逐个对比，直到找到或越界
    }
    return NULL;
}

//目前的写法，即使失败也会传回位置，这要求调用者在外部再进行验证
//e-插入元素、n-查找长度，p-起始位置
template<typename T>
ListNodePosi(T)List<T>::search(T const &e, int n, ListNodePosi(T)p) const {
    while (0 <= n--) {
        if (e >= (p = p->pred)->data) break;//当节点比目标大的时候，直接终止
    }
    return p;//返回查找终止时的位置
}

template<typename T>
ListNodePosi(T)List<T>::insertAsFirst(const T &e) {
    _size++;
    return header->insertAsSucc(e);
}

template<typename T>
ListNodePosi(T)List<T>::insertAsLast(const T &e) {
    _size++;
    return trailer->insertAsPred(e);
}

template<typename T>
ListNodePosi(T)List<T>::insertS(ListNodePosi(T)p, const T &e) {
    _size++;
    return p->insertAsSucc(e);
}

template<typename T>
ListNodePosi(T)List<T>::insertP(ListNodePosi(T)p, const T &e) {
    _size++;
    return p->insertAsPred(e);
}

template<typename T>
void List<T>::copyNodes(ListNodePosi(T)p, int n) {
    init();
    while (n--) {
        insertAsLast(p->data);
        p = p->succ;
    }
}

template<typename T>
List<T>::List(ListNodePosi(T)p, int n) {
    copyNodes(p, n);
}

template<typename T>
List<T>::List(List<T> const &L) {
    copyNodes(L.first(), L._size);
}

template<typename T>
List<T>::List(const List<T> &L, int r, int n) {
    copyNodes(L[r], n);
}

template<typename T>
T List<T>::remove(ListNodePosi(T)p) {
    //备份删除节点数据
    T e = p->data;
    //更新前驱、后继
    p->pred->succ = p->succ;
    p->succ->pred = p->pred;
    delete p;
    _size--;
    return e;
}

template<typename T>
List<T>::~List<T>() {
    clear();
    delete header;
    delete trailer;
}

template<typename T>
int List<T>::clear() {
    int oldSize = _size;
    while (0 < _size) remove(header->succ);
    return oldSize;
}

//O(n^2)
template<typename T>
int List<T>::deduplicate() {
    if (_size < 2) return 0;

    int oldSize = _size;
    ListNodePosi(T)p = header;//从头开始查找
    Rank r = 0;//已经查过的区域不需要查找
    while (trailer != (p = p->succ)) {//查找到尾结束
        ListNodePosi(T)q = find(p->data, r, p);//在A[0,r]区域中寻找p的重复节点
        q ? remove(q) : r++;//存在则重复点，继续查询；否则前往下一区域
    }

    return oldSize - _size;
}

//O(n)
template<typename T>
int List<T>::uniquify() {
    if (_size < 2) return 0;

    int oldSize = _size;
    ListNodePosi(T)p = first();
    ListNodePosi(T)q; //p的后继节点

    while (trailer != (q = p->succ)) {
        if (p->data != q->data) p = q; //互异，则向下
        else remove(q);//重复，则删除
    }

    return oldSize - _size;
}

template<typename T>
void List<T>::traverse(void (*visit)(T &)) {
    for (ListNodePosi(T)p = header->succ; p != trailer; p = p->succ) {
        visit(p->data);
    }
}

template<typename T>
template<typename VST>
void List<T>::traverse(VST &visit) {
    for (ListNodePosi(T)p = header->succ; p != trailer; p = p->succ) {
        visit(p->data);
    }
}

template<typename T>
void List<T>::sort(ListNodePosi(T)p, int n) {
//    insertionSort(p, n);
//    selectionSort(p, n);
    mergeSort(p, n);
}

//O(n^2)
template<typename T>
void List<T>::insertionSort(ListNodePosi(T)p, int n) {
    for (int r = 0; r < n; r++) {
        //查找合适的位置，插入元素
        insertS(search(p->data, r, p), p->data);
        //转向下一节点
        p = p->succ;
        //删除原来的节点
        remove(p->pred);
    }
}

//O(n^2)
template<typename T>
void List<T>::selectionSort(ListNodePosi(T)p, int n) {
    ListNodePosi(T)head = p->pred;
    ListNodePosi(T)tail = p;
    for (int i = 0; i < n; i++) tail = tail->succ;//此时待排序区间为(head,tail)
    while (1 < n) { //区间为1时则无需选择
        ListNodePosi(T)max = selectMax(head->succ, n);
        insertP(tail, remove(max));
        tail = tail->pred;
        n--;
    }
}

template<typename T>
ListNodePosi(T)List<T>::selectMax(ListNodePosi(T)p, int n) {
    ListNodePosi(T)max = p;//暂定最大节点为首节点p
    for (ListNodePosi(T)cur = p; 1 < n; n--) {
        T curData = (cur = cur->succ)->data;
        T maxData = max->data;
        if (curData > maxData) max = cur;
    }
    return max;
}

//当前列表自p起的n个元素，与列表L中自q起的m个元素归并
template<typename T>
void List<T>::merge(ListNodePosi(T)&p, int n, List<T> &L, ListNodePosi(T)q, int m) {
    ListNodePosi(T)pp = p->pred;
    while (0 < m) {//说明m区间还有待归并元素
        if ((0 < n) && (p->data <= q->data)) {//n区间还有待归并元素，且v(p) 小于等于 v(q)
            p = p->succ;//选择p后继作为下一个基准点
            if (q == p) break; //起点和终点相等，则证明已经排序结束
            n--;
        } else {
            //如果v(p) 大于 v(q)
            //则将q插入p的后继
            //将q后继作为基准点
            insertP(p, L.remove((q = q->succ)->pred));
            m--;
        }
    }
    p = pp->succ;//将p还原至初始起点【此时列表已经是有序序列】
}

template<typename T>
void List<T>::mergeSort(ListNodePosi(T)&p, int n) {
    if (n < 2) return;
    int m = n >> 1;
    ListNodePosi(T)q = p;
    for (int i = 0; i < m; i++) q = q->succ;
    mergeSort(p, m);
    mergeSort(q, n - m);
    //p是起点，q是终点
    merge(p, m, *this, q, n - m);
}

#endif //FRONTEND_TARGET_LIST_H

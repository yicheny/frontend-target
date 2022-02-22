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
    void merge(ListNodePosi(T)&, int, List<T> &, ListNodePosi(T), int);

    //归并排序
    void mergeSort(ListNodePosi(T)&, int);

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

}

template<typename T>
void List<T>::insertionSort(ListNodePosi(T)p, int n) {
    for (int r = 0; r < n; r++) {
        insertS(search(p->data, r, p), p->data);
        p = p->succ;
        remove(p->pred);
    }
}
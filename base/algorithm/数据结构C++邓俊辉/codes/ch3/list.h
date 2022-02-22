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

    //复制
    void copyNodes(ListNodePosi(T), int);

    //归并
    void merge(ListNodePosi(T)&, int, List<T> &, ListNodePosi(T), int);

    //归并排序
    void mergeSort(ListNodePosi(T)&, int);

    //选择排序
    void selectionSort(ListNodePosi(T), int);

    //插入排序
    void insertionSort(ListNodePosi(T), int);

public:
    //---------------构造函数---------------
    // 默认
    List() {
        init();
    }

    //复制相关
    List(List<T> const &L);

    List(List<T> const &L, Rank r, int n);

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
        return find(e, _size, trailer)
    }

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
    ListNodePosi(T)insertA(ListNodePosi(T)p, T const &e);

    //e作为指定点前驱插入
    ListNodePosi(T)insertB(ListNodePosi(T)p, T const &e);

    T remove(ListNodePosi(T)p);

    //列表归并
    void merge(List<T> &L) {
        merge(first(), _size, L, L.first(), L.size);
    }

    void sort(ListNodePosi(T)p,int n);
    void sort(){
        sort(first(),_size);
    }

    //无序去重
    int deduplicate();

    //有序去重
    int uniquify();

    //前后倒置
    void reverse();

    //遍历，依次执行visit操作（函数指针，只读或局部性修改）
    void traverse(void (*) (T&));
    //遍历，以此执行visit操作（函数对象，可全局性修改）
    template <typename VST>
    void traverse(VST&);

};

template <typename T>
void List<T>::init() {
    header = new ListNode<T>;
    trailer = new ListNode<T>;
    header->succ = trailer;
    header->pred = NULL;
    trailer->pred = header;
    trailer->succ = NULL;
    _size = 0;
}

template <typename T>
T &List<T>::operator[](Rank r) const {
    ListNodePosi(T) p = first();
    while(0<r--) p = p->succ;
    return p->data;
}
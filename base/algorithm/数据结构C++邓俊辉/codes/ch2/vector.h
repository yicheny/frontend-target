#include <iostream>

typedef int Rank;
#define DEFAULT_CAPACITY 3 //默认初始容量

template<typename T>
class Vector {
protected:
    Rank _size;//规模【填充长度】
    int _capacity;//容量【申请长度】
    T *_elem;//数据项
    void expand();

    void shrink();

public:
    //-----------构造函数-----------
    /*
     * @brief 构造函数
     * @param c 声明长度【容量】
     * @param s 填充长度【规模】，注意：此数值应当小于等于声明长度
     * @param v 默认值
     * */
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = 0) {
        _elem = new T[_capacity = c];
        for (_size = 0; _size < s; _elem[_size++] = v);
    }

    //数组整体复制
    Vector(T const *A, Rank n) { copyFrom(A, 0, n); }

    //数组区间复制
    Vector(T const *A, Rank lo, Rank hi) { copyFrom(A, lo, hi); }

    //向量整体复制
    Vector(Vector<T> const &V) { copyFrom(V._elem, 0, V._size); }

    //向量区间复制
    Vector(Vector<T> const &V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); }

    void copyFrom(T const *A, Rank lo, Rank hi);

    //-----------析构函数-----------
    ~Vector() { delete[] _elem; }

    //-----------只读访问接口-----------
    Rank size() const { return _size; }

    bool empty() const { return !_size; }

    Rank find(T const &e) { return find(e, 0, _size); }

    Rank find(T const &e, Rank lo, Rank hi) const;

    //-----------可写访问接口-----------

    //重载下标操作符，支持形如A[i]引用元素
    T &operator[](Rank r) const { return _elem[r]; }

    Vector<T> &operator=(Vector<T> const &);

    //插入
    Rank insert(T const &e) { return insert(_size, e); }

    Rank insert(Rank r, T const &e);

    //置乱
    void unsort(Rank lo, Rank hi);

    void unsort() { unsort(0, _size); }

    //删除
    T remove(Rank r);//删除秩为r的元素
    int remove(Rank lo, Rank hi);//删除区间[lo,hi)的元素

    //排序
    void sort(Rank lo, Rank hi);

    void sort() { sort(0, _size); }

    //去重
    int deduplicate();//无序去重
    int uniquify();//有序去重

    //遍历
    void traverse(void (*) (T&));//使用函数指针，只读或局部性修改

    template<typename VST>void traverse(VST&);//使用函数对象，可全局性修改
};

template<typename T>
Rank Vector<T>::insert(Rank r, T const &e) {
    expand();
    for (int i = _size; i > r; i--) _elem[i] = _elem[i - 1];//自后向前，依次顺移一位
    _elem[r] = e;
    _size++;
    return r;//返回秩
}

template<typename T>
void Vector<T>::copyFrom(T const *A, Rank lo, Rank hi) {
    _elem = new T[_capacity = 2 * (hi - lo)];//以区间规模的2倍申请容量
    _size = 0;//规模初始化

    //A[lo,hi) => _elem[0,hi-lo)
    while (lo < hi)
        _elem[_size++] = A[lo++];
}

template<typename T>
Vector<T> &Vector<T>::operator=(Vector<T> const &V) {
    if (_elem) delete[] _elem;//释放原有内容
    copyFrom(V._elem, 0, V.size());//整体复制
    return *this;//返回当前对象引用，以便链式赋值
}

template<typename T>
void Vector<T>::expand() {
    if (_size < _capacity) return;
    if (_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY;
    T *oldElem = _elem;
    _elem = new T[_capacity << 2];//容量加倍
    for (int i = 0; i < _size; i++) _elem[i] = oldElem[i];
    delete[] oldElem;
}

template<typename T>
void Vector<T>::shrink() {
    if (_capacity < DEFAULT_CAPACITY << 1) return;
    if (_size << 2 > _capacity) return;//以25%为界
    T *oldElem = _elem;
    _elem = new T[_capacity >>= 1];//容量减半
    for (int i = 0; i < _size; i++) _elem[i] = oldElem[i];
    delete[] oldElem;
}

template<typename T>
void Vector<T>::unsort(Rank lo, Rank hi) {
    T *V = _elem + lo;//将[lo,hi)视作[0,hi-lo)
    for (int i = hi - lo; i > 0; i--)
        std::swap(V[i - 1], V[std::rand() % i]);//V[i,0]与V[0,i)中某一元素随机交换
}

template<typename T>
void permute(Vector<T> &V) {
    for (int i = V.size(); i > 0; i--)
        std::swap(V[i - 1], V[std::rand() % i]);//V[i,0]与V[0,i)中某一元素随机交换
}

template<typename T>
Rank Vector<T>::find(const T &e, Rank lo, Rank hi) const {
    while ((lo < hi--) && (e != _elem[hi]));//从后向前顺序查找
    return hi;//找到则返回指定索引，否则返回-1
}

template<typename T>
int Vector<T>::remove(Rank lo, Rank hi) {
    if (lo == hi) return 0;//出于效率方面的考虑，单独处理退化的情况
    while (hi < _size) _elem[lo++] = _elem[hi++];//将[hi,_size)开始依次推移
    _size = lo;//更新规模，丢弃尾部[lo,_size=hi)的内容--注：此时lo已增长为删除后的长度
    shrink();
    return hi - lo;//返回被删除元素的数量
}

template<typename T>
T Vector<T>::remove(Rank r) {
    T e = _elem[r];
    remove(r, r + 1);
    return e;
}

template<typename T>
int Vector<T>::deduplicate() {
    int oldSize = _size;
    Rank i = 1;//从1开始查重
    while (i < _size)
        find(_elem[i],0,i)<0 ? i++ : remove(i);
    return oldSize - _size;
}

template <typename T>
void Vector<T>::traverse(void (*visit)(T &)) {
    for(int i = 0; i< _size;i++) visit(_elem[i]);
}

template <typename T>
template <typename VST>
void Vector<T>::traverse(VST & visit) {
    for(int i = 0; i< _size;i++) visit(_elem[i]);
}
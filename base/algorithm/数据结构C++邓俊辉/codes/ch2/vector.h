typedef int Rank;
#define DEFAULT_CAPACITY 3 //默认初始容量

template<typename T> class Vector {
protected:
    Rank _size;//规模【填充长度】
    int _capacity;//容量【申请长度】
    T* _elem;//数据项
public:
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

    /*
     * @brief 通过下标获取元素
     * @parma i 元素下标
     * @return T 下标对应的元素
     * */
    T getByIndex(Rank i) const {
        return _elem[i];
    }

    //只读访问接口
    Rank size() const { return _size; }
};


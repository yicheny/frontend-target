#ifndef C___PROJECT_QUEUE_H
#define C___PROJECT_QUEUE_H


#include "./list.h"

//基于List实现
template<typename T>
class Queue : public List<T> {
public:
    void enqueue(T const &e) {
        this->insertAsLast(e);
    }

    T dequeue() {
        return this->remove(this->first());
    }

    T &front() {
        return this->first()->data;
    }
};

//基于Vector实现
/*
#include "./vector.h"

template<typename T>
class Queue : public Vector<T> {
public:
    void enqueue(T const &e) {
        this->insert(e);
    }

    T dequeue() {
        return this->remove(0);
    }

    T &front() {
        return (*this)[0];
    }
};
*/

#endif //C___PROJECT_QUEUE_H

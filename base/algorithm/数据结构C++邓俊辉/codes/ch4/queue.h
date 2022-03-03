#ifndef C___PROJECT_QUEUE_H
#define C___PROJECT_QUEUE_H

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

#endif //C___PROJECT_QUEUE_H

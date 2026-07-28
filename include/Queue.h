#pragma once
#include "./LinkedList.h"

template<class type>
class Queue {
public:
    Queue() {
        _size = 0;
    }

    Queue(const Queue<type>& other) {
        list = other.list;
        _size = other._size;
    }

    Queue<type>& operator=(const Queue<type>& other) {
        list = other.list;
        _size = other._size;
        return *this;
    }

    void put(type data) {
        list.push_back(data);
        _size++;
    }

    type get() {
        type data = list.front();
        list.pop_front();
        _size--;
        return data;
    }

    type top() {
        return list.front();
    }

    bool empty() {
        return list.empty();
    }

    int size() {
        return _size;
    }

private:
    LinkedList<type> list;
    int _size;
};
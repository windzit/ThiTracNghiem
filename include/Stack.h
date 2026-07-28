#pragma once
#include "./LinkedList.h"

template <class type>
class Stack {
public:
    Stack()
    {
        _size = 0;
    }

    Stack(const Stack<type>& other) {
        list = other.list;
        _size = other._size;
    }

    Stack<type>& operator=(const Stack<type>& other) {
        list = other.list;
        _size = other._size;
        return *this;
    }

    void append(type data)
    {
        list.push_back(data);
        size++;
    }

    type pop()
    {
        type temp = (type)list.back();
        list.pop_back();
        size--;
        return temp;
    }

    int size() {
        return _size;
    }

    bool empty()
    {
        return list.empty();
    }

private:
    LinkedList<type> list;
    int _size;
};

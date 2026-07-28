#pragma once

template <class type>
class DArray {
public:
    DArray()
    {
        data = new type[1];
        capacity = 1;
        count = 0;
    }

    ~DArray()
    {
        delete[] data;
    }

    DArray(const DArray<type>& other)
    {
        data = new type[other.capacity];
        capacity = other.capacity;
        count = other.count;
        for (int i = 0; i < count; i++)
        {
            data[i] = other.data[i];
        }
    }

    DArray& operator=(const DArray<type>& other)
    {
        if (this != &other)
        {
            delete[] data;
            data = new type[other.capacity];
            capacity = other.capacity;
            count = other.count;
            for (int i = 0; i < count; i++)
            {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    type& operator[](int index)
    {
        if (index < 0 || index >= count)
            throw std::out_of_range("DArray index out of range");

        return data[index];
    }

    const type& operator[](int index) const
    {
        if (index < 0 || index >= count)
            throw std::out_of_range("DArray index out of range");

        return data[index];
    }

    void push_back(type value)
    {
        if (count == capacity)
        {
            resize(capacity * 2);
        }
        data[count] = value;
        count++;
    }

    void pop_back()
    {
        if (count > 0)
        {
            count--;
        }
    }

    void remove(int index) {
        if (index < 0 || index >= count) return;
        for (int i = index; i < count - 1; i++) {
            data[i] = data[i + 1];
        }
        count--;
    }

    type& back()
    {
        return data[count - 1];
    }

    int size() const 
    {
        return count;
    }

    void resize(int new_size)
    {
        if (new_size <= 0) {
            clear();
            return;
        }
        type* temp = new type[new_size];

        count = new_size > count ? count : new_size;
        for (int i = 0; i < count; i++)
        {
            temp[i] = data[i];
        }
        delete[] data;
        data = temp;
        capacity = new_size;
    }

    void clear()
    {
        count = 0;
    }

    bool empty() const
    {
        return count == 0;
    }

    bool contains(const type& value) const
    {
        for (int i = 0; i < count; i++)
        {
            if (data[i] == value)
                return true;
        }
        return false;
    }

    void reserve(int new_capacity)
    {
        if (new_capacity > capacity)
        {
            type* temp = new type[new_capacity];
            for (int i = 0; i < count; i++)
            {
                temp[i] = data[i];
            }
            delete[] data;
            data = temp;
            capacity = new_capacity;
        }
    }



    type* begin()
    {
        return data;
    }

    type* end()
    {
        return data + count;
    }

    const type* begin() const
    {
        return data;
    }

    const type* end() const
    {
        return data + count;
    }

private:
    type* data;
    int capacity;
    int count;
};
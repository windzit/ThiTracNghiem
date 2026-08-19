#pragma once
#include <stdexcept>
#include <utility>

template <class type>
class DArray {
public:
    // 1. Constructor mặc định
    DArray()
    {
        capacity = 1;
        count = 0;
        data = new type[capacity];
    }

    // 2. Destructor
    ~DArray()
    {
        delete[] data;
    }

    // 3. Copy Constructor (Deep Copy)
    DArray(const DArray<type>& other)
    {
        capacity = other.capacity;
        count = other.count;
        data = new type[capacity];
        for (int i = 0; i < count; i++)
        {
            data[i] = other.data[i];
        }
    }

    // 4. Copy Assignment Operator
    DArray& operator=(const DArray<type>& other)
    {
        if (this != &other)
        {
            delete[] data;
            capacity = other.capacity;
            count = other.count;
            data = new type[capacity];
            for (int i = 0; i < count; i++)
            {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    // 5. Move Constructor (C++11 Move Semantics - Tối ưu O(1))
    DArray(DArray<type&&> other) noexcept
    {
        data = other.data;
        capacity = other.capacity;
        count = other.count;

        other.data = nullptr;
        other.capacity = 0;
        other.count = 0;
    }

    // 6. Move Assignment Operator
    DArray& operator=(DArray<type>&& other) noexcept
    {
        if (this != &other)
        {
            delete[] data;

            data = other.data;
            capacity = other.capacity;
            count = other.count;

            other.data = nullptr;
            other.capacity = 0;
            other.count = 0;
        }
        return *this;
    }

    // Truy xuất phần tử có kiểm tra biên
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

    // Thêm phần tử (Tham chiếu hằng - Tránh copy thừa)
    void push_back(const type& value)
    {
        if (count == capacity)
        {
            reserve(capacity * 2);
        }
        data[count] = value;
        count++;
    }

    // Thêm phần tử (Move rvalue)
    void push_back(type&& value)
    {
        if (count == capacity)
        {
            reserve(capacity * 2);
        }
        data[count] = std::move(value);
        count++;
    }

    void pop_back()
    {
        if (count > 0)
        {
            count--;
        }
    }

    void remove(int index)
    {
        if (index < 0 || index >= count) return;
        for (int i = index; i < count - 1; i++)
        {
            data[i] = std::move(data[i + 1]);
        }
        count--;
    }

    type& back()
    {
        if (count == 0)
            throw std::out_of_range("DArray is empty");
        return data[count - 1];
    }

    const type& back() const
    {
        if (count == 0)
            throw std::out_of_range("DArray is empty");
        return data[count - 1];
    }

    int size() const 
    {
        return count;
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
                temp[i] = std::move(data[i]);
            }
            delete[] data;
            data = temp;
            capacity = new_capacity;
        }
    }

    void resize(int new_size)
    {
        if (new_size <= 0)
        {
            clear();
            return;
        }
        if (new_size > capacity)
        {
            reserve(new_size);
        }
        count = new_size;
    }

    // Iterators cho Range-based for loop
    type* begin() { return data; }
    type* end() { return data + count; }
    const type* begin() const { return data; }
    const type* end() const { return data + count; }

private:
    type* data;
    int capacity;
    int count;
};
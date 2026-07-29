#pragma once
#include <string>
#include <cstdint>

template <typename K, typename V>
struct HashNode {
    K key;
    V value;
    HashNode* next;

    HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
};

template <typename K, typename V>
class HashTable {
public:
    explicit HashTable(size_t bucketCount = 10007)
        : m_capacity(bucketCount), m_size(0) {
        m_buckets = new HashNode<K, V>*[m_capacity];
        for (size_t i = 0; i < m_capacity; i++) {
            m_buckets[i] = nullptr;
        }
    }

    ~HashTable() {
        clear();
        delete[] m_buckets;
    }

    HashTable(const HashTable& other) {
        m_capacity = other.m_capacity;
        m_size = 0;
        m_buckets = new HashNode<K, V>*[m_capacity];
        for (size_t i = 0; i < m_capacity; i++) m_buckets[i] = nullptr;
        copyFrom(other);
    }

    HashTable& operator=(const HashTable& other) {
        if (this == &other) return *this;
        clear();
        delete[] m_buckets;
        m_capacity = other.m_capacity;
        m_size = 0;
        m_buckets = new HashNode<K, V>*[m_capacity];
        for (size_t i = 0; i < m_capacity; i++) m_buckets[i] = nullptr;
        copyFrom(other);
        return *this;
    }

    void clear() {
        if (!m_buckets) return;
        for (size_t i = 0; i < m_capacity; i++) {
            HashNode<K, V>* cur = m_buckets[i];
            while (cur) {
                HashNode<K, V>* tmp = cur;
                cur = cur->next;
                delete tmp;
            }
            m_buckets[i] = nullptr;
        }
        m_size = 0;
    }

    size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }

    bool contains(const K& key) const {
        return find(key) != nullptr;
    }

    V* find(const K& key) {
        size_t idx = hashKey(key) % m_capacity;
        HashNode<K, V>* cur = m_buckets[idx];
        while (cur) {
            if (cur->key == key) {
                return &cur->value;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    const V* find(const K& key) const {
        size_t idx = hashKey(key) % m_capacity;
        HashNode<K, V>* cur = m_buckets[idx];
        while (cur) {
            if (cur->key == key) {
                return &cur->value;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    bool insert(const K& key, const V& value) {
        size_t idx = hashKey(key) % m_capacity;
        HashNode<K, V>* cur = m_buckets[idx];
        while (cur) {
            if (cur->key == key) {
                cur->value = value;
                return true;
            }
            cur = cur->next;
        }
        HashNode<K, V>* newNode = new HashNode<K, V>(key, value);
        newNode->next = m_buckets[idx];
        m_buckets[idx] = newNode;
        m_size++;
        return true;
    }

    bool remove(const K& key) {
        size_t idx = hashKey(key) % m_capacity;
        HashNode<K, V>* cur = m_buckets[idx];
        HashNode<K, V>* prev = nullptr;

        while (cur) {
            if (cur->key == key) {
                if (prev) {
                    prev->next = cur->next;
                } else {
                    m_buckets[idx] = cur->next;
                }
                delete cur;
                m_size--;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    V& operator[](const K& key) {
        V* ptr = find(key);
        if (ptr) return *ptr;
        insert(key, V());
        return *find(key);
    }

    template <typename Function>
    void forEach(Function fn) const {
        for (size_t i = 0; i < m_capacity; i++) {
            HashNode<K, V>* cur = m_buckets[i];
            while (cur) {
                fn(cur->key, cur->value);
                cur = cur->next;
            }
        }
    }

private:
    size_t hashKey(int key) const {
        uint64_t x = static_cast<uint64_t>(key);
        x = ((x >> 16) ^ x) * 0x45d9f3bULL;
        x = ((x >> 16) ^ x) * 0x45d9f3bULL;
        x = (x >> 16) ^ x;
        return static_cast<size_t>(x);
    }

    size_t hashKey(const std::string& key) const {
        uint64_t hash = 14695981039346656037ULL;
        for (char c : key) {
            hash ^= static_cast<uint64_t>(c);
            hash *= 1099511628211ULL;
        }
        return static_cast<size_t>(hash);
    }

    void copyFrom(const HashTable& other) {
        other.forEach([this](const K& k, const V& v) {
            this->insert(k, v);
        });
    }

    size_t m_capacity;
    size_t m_size;
    HashNode<K, V>** m_buckets;
};

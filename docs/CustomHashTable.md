# Custom HashTable Implementation Specification

**Project:** ThiTracNghiem  
**Component:** `HashTable<K, V>` (`include/HashTable.h`)  
**Date:** July 29, 2026  

---

## 1. Why HashTable Was Implemented

The project follows a strict custom data structures policy for educational defense:
- Standard C++ STL containers such as `std::unordered_map`, `std::map`, and `std::set` are forbidden unless pre-existing.
- To maintain $O(1)$ fast-seek index performance for `Question` (`ID -> Offset`), `Student` (`MASV -> Offset`), and `ExamHistory` (`MASV -> List<Offset>`) without violating project rules, a custom generic `HashTable<K, V>` template class was implemented.

---

## 2. Why DArray Cannot Replace unordered_map for Storage Indexing

While `DArray<T>` is the project's custom dynamic array, using a dynamic array for primary key index lookups would require linear scanning:
- Searching a `DArray` of 1 Million records takes $O(N)$ comparisons ($\approx 1,000,000$ operations per lookup), destroying the $O(1)$ performance goal of the index.
- A `HashTable` provides $O(1)$ expected lookup complexity by hashing primary keys directly to bucket indices.

---

## 3. Collision Handling Strategy

`HashTable<K, V>` uses **Separate Chaining** with dynamically allocated linked nodes (`HashNode<K, V>*`).

- **Bucket Array:** Dynamic array of bucket pointers (`HashNode<K, V>** m_buckets`) with prime table capacity ($N_{buckets} = 10,007$).
- **Hash Functions:**
  - `std::string` keys: FNV-1a (Fowler–Noll–Vo) 64-bit hash algorithm.
  - `int` keys: 64-bit Bit-Mixer hash algorithm.
- **Collision Resolution:** When multiple keys produce the same `hash % capacity` index, new entries are prepended to the singly linked list at `m_buckets[index]` in $O(1)$ time.

---

## 4. Complexity Analysis

| Operation | Best Case | Average Case | Worst Case | Space Complexity |
| :--- | :--- | :--- | :--- | :--- |
| **Insert (`insert`)** | $O(1)$ | $O(1)$ | $O(N)$ (all hash collision) | $O(1)$ per node |
| **Lookup (`find`)** | $O(1)$ | $O(1)$ | $O(N)$ | $O(1)$ |
| **Remove (`remove`)** | $O(1)$ | $O(1)$ | $O(N)$ | $O(1)$ |
| **Contains (`contains`)**| $O(1)$ | $O(1)$ | $O(N)$ | $O(1)$ |

---

## 5. ASCII Memory & Collision Diagram

```
m_buckets (Capacity = 10007)
+------+
| [0]  | ──► nullptr
+------+
| [1]  | ──► [ Key: "N22DCCN001" | Val: 4096 ] ──► [ Key: "N22DCCN102" | Val: 8192 ] ──► nullptr
+------+
| [2]  | ──► nullptr
+------+
| ...  |
+------+
| [K]  | ──► [ Key: 105 | Val: 1048 ] ──► nullptr
+------+
```

---

## 6. Example Usage Operations

### 6.1 Example Insert
```cpp
HashTable<std::string, int64_t> studentIndex;
studentIndex.insert("N22DCCN001", 1048);
```
*Flow:* Computes `FNV1a("N22DCCN001") % 10007`. Allocates `HashNode("N22DCCN001", 1048)` and prepends to `m_buckets[hashIdx]`.

### 6.2 Example Lookup
```cpp
const int64_t* offsetPtr = studentIndex.find("N22DCCN001");
if (offsetPtr) {
    int64_t offset = *offsetPtr; // 1048
}
```
*Flow:* Hashes key, traverses linked list at `m_buckets[hashIdx]`, returns pointer to matching value in $O(1)$ time.

### 6.3 Example Remove
```cpp
studentIndex.remove("N22DCCN001");
```
*Flow:* Hashes key, locates node in bucket linked list, unlinks node from list, and calls `delete node`.

---

## 7. Memory Layout & Lifecycle Safety

- **Construction:** Allocates bucket pointer array `new HashNode<K, V>*[capacity]` initialized to `nullptr`.
- **Copy / Assignment:** Deep copy of bucket linked nodes using `copyFrom()`.
- **Destruction (`~HashTable`):** Traverses all bucket linked lists, deleting every `HashNode`, and frees bucket array `delete[] m_buckets`.

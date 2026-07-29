# ThiTracNghiem Project Glossary & Technical Terminology

**Project:** ThiTracNghiem  
**Target:** Quick Revision & Concepts Reference for Oral Defense  

---

## 1. Storage & Persistence Concepts

### StorageManager
- **Role:** Centralized coordinator for atomic disk file operations, validation, verification, and index synchronization.
- **Used In:** `src/StorageManager.cpp`, `include/StorageManager.h`, all HTTP Handlers.
- **Key Modules:** `StorageValidator`, `StorageVerifier`, `IndexManager`, `PathResolver`.
- **Significance:** Ensures database-like ACID safety (Atomic Write + Deep Read-Back Verification).

### IndexManager
- **Role:** High-performance index manager maintaining in-memory `HashTable` lookup tables for `.idx` fast file seeking.
- **Used In:** `src/IndexManager.cpp`, `include/IndexManager.h`, `ReportHandler.cpp`.
- **Significance:** Reduces $O(N)$ linear file scans to $O(1)$ direct byte-offset seek.

### PathResolver
- **Role:** Resolves absolute executable-relative directory paths (`storage/data/` and `storage/indexes/`).
- **Used In:** `src/PathResolver.cpp`, `include/PathResolver.h`.
- **Significance:** Guarantees working-directory independent file resolution.

### StorageValidator
- **Role:** Gatekeeper enforcing string normalization, delimiter checks (`|`, `\n`), range checks, and field rules.
- **Used In:** `src/StorageValidator.cpp`, `include/StorageValidator.h`.

### StorageVerifier
- **Role:** Performs deep verification after every disk write by parsing written `.txt` files back into RAM and asserting equality.
- **Used In:** `src/StorageVerifier.cpp`, `include/StorageVerifier.h`.

### StorageDeserializer
- **Role:** Static string tokenizer parsing pipe-delimited raw `.txt` lines into C++ domain structs.
- **Used In:** `src/StorageDeserializer.cpp`, `include/StorageDeserializer.h`.

---

## 2. Core Custom Data Structures

### HashTable<K, V>
- **Role:** Custom template Hash Table utilizing separate chaining (`HashNode<K,V>`) and FNV-1a hash algorithm.
- **Used In:** `include/HashTable.h`, `IndexManager.h`.
- **Capacity:** Fixed 10,007 buckets.

### DArray<T>
- **Role:** Custom dynamic array replacement for `std::vector<T>`.
- **Used In:** `include/DArray.h` across the entire codebase.

### LinkedList (`dsSinhVien` / `dsCHT` / `dsDiemThi`)
- **Role:** Custom singly-linked lists used for student list per class, questions per subject, and scores per student.

### Binary Search Tree (`Subject` / `NodeMH`)
- **Role:** Balanced/Ordered Binary Search Tree storing subjects ordered by `MAMH`.

---

## 3. Domain Entities

### Class (`Lop` / `dsLop`)
- **Fields:** `MALOP`, `TENLOP`, `dssinhvien` (LinkedList of students).
- **Structure:** Array of pointers (`dsLop`).

### Student (`SinhVien`)
- **Fields:** `MASV`, `HO`, `TEN`, `PHAI`, `passsword`, `dsdiemthi` (LinkedList of scores).

### Subject (`MonHoc` / `NodeMH`)
- **Fields:** `MAMH`, `TENMH`, `dsCauHoi` (LinkedList of questions).

### Question (`CauHoi` / `dsCHT`)
- **Fields:** `ID`, `NOIDUNG`, `A`, `B`, `C`, `D`, `DAPAN_DUNG`, `used`, `deleted`.

### Score (`DiemThi`)
- **Fields:** `MAMH`, `DIEM`.

---

## 4. Key Architectural Terms

### Soft Delete (`deleted = true`)
- **Meaning:** Marking a question as deleted without unlinking its node if it has already been used in previous student exams.

### Hard Delete
- **Meaning:** Complete removal and memory deallocation of an unused entity node.

### Offset
- **Meaning:** The exact zero-indexed byte position of a record line inside a physical `.txt` file used by `std::ifstream::seekg()`.

### Atomic Write
- **Meaning:** Writing data to a temporary file (`.tmp`) first and performing atomic file rename to prevent file corruption during crashes.

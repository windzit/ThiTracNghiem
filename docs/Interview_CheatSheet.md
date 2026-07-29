# ThiTracNghiem Oral Defense & Exam CheatSheet

**Project:** ThiTracNghiem (C++ REST Server & React Frontend)  
**Target:** Rapid Revision & Oral Defense High-Frequency Q&A  

---

## ⚡ 1. High-Frequency Defense Questions & Concise Answers

### Q1: Why did you implement a custom `HashTable` instead of using `std::unordered_map`?
> **Answer:** The project follows strict educational requirements banning STL containers (`std::vector`, `std::unordered_map`, `std::map`). We implemented a custom `HashTable<K,V>` using separate chaining (`HashNode<K,V>`) and FNV-1a hashing to provide $O(1)$ fast byte offset lookups for indexed file seeking.

### Q2: How does your storage subsystem achieve $O(1)$ fast file reads without a traditional SQL database?
> **Answer:** We implemented an **Indexed Text Storage Architecture**. Source-of-truth data is saved in readable pipe-delimited text files (`storage/data/*.txt`). Accelerator index files (`storage/indexes/*.idx`) store Primary Key to Byte Offset pairs loaded into in-memory `HashTable` lookup tables. To read a specific record, the server queries the `HashTable` for the byte offset and performs direct file seeking (`std::ifstream::seekg(offset)`) in $O(1)$ time.

### Q3: How do you prevent file corruption if the server crashes during a disk save operation?
> **Answer:** We use **Atomic File Writes**. Data is first written to a temporary file (`.tmp`). Once the file write completes and is verified, an atomic file rename (`std::filesystem::rename`) replaces the target data file. If a power cut or crash occurs mid-write, the original data file remains intact and uncorrupted.

### Q4: What is the difference between Soft Delete and Hard Delete in your system, and why is Soft Delete used for Questions?
> **Answer:**
> - **Hard Delete:** Unlinks the node from memory and deletes its record line from disk.
> - **Soft Delete (`deleted = true`):** Marks the record as deleted while preserving its node in memory and line on disk.
>
> **Why Soft Delete for Questions?** If a question has already been used in past student exams (`used = true`), hard deleting it would corrupt historical exam review reports. Soft deleting allows the question to be hidden from future exam generation while preserving historic exam integrity.

### Q5: How does your server ensure data integrity after writing to disk?
> **Answer:** After every disk write, `StorageManager` invokes `StorageVerifier`. `StorageVerifier` opens the newly written `.txt` file, deserializes its lines back into memory, and performs deep equality checks against RAM domain structures (verifying row counts, primary keys, and field contents). If verification fails, the operation is rolled back and an error is logged.

### Q6: What happens if an index file (`.idx`) is deleted or missing when the server starts?
> **Answer:** The system is **Self-Healing**. Physical data files (`.txt`) are the sole authoritative source of truth. If an `.idx` file is missing or corrupted on boot, `IndexManager` detects this, scans the physical `.txt` file, rebuilds the index table in memory, and writes a fresh `.idx` file to disk automatically.

---

## 🏛️ 2. Architectural Summaries for Defense Presentation

### Architecture Overview
1. **Frontend Layer:** React 18 SPA built with Vite, TypeScript, and TailwindCSS, making REST API calls via `axios`.
2. **Transport Layer:** Multithreaded C++ HTTP Server (`httplib::Server` on port 8080) handling JSON request/response payloads with CORS support.
3. **Application Handler Layer:** Handlers (`AuthHandler`, `StudentHandler`, `QuestionHandler`, `ExamHandler`, `ReportHandler`, `AdminHandler`) parsing requests, validating input, and executing business logic under thread-safe write locks (`DB_WRITE_LOCK`).
4. **In-Memory Domain Layer:** Memory structures maintaining state in RAM:
   - Class List (`dsLop`): Dynamic array of Class pointers.
   - Student List (`dsSinhVien`): Sorted singly-linked list of Students per Class.
   - Subject Tree (`NodeMH`): Binary Search Tree of Subjects ordered by `MAMH`.
   - Question List (`dsCHT`): Singly-linked list of Questions per Subject.
   - Score List (`dsDiemThi`): Singly-linked list of Scores per Student.
5. **Storage Layer:** Dual-layered storage with `storage/data/*.txt` files and `storage/indexes/*.idx` offset indexes.

---

## 📊 3. Core Data Structure Summary

| Entity / Component | Data Structure | Rationale & Properties |
| :--- | :--- | :--- |
| **Class List (`dsLop`)** | `DArray<Lop*>` | Array storing class pointers; fast index access |
| **Student List (`dsSinhVien`)** | Singly Linked List | Kept sorted by `MASV`; efficient node insertion/deletion |
| **Subject List (`NodeMH`)** | Binary Search Tree (BST) | Ordered by `MAMH`; $O(\log N)$ search & in-order traversal |
| **Question List (`dsCHT`)** | Singly Linked List | Appended per subject; supports `used` and `deleted` flags |
| **Score List (`dsDiemThi`)** | Singly Linked List | Stores exam scores per student; prevents duplicate subject exams |
| **Index Accelerator** | Custom `HashTable<K,V>` | Separate chaining + FNV-1a hash; $O(1)$ primary key $\rightarrow$ byte offset lookup |

---

## 🔑 4. Key Design Trade-Offs & Defense Justifications

1. **Why In-Memory Domain Trees + Disk Backup?**
   - *Justification:* In-memory operations provide microsecond latency for exam generation and scoring without disk bottlenecks during active exam taking.

2. **Why Plain-Text Files instead of Binary Format?**
   - *Justification:* Text files (`.txt`) are human-readable, auditable, easy to seed/reset, and robust against binary platform alignment issues.

3. **Why Custom Containers instead of STL?**
   - *Justification:* Demonstrates foundational data structure implementation skills (memory allocation, pointer manipulation, custom hashing, dynamic array expansion) required for computer science curriculum.

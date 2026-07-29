# ThiTracNghiem Codebase Learning & Defense Roadmap

**Project:** ThiTracNghiem C++ REST Backend & React Frontend  
**Goal:** Complete Architectural Mastery & Exam Defense Preparedness  
**Cross-References:** [Architecture Map](Architecture_Map.md) | [Glossary](Glossary.md) | [Interview CheatSheet](Interview_CheatSheet.md)  

---

## 🗺️ Learning Roadmap Overview

```
+-----------------------------------------------------------------------------------+
|  PHASE 1: Core Data Structures & Custom Containers (DArray, HashTable, BST, List)|
+----------------------------------------│------------------------------------------+
                                         ▼
+-----------------------------------------------------------------------------------+
|  PHASE 2: Storage Infrastructure & Fast Index Seeking (PathResolver, Index, Mgr)  |
+----------------------------------------│------------------------------------------+
                                         ▼
+-----------------------------------------------------------------------------------+
|  PHASE 3: Server Bootstrap, Lifecycle & HTTP Routing (ServerBootstrap, Handlers)  |
+----------------------------------------│------------------------------------------+
                                         ▼
+-----------------------------------------------------------------------------------+
|  PHASE 4: Domain Models & Business Rules (Class, Student, Subject, Question)      |
+----------------------------------------│------------------------------------------+
                                         ▼
+-----------------------------------------------------------------------------------+
|  PHASE 5: Exam Engine, Scoring & Report Subsystem (ExamSession, History, Reports) |
+----------------------------------------│------------------------------------------+
                                         ▼
+-----------------------------------------------------------------------------------+
|  PHASE 6: End-to-End Frontend Integration & Defense Mock Exams                    |
+-----------------------------------------------------------------------------------+
```

---

## 📌 Phase 1: Core Custom Data Structures & Memory Layout

### Objectives
Master custom container implementations (`DArray<T>`, `HashTable<K,V>`, `dsLop`, `dsSinhVien`, `NodeMH` BST) built under the strict "No STL Containers/Algorithms" policy.

### Files to Read (Strict Order)
1. `include/DArray.h` — Custom dynamic array implementation
2. `include/HashTable.h` — Custom separate-chaining Hash Table
3. `include/Student.h` & `src/Student.cpp` — Student linked list (`dsSinhVien`)
4. `include/Class.h` & `src/Class.cpp` — Class array (`dsLop`)
5. `include/Subject.h` & `src/Subject.cpp` — Subject Binary Search Tree (`NodeMH`)

### Rationale for Reading Order
Containers are the foundational building blocks. Storage, Handlers, and API layers all operate directly on these domain models.

### Key Concepts & ASCII Diagram
```
[HashTable<std::string, int64_t>]
  Key: "N22DCCN001" ──► FNV-1a Hash ──► Bucket Index: hash % 10007
                                                 │
                                                 ▼
                                        [HashNode<K,V>] ──► [HashNode<K,V>]
```

### Checkpoint Interview (Phase 1)
- **10 Theory Qs:**
  1. Why did the project replace `std::vector` with `DArray`?
  2. How does `HashTable` handle key collisions?
  3. What is the initial capacity and growth factor of `DArray`?
  4. How does `Subject` BST maintain in-order traversal for subject listings?
  5. Explain the copy constructor memory management in `Score` and `SinhVien`.
  6. Why is `dsSinhVien` sorted by `MASV`?
  7. What is the time complexity of searching a student in a class?
  8. What is FNV-1a hashing?
  9. How does `DArray::reserve()` handle memory re-allocation safely?
  10. Why is `delete[] data` called during `DArray` destruction?
- **5 "Why" Qs:**
  1. Why is `HashTable` bucket count set to a prime number ($10,007$)?
  2. Why does `SinhVien` hold a nested `Score` object instead of a pointer?
  3. Why are subject nodes organized in a BST rather than an array?
  4. Why is `used` flag needed in `CauHoi`?
  5. Why is `deleted` boolean flag used instead of immediate deletion?
- **3 ASCII Questions:** Draw `DArray` expansion, `HashTable` bucket collision, and `Subject` BST node insertion.
- **2 Data Flow Questions:** Trace `Student::insert()` and `Subject::insert()`.
- **1 Re-implementation Exercise:** Write a minimal template `HashTable` class with `insert()` and `find()`.

---

## 📌 Phase 2: Storage Infrastructure & Fast Index Seeking

### Objectives
Understand the dual-layer storage architecture (`storage/data/*.txt` and `storage/indexes/*.idx`), `PathResolver`, `StorageManager`, `IndexManager`, and atomic write semantics.

### Files to Read (Strict Order)
1. `include/PathResolver.h` & `src/PathResolver.cpp`
2. `include/IndexManager.h` & `src/IndexManager.cpp`
3. `include/StorageValidator.h` & `src/StorageValidator.cpp`
4. `include/StorageVerifier.h` & `src/StorageVerifier.cpp`
5. `include/StorageManager.h` & `src/StorageManager.cpp`

### Checkpoint Interview (Phase 2)
- **10 Theory Qs:**
  1. Difference between `storage/data/` and `storage/indexes/`?
  2. What happens if an `.idx` file is deleted or corrupt on server boot?
  3. Explain Atomic File Write using `.tmp` files.
  4. How does `IndexManager::getHistoryOffsets(masv)` retrieve multiple offset entries?
  5. What is the role of `StorageVerifier`?
  6. Why does `PathResolver::init(argv[0])` execute before storage loading?
  7. How does `StorageValidator` prevent delimiter corruption (`|`)?
  8. Explain how `seekg()` works with stored byte offsets.
  9. Why is `dirty` flag used in `StorageManager`?
  10. What is stored in `metadata.txt`?
- **5 "Why" Qs:**
  1. Why are `.idx` files non-authoritative?
  2. Why use atomic temporary file write before renaming?
  3. Why do we need `verifyStudents` after writing `students.txt`?
  4. Why is `PathResolver` implemented as a static singleton?
  5. Why store offsets as 64-bit integers (`int64_t`)?
- **3 ASCII Questions:** Draw Storage Seek Pipeline, Atomic Write Sequence, and Index Rebuild Flow.
- **2 Data Flow Questions:** Trace `StorageManager::saveStudents()` and `IndexManager::loadQuestionIndex()`.
- **1 Re-implementation Exercise:** Implement an atomic file write helper function that writes text via a `.tmp` file and renames it.

---

## 📌 Phase 3: Server Bootstrap, Lifecycle & HTTP Routing

### Objectives
Understand `httplib::Server` integration, endpoint registration in `RouteRegistry.cpp`, and server startup/shutdown cycles.

### Files to Read (Strict Order)
1. `server.cpp` & `server/ServerBootstrap.h` & `src/ServerBootstrap.cpp`
2. `server/ServerContext.h` & `server/ServerContext.cpp`
3. `server/RouteRegistry.h` & `server/RouteRegistry.cpp`
4. `server/handlers/AuthHandler.cpp`
5. `server/handlers/AdminHandler.cpp`

### Checkpoint Interview (Phase 3)
- **10 Theory Qs:**
  1. How are HTTP routes registered in `RouteRegistry.cpp`?
  2. What is `DB_WRITE_LOCK` and why is it used?
  3. What CORS headers are set in API responses?
  4. How does `adjustSessionsForDowntime()` clean up expired exam sessions on boot?
  5. Explain `POST /api/admin/save` execution flow.
  6. How are CLI arguments parsed during boot?
  7. What is the role of `ServerContext`?
  8. How does `AuthHandler` handle teacher vs student login?
  9. What status code is returned when login fails?
  10. How are JSON request bodies parsed using `nlohmann::json`?
- **5 "Why" Qs:**
  1. Why is `DB_WRITE_LOCK` required in a multithreaded HTTP server?
  2. Why keep session tokens in memory instead of disk?
  3. Why does `ServerBootstrap` execute `loadAllData` before `svr.listen()`?
  4. Why return CORS headers on every response?
  5. Why use REST JSON endpoints instead of server-side HTML rendering?
- **3 ASCII Questions:** Draw Server Startup Flow, Route Dispatching Flow, and Authentication Flow.
- **2 Data Flow Questions:** Trace `POST /api/login` and `POST /api/admin/save`.
- **1 Re-implementation Exercise:** Write an HTTP handler function that parses JSON input, validates fields, and returns a JSON response.

---

## 📌 Phase 4: Domain Models & Business Rules CRUD Operations

### Objectives
Master business logic, validation rules, and CRUD handlers for Student, Class, Subject, and Question entities.

### Files to Read (Strict Order)
1. `server/handlers/ClassHandler.cpp`
2. `server/handlers/StudentHandler.cpp`
3. `server/handlers/SubjectHandler.cpp`
4. `server/handlers/QuestionHandler.cpp`

### Checkpoint Interview (Phase 4)
- **10 Theory Qs:**
  1. What happens when deleting a class that still has students?
  2. Explain the difference between Soft Delete and Hard Delete for questions.
  3. What validation rules apply to Student IDs (`MASV`)?
  4. How is question ID auto-incremented?
  5. Can a subject be deleted if it contains questions?
  6. How does `StringNormalizer` clean up student names?
  7. What status code is returned when creating a duplicate Class ID?
  8. How are questions retrieved for a specific subject?
  9. What happens when updating a student's class ID?
  10. How does `findStudentGlobal` search across all classes?
- **5 "Why" Qs:**
  1. Why is Soft Delete necessary for questions?
  2. Why prevent deleting a class with students?
  3. Why normalize Vietnamese names before saving?
  4. Why store questions inside a linked list in each Subject BST node?
  5. Why lock the database during question deletion?
- **3 ASCII Questions:** Draw Student Creation Flow, Question Soft Delete Flow, and Subject Deletion Check Flow.
- **2 Data Flow Questions:** Trace `DELETE /api/questions/:id` and `POST /api/students`.
- **1 Re-implementation Exercise:** Implement a handler function for updating subject details with full error checking.

---

## 📌 Phase 5: Exam Engine, Scoring & Report Subsystem

### Objectives
Master exam session sampling, live session tracking, grading algorithms, exam history appending, and cross-tabulated scoreboard reporting.

### Files to Read (Strict Order)
1. `include/Exam.h` & `src/Exam.cpp`
2. `server/handlers/ExamHandler.cpp`
3. `src/Report.cpp` & `server/handlers/ReportHandler.cpp`

### Checkpoint Interview (Phase 5)
- **10 Theory Qs:**
  1. How are random questions sampled when starting an exam?
  2. Where are active exam sessions stored during an exam?
  3. How is an exam scored upon submission?
  4. What happens when a student tries to take an exam for a subject they already completed?
  5. How does `appendExamHistory()` record student answers?
  6. Explain how `rebuildUsedFlags()` updates question usage state.
  7. How does `ReportHandler` construct the class scoreboard?
  8. How does `IndexManager` accelerate student exam history report lookup?
  9. What happens if the server restarts while a student is taking an exam?
  10. How is remaining exam time calculated?
- **5 "Why" Qs:**
  1. Why record exam sessions to `exam_sessions.txt` during the exam?
  2. Why mark questions as `used=true` after exam submission?
  3. Why lock student exam retakes per subject?
  4. Why use byte offset indexing for exam history reports?
  5. Why calculate score rounding to 2 decimal places?
- **3 ASCII Questions:** Draw Exam Lifecycle Flow, Score Submission Flow, and Exam History Report Index Seek Flow.
- **2 Data Flow Questions:** Trace `POST /api/exams/start` and `POST /api/exams/submit`.
- **1 Re-implementation Exercise:** Write an exam grading function that computes scores from student answers and correct answer keys.

---

## 📌 Phase 6: Frontend Integration & Oral Defense Preparation

### Objectives
Connect React frontend service calls to C++ REST endpoints and prepare for oral examination questions.

### Files & References to Study
1. `frontend/src/services/` (`authService.ts`, `studentService.ts`, `examService.ts`, etc.)
2. `docs/Interview_CheatSheet.md`
3. `docs/Architecture_Map.md`

### Final Defense Readiness Criteria
- Ability to draw any of the 16 ASCII diagrams from `Architecture_Map.md` on a whiteboard without reference.
- Ability to explain every API endpoint, its handler, and its storage impact.
- Ability to explain all architectural trade-offs (Custom Containers, Indexed Text Storage, Soft Delete).

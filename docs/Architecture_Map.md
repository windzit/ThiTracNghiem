# ThiTracNghiem System Architecture Map

**Project:** ThiTracNghiem (C++ HTTP Backend & React Frontend)  
**Target:** Oral Defense & Architectural Understanding  
**Status:** Authoritative Architectural Specification  

---

## 1. System Overview Flow

```
+-----------------------------------------------------------------------------------+
|                                  USER INTERFACE                                   |
|   +---------------------------------------------------------------------------+   |
|   |                          React 18 Frontend (Vite)                         |   |
|   |   Pages: Login, Classes, Students, Subjects, Questions, Exam, Reports     |   |
|   +---------------------------------------------------------------------------+   |
+------------------------------------------│----------------------------------------+
                                           │ HTTP REST Requests (JSON)
                                           ▼
+-----------------------------------------------------------------------------------+
|                                 C++ HTTP BACKEND                                  |
|   +---------------------------------------------------------------------------+   |
|   |              httplib::Server (Port 8080) & RouteRegistry                  |   |
|   +------------------------------------│--------------------------------------+   |
|                                        ▼                                          |
|   +---------------------------------------------------------------------------+   |
|   |                             HTTP Handlers                                 |   |
|   |   AuthHandler, ClassHandler, StudentHandler, SubjectHandler,              |   |
|   |   QuestionHandler, ExamHandler, ReportHandler, AdminHandler               |   |
|   +------------------------------------│--------------------------------------+   |
|                                        ▼                                          |
|   +---------------------------------------------------------------------------+   |
|   |                          In-Memory Domain Objects                         |   |
|   |   dsLop (Class Array), dsSinhVien (LinkedList), NodeMH (Subject BST),     |   |
|   |   dsCHT (Question List), dsDiemThi (Score List)                           |   |
|   +------------------------------------│--------------------------------------+   |
|                                        ▼                                          |
|   +---------------------------------------------------------------------------+   |
|   |                          Storage Acceleration                             |   |
|   |   IndexManager (HashTable<K, V> Byte Offset Lookups: .idx files)           |   |
|   |   StorageManager (Atomic File I/O, Validation, Verification)              |   |
|   +------------------------------------│--------------------------------------+   |
+----------------------------------------│------------------------------------------+
                                         │ Physical File I/O
                                         ▼
+-----------------------------------------------------------------------------------+
|                               PHYSICAL STORAGE (DISK)                             |
|   storage/data/*.txt   (Authoritative Source of Truth)                            |
|   storage/indexes/*.idx (Non-authoritative Fast Byte-Offset Accelerator Files)    |
+-----------------------------------------------------------------------------------+
```

---

## 2. Frontend → Backend End-to-End Execution Flow

```
[React View Component (e.g., StudentListPage.tsx)]
                        │
                        ▼ (Calls function)
[Service Layer (e.g., studentService.ts)]
                        │
                        ▼ (Constructs HTTP Request & Axios call)
[HTTP REST API Request (e.g., POST http://localhost:8080/api/students)]
                        │
                        ▼ (Network Transport)
[RouteRegistry.cpp (httplib::Server)]
                        │ Matches URL & Method
                        ▼
[Handler Function (e.g., handle_create_student() in StudentHandler.cpp)]
                        │ 1. Parses JSON Body
                        │ 2. Acquires DB_WRITE_LOCK
                        │ 3. Normalizes via StringNormalizer
                        │ 4. Validates via StorageValidator
                        ▼
[Domain Object Mutation (e.g., Class::insert() & dsSinhVien linked list node creation)]
                        │
                        ▼
[StorageManager::saveStudents()]
                        │ 1. Serializes DOM tree to std::ostringstream
                        │ 2. Writes atomically to storage/data/students.txt via .tmp file rename
                        │ 3. Performs deep verification via StorageVerifier
                        │ 4. Calls IndexManager::rebuildStudentIndex() -> saves storage/indexes/student.idx
                        ▼
[JSON HTTP Response (e.g., HTTP 201 Created {"masv": "N22DCCN001", ...})]
                        │
                        ▼
[React Query / State Update in Component]
                        │
                        ▼
[Browser DOM Re-render]
```

---

## 3. Server Startup Lifecycle Flow

```
[Server Entry Point: main() in server.cpp]
                        │
                        ▼
[ServerBootstrap::run(argc, argv)]
                        │
                        ├──────► 1. PathResolver::init(argv[0])
                        │           - Resolves absolute path to executable directory
                        │           - Auto-migrates legacy root storage/*.txt to storage/data/
                        │
                        ├──────► 2. CLI Flag Check (--reset-storage / --test-validation)
                        │           - If --reset-storage: Resets 10 files to empty headers
                        │
                        ├──────► 3. LoadAllData(dsl, dsmh)
                        │           - StorageManager::loadClasses(dsl) -> storage/data/classes.txt
                        │           - StorageManager::loadStudents(dsl) -> storage/data/students.txt
                        │           - StorageManager::loadSubjects(dsmh) -> storage/data/subjects.txt
                        │           - StorageManager::loadQuestions(dsmh) -> storage/data/questions.txt
                        │           - StorageManager::loadScores(dsl) -> storage/data/scores.txt
                        │           - StorageManager::loadExamSessions() -> storage/data/exam_sessions.txt
                        │
                        ├──────► 4. IndexManager Initialization
                        │           - IndexManager::getInstance().loadQuestionIndex() -> storage/indexes/question.idx
                        │           - IndexManager::getInstance().loadStudentIndex() -> storage/indexes/student.idx
                        │           - IndexManager::getInstance().loadHistoryIndex() -> storage/indexes/history.idx
                        │           - If .idx missing: Automatically rebuilds from .txt source files
                        │
                        ├──────► 5. StorageManager::loadSystemSettings()
                        │           - Reads storage/data/SystemSettings.txt
                        │
                        ├──────► 6. adjustSessionsForDowntime()
                        │           - Marks timed-out exam sessions as submitted
                        │
                        ├──────► 7. registerRoutes(httplib::Server svr)
                        │           - Registers 28 REST endpoints + CORS headers
                        │
                        └──────► 8. svr.listen("0.0.0.0", 8080)
                                    - Starts multithreaded HTTP server event loop
```

---

## 4. Server Shutdown & Flush Flow

```
[SIGINT / Server Stop Signal]
                        │
                        ▼
[httplib::Server::stop()]
                        │
                        ▼
[StorageManager::saveAllData(dsl, dsmh)]
                        │
                        ├──────► 1. Flush metadata.txt (SCHEMA_VERSION, LAST_QUESTION_ID)
                        ├──────► 2. StorageManager::saveClasses(dsl) -> storage/data/classes.txt
                        ├──────► 3. StorageManager::saveStudents(dsl) -> storage/data/students.txt
                        ├──────► 4. StorageManager::saveSubjects(dsmh) -> storage/data/subjects.txt
                        ├──────► 5. StorageManager::saveQuestions(dsmh) -> storage/data/questions.txt
                        ├──────► 6. StorageManager::saveScores(dsl) -> storage/data/scores.txt
                        └──────► 7. IndexManager::rebuildAllIndexes() -> Flush .idx files
                        │
                        ▼
[Memory Cleanup & Process Exit 0]
```

---

## 5. Authentication Flow

```
[User Submits Credentials on /login]
                        │
                        ▼
[POST /api/login {"username": "...", "password": "...", "role": "GV"|"SV"}]
                        │
                        ▼
[handle_login() in AuthHandler.cpp]
                        │
                        ├──► If role == "GV" (Teacher / Admin):
                        │      Validate against Admin credentials ("GV" / "GV")
                        │
                        └──► If role == "SV" (Student):
                               Search Student globally via findStudentGlobal(username)
                               Verify Student.password == body.password
                        │
                        ▼
[Generate Session Token]
                        │ Creates active session entry in g_activeSessions map
                        ▼
[HTTP Response 200 OK {"token": "...", "user": {...}}]
                        │
                        ▼
[Frontend AuthContext / localStorage]
                        │ Stores token & User details, redirects to Dashboard
```

---

## 6. CRUD Flows for All Entities

### 6.1 Student CRUD Flow
- **Create:** `POST /api/students` $\rightarrow$ `handle_create_student()` $\rightarrow$ Validate $\rightarrow$ Append to `dsSinhVien` linked list $\rightarrow$ `saveStudents()` $\rightarrow$ Update `student.idx`.
- **Read:** `GET /api/students?malop=X` $\rightarrow$ `handle_get_students()` $\rightarrow$ Traverses `dsLop[X]->dssinhvien` list $\rightarrow$ Returns JSON array.
- **Update:** `PUT /api/students/:id` $\rightarrow$ `handle_update_student()` $\rightarrow$ Find in `dsSinhVien` $\rightarrow$ Mutate HO/TEN/PHAI/password $\rightarrow$ `saveStudents()`.
- **Delete:** `DELETE /api/students/:id` $\rightarrow$ `handle_delete_student()` $\rightarrow$ Check if student has scores $\rightarrow$ Remove node from `dsSinhVien` $\rightarrow$ `saveStudents()`.

### 6.2 Class CRUD Flow
- **Create:** `POST /api/classes` $\rightarrow$ `handle_create_class()` $\rightarrow$ Validate $\rightarrow$ Append to `dsLop.dslop[]` array $\rightarrow$ `saveClasses()`.
- **Read:** `GET /api/classes` $\rightarrow$ `handle_get_classes()` $\rightarrow$ Iterates `dsLop` array $\rightarrow$ Returns JSON array.
- **Update:** `PUT /api/classes/:id` $\rightarrow$ `handle_update_class()` $\rightarrow$ Update `TENLOP` $\rightarrow$ `saveClasses()`.
- **Delete:** `DELETE /api/classes/:id` $\rightarrow$ `handle_delete_class()` $\rightarrow$ Verify class is empty $\rightarrow$ Remove from `dsLop` $\rightarrow$ `saveClasses()`.

### 6.3 Subject CRUD Flow
- **Create:** `POST /api/subjects` $\rightarrow$ `handle_create_subject()` $\rightarrow$ Validate $\rightarrow$ Insert into `Subject` BST (`NodeMH`) $\rightarrow$ `saveSubjects()`.
- **Read:** `GET /api/subjects` $\rightarrow$ `handle_get_subjects()` $\rightarrow$ In-order BST traversal $\rightarrow$ Returns JSON array.
- **Update:** `PUT /api/subjects/:id` $\rightarrow$ `handle_update_subject()` $\rightarrow$ Find in BST $\rightarrow$ Update `TENMH` $\rightarrow$ `saveSubjects()`.
- **Delete:** `DELETE /api/subjects/:id` $\rightarrow$ `handle_delete_subject()` $\rightarrow$ Verify subject has no questions & no scores $\rightarrow$ Remove from BST $\rightarrow$ `saveSubjects()`.

### 6.4 Question CRUD Flow
- **Create:** `POST /api/questions` $\rightarrow$ `handle_create_question()` $\rightarrow$ Assign auto-increment `ID` $\rightarrow$ Append to `dsCHT` list of subject node $\rightarrow$ `saveQuestions()` $\rightarrow$ Update `question.idx`.
- **Read:** `GET /api/reports/exam` $\rightarrow$ Query question linked list by `ID`.
- **Update:** `PUT /api/questions/:id` $\rightarrow$ `handle_update_question()` $\rightarrow$ Find in `dsCHT` $\rightarrow$ Update text/options/answer $\rightarrow$ `saveQuestions()`.
- **Delete (Soft Delete):** `DELETE /api/questions/:id` $\rightarrow$ If question used in exams: Set `deleted = true` (Soft Delete). If unused: Unlink from `dsCHT` list (Hard Delete) $\rightarrow$ `saveQuestions()`.

### 6.5 Score & Exam Flow
- **Start Exam:** `POST /api/exams/start` $\rightarrow$ Check student score existence $\rightarrow$ Sample $Q$ questions from subject `dsCHT` $\rightarrow$ Save `ExamSession` to `exam_sessions.txt`.
- **Answer Question:** `PUT /api/exams/answer` $\rightarrow$ Update answer in active `ExamSession` $\rightarrow$ Save `exam_sessions.txt`.
- **Submit Exam:** `POST /api/exams/submit` $\rightarrow$ Grade answers against `DAPAN_DUNG` $\rightarrow$ Insert `DiemThi` into student's `dsdiemthi` list $\rightarrow$ `saveScores()` $\rightarrow$ `appendExamHistory()` $\rightarrow$ Update `history.idx` $\rightarrow$ `rebuildUsedFlags()`.

---

## 7. Exam End-to-End Workflow

```
[Teacher Creates Question Bank]
               │ (POST /api/questions)
               ▼
[Subject BST Node -> dsCHT Question Linked List]
               │
               ▼
[Student Starts Exam: POST /api/exams/start]
               │ 1. Checks if student has already taken subject exam
               │ 2. Selects Q random questions from dsCHT linked list
               │ 3. Creates ExamSession in memory & flushes to exam_sessions.txt
               ▼
[Student Submits Answers: PUT /api/exams/answer]
               │ Updates active session answer array
               ▼
[Student Submits Exam: POST /api/exams/submit]
               │ 1. Compares student answers against question correct answers
               │ 2. Calculates Score = (soDung / total) * 10.0
               │ 3. Inserts DiemThi into Student.dsdiemthi list -> saves storage/data/scores.txt
               │ 4. Appends record to storage/data/exam_history.txt
               │ 5. Updates storage/indexes/history.idx offset index
               │ 6. Calls StorageManager::rebuildUsedFlags() -> marks examined questions as used=true
               │ 7. Removes active ExamSession from exam_sessions.txt
```

---

## 8. Report Subsystem Flow

```
[User Requests Report (Scoreboard / Exam History)]
                        │
                        ▼
[GET /api/reports/scoreboard?malop=X  OR  GET /api/reports/exam?masv=Y&mamh=Z]
                        │
                        ▼
[ReportHandler.cpp]
                        │
                        ├──► Scoreboard Report:
                        │      1. Finds Lop by malop in dsLop
                        │      2. Traverses dssinhvien linked list
                        │      3. For each student, reads dsdiemthi list
                        │      4. Builds cross-tabulated JSON score matrix
                        │
                        └──► Student Exam History Report:
                               1. Queries IndexManager::getHistoryOffsets(masv)
                               2. Seeks byte offsets directly in storage/data/exam_history.txt
                               3. Deserializes exact exam session lines in O(1) time
                               4. Constructs detailed answer choice breakdown JSON
```

---

## 9. Storage & Fast Index Seek Pipeline

```
                       [Entity Lookup Request (e.g., MASV="N22DCCN001")]
                                                │
                                                ▼
                       [IndexManager::getStudentOffset("N22DCCN001")]
                                                │
                                                ▼
                       [Custom HashTable<std::string, int64_t>]
                                                │
                                                ▼ Computes FNV-1a Hash % 10007
                                                │ Returns Byte Offset (e.g., 4096)
                                                ▼
                       [std::ifstream.seekg(4096) in storage/data/students.txt]
                                                │
                                                ▼ (Direct Line Read)
                       [Raw Text Line: "D22CQCN01-N|N22DCCN001|Nguyen Van|An|Nam|123"]
                                                │
                                                ▼
                       [StorageDeserializer::split(line, '|')]
                                                │
                                                ▼
                       [Instantiates SinhVien Object in RAM]
```

---

## 10. Module Execution Dependency Graph

```
[RouteRegistry]
       │
       ▼
[ServerHandlers] (AuthHandler, StudentHandler, QuestionHandler, ExamHandler, etc.)
       │
       ├───────────────────────────┐
       ▼                           ▼
[Domain Models]             [StorageManager]
(Class, Student,             │
 Subject, Question,          ├───────────────────────────┐
 Score, Exam)                ▼                           ▼
       │                    [IndexManager]         [StorageValidator]
       │                     │                           │
       ▼                     ▼                           ▼
[Custom Containers]         [Custom HashTable]     [StringNormalizer]
(DArray, LinkedList,         │
 BST, Queue, Stack)          ▼
                    [PathResolver] ──► Physical Storage File System
```

---

## 11. Core Module Responsibilities

| Module | Role | Called By | Calls | Input | Output |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **`ServerBootstrap`** | Application Lifecycle Orchestrator | `main.cpp` | `PathResolver`, `StorageManager`, `IndexManager`, `httplib::Server` | CLI args | Exit code |
| **`RouteRegistry`** | HTTP Endpoint Router | `ServerBootstrap` | Server Handlers | HTTP Request | HTTP Response |
| **`StorageManager`** | Persistence & File I/O Coordinator | Handlers, Bootstrap | `PathResolver`, `StorageValidator`, `StorageVerifier`, `IndexManager` | Domain objects | `bool` success |
| **`IndexManager`** | Fast Offset Seeking Accelerator | `StorageManager`, Handlers | `PathResolver`, `HashTable` | Primary Keys | Byte Offsets |
| **`HashTable<K,V>`**| Custom $O(1)$ Hash Container | `IndexManager` | None | Keys (`string`/`int`) | Values (`int64_t`/`DArray`) |
| **`PathResolver`** | Storage Directory Path Router | All Storage Modules | `std::filesystem` | File basename | Absolute Path |
| **`StorageValidator`**| Data Integrity & Delimiter Gatekeeper | `StorageManager`, Handlers | `StringNormalizer` | Domain objects | `bool` + error reason |
| **`StorageVerifier`** | Deep Disk Flush Verification | `StorageManager` | `StorageDeserializer` | Domain objects | `bool` + error reason |

---

## 12. Complete API Map

| Method | Endpoint | Frontend Caller | Backend File | Handler | Storage Layer |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `POST` | `/api/login` | `authService.ts` | `AuthHandler.cpp` | `handle_login` | Memory Session |
| `GET` | `/api/classes` | `classService.ts` | `ClassHandler.cpp` | `handle_get_classes` | `dsLop` |
| `POST` | `/api/classes` | `classService.ts` | `ClassHandler.cpp` | `handle_create_class` | `classes.txt` |
| `PUT` | `/api/classes/:id` | `classService.ts` | `ClassHandler.cpp` | `handle_update_class` | `classes.txt` |
| `DELETE`| `/api/classes/:id` | `classService.ts` | `ClassHandler.cpp` | `handle_delete_class` | `classes.txt` |
| `GET` | `/api/students` | `studentService.ts` | `StudentHandler.cpp` | `handle_get_students` | `dsSinhVien` |
| `POST` | `/api/students` | `studentService.ts` | `StudentHandler.cpp` | `handle_create_student` | `students.txt` / `student.idx` |
| `PUT` | `/api/students/:id` | `studentService.ts` | `StudentHandler.cpp` | `handle_update_student` | `students.txt` |
| `DELETE`| `/api/students/:id` | `studentService.ts` | `StudentHandler.cpp` | `handle_delete_student` | `students.txt` |
| `GET` | `/api/subjects` | `subjectService.ts` | `SubjectHandler.cpp` | `handle_get_subjects` | `NodeMH` BST |
| `POST` | `/api/subjects` | `subjectService.ts` | `SubjectHandler.cpp` | `handle_create_subject` | `subjects.txt` |
| `PUT` | `/api/subjects/:id` | `subjectService.ts` | `SubjectHandler.cpp` | `handle_update_subject` | `subjects.txt` |
| `DELETE`| `/api/subjects/:id` | `subjectService.ts` | `SubjectHandler.cpp` | `handle_delete_subject` | `subjects.txt` |
| `POST` | `/api/questions` | `questionService.ts`| `QuestionHandler.cpp`| `handle_create_question`| `questions.txt` / `question.idx` |
| `PUT` | `/api/questions/:id`| `questionService.ts`| `QuestionHandler.cpp`| `handle_update_question`| `questions.txt` |
| `DELETE`| `/api/questions/:id`| `questionService.ts`| `QuestionHandler.cpp`| `handle_delete_question`| `questions.txt` (Soft Delete) |
| `POST` | `/api/exams/start` | `examService.ts` | `ExamHandler.cpp` | `handle_exam_start` | `exam_sessions.txt` |
| `PUT` | `/api/exams/answer` | `examService.ts` | `ExamHandler.cpp` | `handle_exam_answer` | `exam_sessions.txt` |
| `POST` | `/api/exams/submit` | `examService.ts` | `ExamHandler.cpp` | `handle_exam_submit` | `scores.txt` / `exam_history.txt` |
| `GET` | `/api/reports/scoreboard` | `reportService.ts`| `ReportHandler.cpp` | `handle_report_scoreboard`| `scores.txt` |
| `GET` | `/api/reports/exam` | `reportService.ts`| `ReportHandler.cpp` | `handle_report_exam` | `history.idx` / `exam_history.txt` |

---

## 13. Frontend to Storage File Relationship

```
[StudentListPage.tsx] ──► [studentService.ts] ──► [StudentHandler.cpp]  ──► [students.txt & student.idx]
[ClassListPage.tsx]   ──► [classService.ts]   ──► [ClassHandler.cpp]    ──► [classes.txt]
[SubjectListPage.tsx] ──► [subjectService.ts] ──► [SubjectHandler.cpp]  ──► [subjects.txt]
[QuestionPage.tsx]    ──► [questionService.ts]──► [QuestionHandler.cpp] ──► [questions.txt & question.idx]
[ExamTakingPage.tsx]  ──► [examService.ts]    ──► [ExamHandler.cpp]     ──► [scores.txt & history.idx]
```

---

## 14. Detailed Feature Data Flows

### 14.1 Delete Question Data Flow
```
[User Clicks Delete Question #105]
             │
             ▼
[DELETE /api/questions/105]
             │
             ▼
[QuestionHandler.cpp: handle_delete_question()]
             │
             ▼ Checks Question.used flag
             ├──────► If used == true:
             │          Set Question.deleted = true (Soft Delete - Preserves Exam History integrity)
             │
             └──────► If used == false:
                        Unlink node 105 from dsCHT linked list (Hard Delete)
             │
             ▼
[StorageManager::saveQuestions()]
             │ 1. Rewrites storage/data/questions.txt atomically
             │ 2. Rebuilds & saves storage/indexes/question.idx
             ▼
[HTTP 200 OK {"success": true}]
```

---

## 15. Storage Layout & Index Mapping

```
+-------------------------------------------------------------------------------+
|                             STORAGE ARCHITECTURE                              |
|                                                                               |
|   storage/data/questions.txt (Data File)                                      |
|   +-----------------------------------------------------------------------+   |
|   | Byte 0:    MAMH|ID|NOIDUNG|A|B|C|D|DAPAN_DUNG|USED|DELETED              |   |
|   | Byte 48:   INT1339|1|Polymorphism la gi?|Da hinh|...|a|1|0            |   |
|   | Byte 1048: INT1339|105|Tu khoa 'virtual' dung de?|...|b|0|0          |   |
|   +-----------------------------------------------------------------------+   |
|                                     ▲                                         |
|                                     │ Fast Offset Seek (Seekg: 1048)          |
|   storage/indexes/question.idx      │                                         |
|   +---------------------------------┴-------------------------------------+   |
|   | 105|1048                                                              |   |
|   +-----------------------------------------------------------------------+   |
|                                     ▲                                         |
|                                     │ Load on Boot                            |
|   HashTable<int, int64_t> m_questionIndex (RAM)                               |
|   +-----------------------------------------------------------------------+   |
|   | Key: 105  ===>  Value: 1048                                              |   |
|   +-----------------------------------------------------------------------+   |
+-------------------------------------------------------------------------------+
```

---

## 16. Architecture Summary & Assessment

### Strengths
1. **Indexed Text Storage Architecture:** Combines plain-text file safety and human readability (`.txt`) with $O(1)$ fast byte offset seeking (`.idx`).
2. **Custom Container Policy:** Implemented custom generic `HashTable<K,V>` and `DArray<T>` containers without relying on STL headers.
3. **Atomic File Flush:** Temporary `.tmp` file writing + atomic rename prevents storage file corruption during unexpected power cuts.
4. **Self-Healing Index Recovery:** Automatically rebuilds `.idx` files from `.txt` data files if index files are missing on boot.

### Trade-Offs & Limitations
1. **Fixed Hash Capacity:** `HashTable` capacity is fixed ($10,007$ buckets), sufficient for $100,000+$ records but lacks dynamic rehashing.
2. **Full Array Serialization:** Mutating a student serializes all students back to disk rather than performing in-place block updates.

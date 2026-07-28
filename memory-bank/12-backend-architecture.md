# 12 — Backend Architecture

## Layer Architecture (Verified)

```
┌────────────────────────────────────────────────────────────┐
│  PRESENTATION LAYER (Console UI)                           │
│  ┌──────────────┐  ┌───────────────┐  ┌────────────────┐  │
│  │ Menu.cpp     │  │ Validation.   │  │ Report.cpp     │  │
│  │ (STUB)       │  │ cpp           │  │ (Partial)      │  │
│  └──────────────┘  └───────────────┘  └────────────────┘  │
├────────────────────────────────────────────────────────────┤
│  BUSINESS LOGIC LAYER (Modules)                           │
│  ┌──────┬────────┬────────┬──────────┬────────┬────────┐  │
│  │ Auth │ Class  │Student │ Subject  │Question│ Score  │  │
│  │ ✅   │ ✅     │  ✅    │  ✅      │  ✅    │  ✅    │  │
│  └──────┴────────┴────────┴──────────┴────────┴────────┘  │
│  ┌────────┐  ┌────────┐                                    │
│  │ Exam   │  │ Utils  │                                    │
│  │ ❌     │  │ ❌     │                                    │
│  └────────┘  └────────┘                                    │
├────────────────────────────────────────────────────────────┤
│  DATA STRUCTURE LAYER                                      │
│  ┌──────────┬───────────┬──────────┬──────────┬──────────┐ │
│  │ BST      │ Array of  │ Linked   │ Linked   │ Linked   │ │
│  │ (MonHoc) │ Pointers  │ List     │ List     │ List     │ │
│  │          │ (Lop*)    │(SinhVien)│ (CauHoi) │(DiemThi) │ │
│  └──────────┴───────────┴──────────┴──────────┴──────────┘ │
├────────────────────────────────────────────────────────────┤
│  STORAGE LAYER                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ TXT Files (pipe-delimited) + Storage.cpp (STUB)       │  │
│  └──────────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────────┘

⚠ HTTP SERVER (standalone, not connected to business layer)
  ┌──────────────────────────────────────────────────────┐
  │ server.cpp (httplib.h) — single GET / route only      │
  └──────────────────────────────────────────────────────┘
```

**Confidence**: Verified

## Data Structures

| Structure | Type | Details | Module |
|-----------|------|---------|--------|
| MonHoc BST | Binary Search Tree | Auto-balanced via perfect tree rebuild | Subject |
| CauHoi List | Singly Linked List | Sorted by ID | Question |
| Lop Array | Array of Pointers | Fixed size MAXLOP=10000 | Class |
| SinhVien List | Singly Linked List | Sorted by MASV | Student |
| DiemThi List | Singly Linked List | Sorted by MAMH | Score |
| DArray | Dynamic Array (Template) | Used for tree rebuild | Subject (utility) |

**Confidence**: Verified

## Memory Model
- All data structures are **in-memory** (loaded from files at startup)
- Raw pointers with manual `new`/`delete` (no smart pointers)
- Each module owns its data structure via `root` member pointer
- Deep copy in copy constructors and assignment operators
- Linked lists maintain sorted order on insert (O(n) insertion)

**Confidence**: Verified

## Key Design Decisions

| Decision | Rationale | Evidence |
|----------|-----------|----------|
| BST auto-balancing | Maintain O(log n) search time | Subject::_rebuildTree() in Subject.cpp |
| Sorted linked lists | Enables O(n) search with early termination | Student::find(), Question::find() use comparison skipping |
| Legacy Init.h | Pre-dates class-based headers, still used by Auth and Report | Auth.cpp:3 includes "Init.h" |
| Standalone storage paths | Relative paths with `../../../storage/` prefix | All PATH_* constants in src/*.cpp |
| No smart pointers | C++17 project using raw pointers for pedagogy | All modules use raw new/delete |

**Confidence**: Verified

## Files by Layer

| Layer | Files |
|-------|-------|
| Presentation (Console) | `src/Menu.cpp`, `src/Validation.cpp`, `src/Report.cpp` |
| Business Logic | `src/Auth.cpp`, `src/Class.cpp`, `src/Student.cpp`, `src/Subject.cpp`, `src/Question.cpp`, `src/Score.cpp` |
| Data Structures | `include/*.h` (all headers define structs/classes) |
| Storage | `src/Storage.cpp` (orchestration stub) + inline in each module |
| HTTP Server | `server.cpp`, `include/httplib.h` |

**Confidence**: Verified
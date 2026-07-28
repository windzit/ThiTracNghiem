# 00 — Project DNA

## Architecture Summary

```
┌─────────────────────────────────────────────────────────────┐
│  FRONTEND (React + TypeScript + Vite)                       │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  Pages (23 routes) → Services (8) → Mock Data (7)     │  │
│  └───────────────────────────────────────────────────────┘  │
│         ⚠ NO HTTP CONNECTION TO BACKEND                     │
├─────────────────────────────────────────────────────────────┤
│  BACKEND (C++17 Console Application)                        │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  main.cpp → Menu.cpp → Auth → CRUD → Storage → TXT    │  │
│  │  server.cpp (httplib.h) — standalone HTTP stub         │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

**Confidence**: Verified

## Layer Diagram (Actual)

| Layer | Technology | Status | Evidence |
|-------|-----------|--------|----------|
| Presentation (Console) | C++ iostream, conio.h | ⚠ Partial | Menu.cpp is stub |
| Business Logic | C++ classes (Class, Student, Subject, Question, Score) | ✅ Complete | All CRUD implemented |
| Validation | C++ functions (Validation.cpp) | ✅ Complete | Input validation with getch() |
| Data Structures | BST, linked list, array of pointers | ✅ Complete | Per assignment spec |
| Storage | TXT files with `\|` delimiter | ✅ Complete | Per-module save/load |
| HTTP Server | httplib.h (server.cpp) | ⚠ Stub | Single route only |
| Frontend | React 18 + TypeScript + Vite | ✅ Complete | 23 routes, mock data |

## Core Modules

| Module | Header | Source | Status | Lines |
|--------|--------|--------|--------|-------|
| Auth | include/Auth.h | src/Auth.cpp | ✅ Complete | 20 |
| Class | include/Class.h | src/Class.cpp | ✅ Complete | 118 |
| Student | include/Student.h | src/Student.cpp | ✅ Complete | 156 |
| Subject | include/Subject.h | src/Subject.cpp | ✅ Complete | 225 |
| Question | include/Question.h | src/Question.cpp | ✅ Complete | 177 |
| Score | include/Score.h | src/Score.cpp | ✅ Complete | 172 |
| Validation | — (no header) | src/Validation.cpp | ✅ Complete | 206 |
| Report | — (no header) | src/Report.cpp | ⚠ Partial | 24 |
| Storage | include/Storage.h | src/Storage.cpp | ⚠ Stub | 13 |
| Menu | include/Menu.h | src/Menu.cpp | ⚠ Stub | 6 |
| Exam | include/Exam.h | — | ❌ Missing | 0 |
| Utils | — | — | ❌ Missing | 0 |

## Dependency Tree (Backend)

```
main.cpp
  └── Menu.h
        ├── Auth.h → Init.h (LEGACY)
        ├── Class.h → Student.h → Score.h → CommonTypes.h
        ├── Subject.h → Question.h → CommonTypes.h
        │               └── DArray.h
        ├── Storage.h → Class.h, Subject.h
        ├── Report.h → Init.h (LEGACY)
        └── Exam.h
```

## Execution Flow

```
1. main.cpp starts
2. Calls ShowMainMenu() [STUB]
3. (Planned) Menu → Auth → Role selection (GV/SV)
4. (Planned) GV: CRUD operations on Class/Student/Subject/Question
5. (Planned) SV: Take exam (g/), View results (h/)
6. (Planned) Save/Load data to/from TXT files
```

## Entry Points

| Entry | File | Line | Status |
|-------|------|------|--------|
| `main()` | main.cpp | 4 | ✅ Active |
| `ShowMainMenu()` | src/Menu.cpp | 4 | ⚠ Stub |
| `server.cpp main()` | server.cpp | 11 | ⚠ Stub |
| `CMakeLists.txt` | CMakeLists.txt | — | ⚠ Builds test.cpp target |

## Critical Files

| File | Why Critical |
|------|-------------|
| include/Class.h | Defines Lop struct + dsLop array container |
| include/Student.h | Defines SinhVien struct + linked list |
| include/Subject.h | Defines MonHoc struct + BST with auto-balance |
| include/Question.h | Defines CauHoi struct + linked list |
| include/Score.h | Defines DiemThi struct + linked list |
| src/Init.h | Legacy duplicate of all data structures (used by Auth, Report) |
| src/Validation.cpp | Console input validation (getch-based) |
| document/debai.md | **Authoritative** assignment specification |

## Important Classes/Functions

| Class/Function | File | Role |
|---------------|------|------|
| `Class` | include/Class.h | Array-of-pointers container for Lop |
| `Student` | include/Student.h | Sorted linked list for SinhVien |
| `Subject` | include/Subject.h | BST with auto-rebalancing for MonHoc |
| `Question` | include/Question.h | Sorted linked list for CauHoi |
| `Score` | include/Score.h | Sorted linked list for DiemThi |
| `login_teacher()` | src/Auth.cpp | Checks "GV"/"GV" |
| `login_student()` | src/Auth.cpp | Linear search across all classes |
| `Subject::_rebuildTree()` | src/Subject.cpp | BST auto-balancing via perfect tree rebuild |
| `NhapChuoi()` | src/Validation.cpp | Console string input with validation |
| `NhapMa()` | src/Validation.cpp | Console code input (alphanumeric + - _) |

## Shared Data Structures

| Structure | Defined In | Used By |
|-----------|-----------|---------|
| `Lop` | include/Class.h | Class, Student, Auth |
| `SinhVien` | include/Student.h | Student, Class, Auth |
| `MonHoc` | include/Subject.h | Subject |
| `CauHoi` | include/Question.h | Question, Subject |
| `DiemThi` | include/Score.h | Score, Student |
| `dsLop` | include/Class.h | Class, Auth, Report |
| `dsSinhVien` | include/Student.h | Student |
| `dsCHT` | include/Question.h | Question |
| `dsDiemThi` | include/Score.h | Score |
| `NodeMH` | include/Subject.h | Subject |
| `DArray` | include/DArray.h | Subject (rebuild tree) |

**Confidence**: Verified
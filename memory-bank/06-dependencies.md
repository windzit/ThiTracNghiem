# 06 — Dependencies

## Community Summary (from Graphify)

| Community | Nodes | Edges | Cohesion | Focus |
|-----------|-------|-------|----------|-------|
| httplib.h | 52 | 410 | 0.03 | HTTP library internals |
| ClientImpl | 65 | 640 | 0.03 | HTTP client |
| Server | 56 | 470 | 0.04 | HTTP server |
| Subject.cpp | 16 | 44 | 0.18 | Subject BST + auto-balance |
| Class.cpp | 6 | 18 | 0.24 | Class CRUD |
| Student.cpp | 7 | 18 | 0.33 | Student CRUD |
| Question.cpp | 5 | 14 | 0.20 | Question CRUD |
| Score.cpp | 8 | 28 | 0.20 | Score CRUD |
| Auth.cpp | 4 | 8 | 0.50 | Authentication |
| Validation.cpp | 7 | 26 | 0.64 | Input validation |
| Menu.h | 4 | 8 | 0.25 | Menu stub |
| Report.cpp | 4 | 12 | 0.21 | Reports |
| App.tsx | 12 | 38 | 0.07 | Frontend routing |

**Confidence**: Verified

## Dependency Graph (Backend — Verified from Source)

```
main.cpp
  └── include/Menu.h
        └── src/Menu.cpp [STUB]
              └── (planned: Auth, Class, Subject, Student, Question, Score, Report, Storage)

src/Auth.cpp
  ├── include/Auth.h
  └── src/Init.h [LEGACY — uses raw-pointer structs]

src/Class.cpp
  ├── include/Class.h
  │     └── include/Student.h
  │           └── include/Score.h
  │                 └── include/CommonTypes.h
  └── (file I/O: storage/Classes.txt)

src/Student.cpp
  ├── include/Student.h
  │     └── include/Score.h
  │           └── include/CommonTypes.h
  └── (file I/O: storage/Students/{MALOP}.txt)

src/Subject.cpp
  ├── include/Subject.h
  │     ├── include/Question.h
  │     │     └── include/CommonTypes.h
  │     └── include/DArray.h
  └── (file I/O: storage/Subjects.txt)

src/Question.cpp
  ├── include/Question.h
  │     └── include/CommonTypes.h
  └── (file I/O: storage/Questions/{MAMH}.txt)

src/Score.cpp
  ├── include/Score.h
  │     └── include/CommonTypes.h
  └── (file I/O: storage/ScoreList/{MASV}.txt)

src/Report.cpp
  ├── include/Report.h
  ├── src/Init.h [LEGACY]
  └── include/Class.h

src/Storage.cpp
  └── include/Storage.h
        ├── include/Class.h
        └── include/Subject.h

src/Validation.cpp
  └── (standalone — no custom headers)
```

**Confidence**: Verified

## Call Graph (Key Functions)

| Caller | Callee | File |
|--------|--------|------|
| `main()` | `ShowMainMenu()` | main.cpp → src/Menu.cpp |
| `login_teacher()` | (inline check) | src/Auth.cpp |
| `login_student()` | (linear search dsLop → dsSinhVien) | src/Auth.cpp |
| `Class::find()` | (linear array search) | src/Class.cpp |
| `Class::insert()` | `Class::find()` | src/Class.cpp |
| `Class::remove()` | (array shift) | src/Class.cpp |
| `Class::save()` | `Student::save()` | src/Class.cpp |
| `Class::load()` | `Student::load()` | src/Class.cpp |
| `Student::save()` | `Score::save()` | src/Student.cpp |
| `Student::load()` | `Score::load()` | src/Student.cpp |
| `Subject::insert()` | `Subject::_insert()`, `Subject::_rebuildTree()` | src/Subject.cpp |
| `Subject::_rebuildTree()` | `Subject::_storeNodes()`, `Subject::_buildPerfectTree()` | src/Subject.cpp |
| `Subject::save()` | `Subject::_save()`, `Question::save()` | src/Subject.cpp |
| `Subject::load()` | `Question::load()` | src/Subject.cpp |

**Confidence**: Verified

## Critical Nodes (Bridge Nodes)

| Node | Communities Bridged | Why Critical |
|------|-------------------|-------------|
| `Lop` | Class.h, test.cpp, string | Cross-community connector |
| `Subject` | Subject, test.cpp, MonHoc | Cross-community connector |
| `Server` | httplib.h, Server, ClientImpl, ... | Central HTTP hub |
| `dsLop` | Class.h, Auth.cpp, Report.cpp | Shared container used by legacy and new code |

**Confidence**: Verified

## Shared Components

| Component | Used By | Type |
|-----------|---------|------|
| `CommonTypes.h` | Question, Score | Header (includes fstream, string, etc.) |
| `DArray.h` | Subject | Template dynamic array |
| `Init.h` | Auth, Report | Legacy struct definitions (duplicate of include/*.h) |

**Confidence**: Verified
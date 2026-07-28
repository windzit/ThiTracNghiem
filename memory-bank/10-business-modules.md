# 10 — Business Modules

## Discovered Modules

| Module | Responsibility | Status | Confidence |
|--------|---------------|--------|------------|
| **Auth** | Authentication, role detection (SV/GV) | ✅ Complete | Verified |
| **Class** | Lop CRUD (array of pointers, max 10000) | ✅ Complete | Verified |
| **Student** | SinhVien CRUD (sorted linked list) | ✅ Complete | Verified |
| **Subject** | MonHoc CRUD (BST with auto-rebalancing) | ✅ Complete | Verified |
| **Question** | CauHoi CRUD (sorted linked list, used-flag) | ✅ Complete | Verified |
| **Score** | DiemThi CRUD (sorted linked list) | ✅ Complete | Verified |
| **Validation** | Console input validation (getch-based) | ✅ Complete | Verified |
| **Report** | Print formatted reports | ⚠ Partial | Verified |
| **Storage** | Load/Save all data orchestration | ⚠ Stub | Verified |
| **Menu** | Console menu navigation | ⚠ Stub | Verified |
| **Exam** | Exam orchestration (g/) | ❌ Missing | Verified |
| **Utils** | Random ID, rounding, string utils | ❌ Missing | Verified |

## Module Details

### Auth
- **Header**: `include/Auth.h`
- **Source**: `src/Auth.cpp`
- **Public Functions**: `login_teacher()`, `login_student()`
- **Dependencies**: `Init.h` (legacy), `string`
- **File I/O**: None (uses loaded data)
- **Console I/O**: None (called by Menu)
- **Business Rules**: BR-01 (GV/GV → admin; MASV/password → student)
- **Status**: ✅ Complete (20 lines)

### Class
- **Header**: `include/Class.h`
- **Source**: `src/Class.cpp`
- **Data Structure**: Array of pointers (`Lop* dslop[MAXLOP]`) with count `n`
- **Public Functions**: `insert(Lop*)`, `remove(MALOP)`, `find(MALOP)`, `save()`, `load()`
- **Dependencies**: `Student.h`, `Score.h`, `CommonTypes.h`
- **File I/O**: `storage/Classes.txt` (pipe-delimited), delegates to Student::save/load
- **Console I/O**: None (called by Menu)
- **Status**: ✅ Complete (118 lines)

### Student
- **Header**: `include/Student.h`
- **Source**: `src/Student.cpp`
- **Data Structure**: Sorted linked list (`dsSinhVien` sorted by MASV)
- **Public Functions**: `insert(SinhVien)`, `remove(MASV)`, `find(MASV)`, `save(MALOP)`, `load(MALOP)`
- **Dependencies**: `Score.h`
- **File I/O**: `storage/Students/{MALOP}.txt`, delegates to Score::save/load
- **Console I/O**: None
- **Status**: ✅ Complete (156 lines)

### Subject
- **Header**: `include/Subject.h`
- **Source**: `src/Subject.cpp`
- **Data Structure**: BST (`NodeMH`) with auto-rebalancing (perfect tree rebuild)
- **Public Functions**: `insert(MonHoc)`, `remove(MAMH)`, `find(MAMH)`, `save()`, `load()`
- **Private Functions**: `_insert()`, `_remove()`, `_save()`, `_rebuildTree()`, `_storeNodes()`, `_buildPerfectTree()`
- **Dependencies**: `Question.h`, `DArray.h`, `cmath`
- **File I/O**: `storage/Subjects.txt`, delegates to Question::save/load
- **Console I/O**: None
- **Status**: ✅ Complete (225 lines)

### Question
- **Header**: `include/Question.h`
- **Source**: `src/Question.cpp`
- **Data Structure**: Sorted linked list (`dsCHT` sorted by ID)
- **Public Functions**: `insert(CauHoi)`, `remove(ID)`, `find(ID)`, `save(MAMH)`, `load(MAMH)`
- **Dependencies**: `CommonTypes.h`
- **File I/O**: `storage/Questions/{MAMH}.txt`
- **Business Rule**: BR-03 (cannot remove if used flag is true)
- **Status**: ✅ Complete (177 lines)
- **Note**: ID generation is NOT implemented — `insert()` takes an externally-provided ID

### Score
- **Header**: `include/Score.h`
- **Source**: `src/Score.cpp`
- **Data Structure**: Sorted linked list (`dsDiemThi` sorted by MAMH)
- **Public Functions**: `insert(DiemThi)`, `remove(MAMH)`, `find(MAMH)`, `save(MASV)`, `load(MASV)`
- **Dependencies**: `CommonTypes.h`
- **File I/O**: `storage/ScoreList/{MASV}.txt`
- **Status**: ✅ Complete (172 lines)

### Validation
- **Header**: None (standalone)
- **Source**: `src/Validation.cpp`
- **Public Functions**: `CheckBackSpace()`, `CheckChuCai()`, `CheckChuSo()`, `CheckPhimChucNang()`, `NhapChuoi()`, `NhapChuSo()`, `NhapMa()`
- **Dependencies**: `conio.h`, `windows.h`, `iostream`
- **Console I/O**: Interactive (getch-based input with echo)
- **Status**: ✅ Complete (206 lines)
- **Note**: Uses Windows-specific console I/O (conio.h, getch)

### Report
- **Header**: None (declared in src/Report.cpp? include/Report.h exists)
- **Source**: `src/Report.cpp`
- **Public Functions**: `PrintClassListReport()`, `PrintExamDetailReport()`, `PrintScoreboardReport()`
- **Dependencies**: `Init.h` (legacy), `Class.h`
- **Console I/O**: Prints formatted tables
- **Status**: ⚠ Partial — only PrintClassListReport() implemented (24 lines)

### Storage
- **Header**: `include/Storage.h`
- **Source**: `src/Storage.cpp`
- **Public Functions**: `LoadAllData()`, `SaveAllData()`
- **Dependencies**: `Class.h`, `Subject.h`
- **Status**: ⚠ Stub — both functions are empty stubs (13 lines)

### Menu
- **Header**: `include/Menu.h`
- **Source**: `src/Menu.cpp`
- **Public Functions**: `ShowMainMenu()`
- **Status**: ⚠ Stub — prints "not implemented yet" (6 lines)

### Exam
- **Header**: `include/Exam.h` (stub — only `InitExamModule()` declared)
- **Source**: `src/Exam.cpp` — **NOT FOUND**
- **Status**: ❌ Missing entirely

### Utils
- **Header**: `include/Utils.h` — **NOT FOUND** (referenced in docs but doesn't exist)
- **Source**: `src/Utils.cpp` — **NOT FOUND**
- **Status**: ❌ Missing entirely
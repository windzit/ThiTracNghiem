# 11 — Coding Convention

## C++ Conventions (from Source Code)

### Naming
| Convention | Example | File |
|-----------|---------|------|
| PascalCase classes | `Class`, `Student`, `Subject`, `Question`, `Score` | All headers |
| PascalCase structs | `Lop`, `SinhVien`, `MonHoc`, `CauHoi`, `DiemThi` | include/*.h |
| UPPER_SNAKE constants | `MAXLOP` | CommonTypes.h |
| snake_case files | `class.cpp`, `student.cpp` | src/ |
| Hungarian-ish prefixes | `ds` for data structures (dsLop, dsSinhVien, dsCHT, dsDiemThi) | All headers |
| Prefix `_` for private methods | `_insert`, `_remove`, `_rebuildTree`, `_storeNodes`, `_buildPerfectTree` | Subject.h, Subject.cpp |
| `MALOP`, `MAMH`, `MASV` | Vietnamese abbreviations for "mã lớp", "mã môn học", "mã sinh viên" | All data structs |

**Confidence**: Verified

### Coding Patterns
| Pattern | Usage | Example |
|---------|-------|---------|
| `#pragma once` | Header guards | All headers |
| `extern std::string` | File path constants | PATH_CLASSES_TEXT, PATH_FOLDER_STUDENTS, etc. |
| `std::stringstream` + `getline` + `|` | Pipe-delimited file parsing | All save/load methods |
| `this == &other` check | Copy assignment operator | Class, Student, Subject, Question, Score |
| Raw pointers + manual `new`/`delete` | Dynamic memory management | All data structures |
| `std::strcpy` | C-string copy (for char arrays like MAMH[15]) | Subject.cpp, Score.cpp |

**Confidence**: Verified

### Vietnamese Naming (Business Domain)
| Vietnamese | English | Used In |
|-----------|---------|---------|
| Lop | Class | Class.h |
| SinhVien | Student | Student.h |
| MonHoc | Subject | Subject.h |
| CauHoi | Question | Question.h |
| DiemThi | Score | Score.h |
| MALOP | Class code | Class.h |
| TENLOP | Class name | Class.h |
| MAMH | Subject code | Subject.h |
| TENMH | Subject name | Subject.h |
| MASV | Student code | Student.h |
| HO | Last name | Student.h |
| TEN | First name | Student.h |
| PHAI | Gender | Student.h |
| NOIDUNG | Content | Question.h |
| DAPAN_DUNG | Correct answer | Question.h |
| NhapChuoi | Input string | Validation.cpp |
| NhapMa | Input code | Validation.cpp |

**Confidence**: Verified

### TypeScript/React Conventions (from Frontend)

| Convention | Example |
|-----------|---------|
| PascalCase components | `StudentDashboard`, `ClassManagement`, `SubjectDetail` |
| camelCase services/functions | `authService`, `getClasses()`, `getStudentById()` |
| `.tsx` extension for components | All page and component files |
| shadcn/ui component naming | `button.tsx`, `card.tsx`, `input.tsx` |
| Functional components with hooks | `export default function Page() { ... }` |
| React Router v7 syntax | `<Route path="/..." element={<Component />} />` |

**Confidence**: Verified

### Documentation Convention
- Documents use Vietnamese with English technical terms
- Business Rules prefixed `BR-` (BR-01 through BR-14 in TongQuan.md)
- Functions referenced as `a/`, `b/`, `c/`, etc. matching assignment spec

**Confidence**: Verified
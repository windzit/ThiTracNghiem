# 14 — Decisions

## Architecture Decisions

| Decision | Choice | Rationale | Date | Confidence |
|----------|--------|-----------|------|------------|
| Data structures | BST + linked lists + array of pointers | Per assignment spec (debai.md) | Original | Verified |
| File format | Pipe-delimited TXT | Simple, human-readable, easy to parse | Original | Verified |
| Console I/O | getch() from conio.h | Windows-specific, real-time key capture | Original | Verified |
| BST auto-balancing | Perfect tree rebuild on insert | Maintains O(log n) search time | Original | Verified |
| Sorted linked lists | Insert maintains sorted order | Enables O(n) search with early termination | Original | Verified |
| Legacy Init.h | Separate from include/ headers | Pre-dates class-based design, still used by Auth/Report | Original | Verified |
| HTTP library | httplib.h (header-only) | No build dependency, single-file include | Original | Verified |
| Frontend framework | React + Vite + TypeScript | Modern web stack, separate from C++ backend | Original | Verified |
| Frontend data | Mock data only | No backend connection implemented yet | Original | Verified |

## Business Rule Decisions

| Rule | Decision | Source | Confidence |
|------|----------|--------|------------|
| BR-01 | GV/GV → admin; MASV/password → student | debai.md | Verified |
| BR-02 | One exam per subject per student | debai.md | Verified |
| BR-03 | Cannot delete used question | debai.md | ✅ Implemented in Question::remove() |
| BR-04 | Cannot delete subject with used questions | debai.md | ❌ NOT implemented in Subject::remove() |
| BR-07 | Question count ≤ available | debai.md | Not implemented (no Exam module) |
| BR-08 | Auto-submit on timeout | debai.md | Not implemented (no Exam module) |
| BR-09 | Round score to 1 decimal | debai.md | Not implemented (no Utils module) |
| BR-10 | "Chưa thi" for untaken exams | debai.md | Not implemented (Report stub) |
| BR-11 | System-generated unique ID | debai.md | ❌ NOT implemented (Question::insert takes external ID) |

## Key Technical Decisions

| Decision | Detail | Impact |
|----------|--------|--------|
| Relative paths `../../../storage/` | Executable runs from build directory 3 levels deep | Fragile path dependency |
| No smart pointers | Raw new/delete throughout | Memory leak risk if exceptions occur |
| No JSON serialization | nlohmann/json exists but unused | Cannot easily build REST API |
| No unit tests | No test framework found | Risk of regression |
| CMake builds test.cpp | Not main.cpp | Console app entry point not in build |

**Confidence**: Verified
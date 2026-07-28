# 18 — Issues

## Open Issues Requiring Confirmation

### Issue 1: Init.h vs include/ headers
- **Description**: `src/Auth.cpp` and `src/Report.cpp` include `"Init.h"` which contains duplicate definitions of all data structures (Lop, SinhVien, MonHoc, CauHoi, DiemThi) using raw pointers. The `include/` headers wrap these in classes with proper encapsulation.
- **Evidence**: Auth.cpp:L2 `#include "Auth.h"`, Auth.cpp:L3 `#include "Init.h"` — uses Init.h structs, not Class.h/Student.h/etc.
- **Impact**: Dual code paths — changes to one won't be reflected in the other.
- **Needs**: Decision: migrate to class-based or keep legacy?

### Issue 2: Question ID Generation — NOT IMPLEMENTED
- **Description**: `debai.md` says "Id là số tự động tăng" (auto-increment). `TongQuan.md` says "số ngẫu nhiên". The actual `Question::insert(CauHoi)` takes an externally-provided ID with no generation logic.
- **Evidence**: Question.cpp:L45-67 — insert() takes a complete CauHoi with ID already set.
- **Needs**: Decision on which strategy to implement. `debai.md` is authoritative.

### Issue 3: Utils module — MISSING
- **Description**: Referenced in documentation (Utils.cpp/h) but neither file exists.
- **Evidence**: No `include/Utils.h` or `src/Utils.cpp` found.
- **Needs**: Create Utils module with random ID generation, score rounding, string utilities.

### Issue 4: Exam module — MISSING
- **Description**: `include/Exam.h` is a stub with only `void InitExamModule();`. `src/Exam.cpp` does not exist.
- **Evidence**: No Exam.cpp anywhere in the project.
- **Needs**: Implement full exam flow (g/).

### Issue 5: Menu module — STUB
- **Description**: `src/Menu.cpp` only prints "Main menu is not implemented yet."
- **Evidence**: Menu.cpp:L4-6.
- **Needs**: Full menu implementation with GV and SV flows.

### Issue 6: CMakeLists.txt builds test.cpp, not main.cpp
- **Description**: CMake target `server` compiles `test.cpp`, not `main.cpp`.
- **Evidence**: CMakeLists.txt:L10-17 lists test.cpp as source.
- **Needs**: Confirmation: is this intentional?

### Issue 7: server.cpp has no business endpoints
- **Description**: server.cpp has httplib.h but only a single `GET /` route returning "Welcome to My Project".
- **Evidence**: server.cpp:L30-32.
- **Needs**: Decision: should server.cpp become the API layer connecting frontend to backend?

### Issue 8: Document duplication
- **Description**: `document/UI_goiy.md` is identical to `document/TongQuan.md`.
- **Needs**: Can UI_goiy.md be removed?

### Issue 9: Frontend-backend gap
- **Description**: Frontend uses mock data with no HTTP connection to the C++ backend. Axios is installed but unused.
- **Evidence**: All services return mock data synchronously. No API base URL configured.
- **Needs**: Decision: should they be connected or remain separate projects?

### Issue 10: BR-04 not implemented in Subject::remove()
- **Description**: Subject::remove() does not check if any questions have been used before deleting.
- **Evidence**: Subject.cpp:L129-131 — remove() calls _remove() directly without business rule check.
- **Needs**: Implement BR-04 check before deletion.

### Issue 11: Relative path fragility
- **Description**: All storage paths use `"../../../storage/"` prefix, assuming executable runs from a specific build directory.
- **Impact**: Will break if build output structure changes.
- **Needs**: Use absolute paths or CMake-configured paths.

### Issue 12: storage/ID_Question.txt purpose unknown
- **Description**: File exists but is never referenced in any source code.
- **Needs**: Clarify its purpose or remove.

## Resolved Issues

| Issue | Resolution | Date |
|-------|-----------|------|
| CMake/httplib errors | Ignore — Visual Studio purple squiggles, not real errors | 2026-07-24 |
| Authoritative spec | `debai.md` is authoritative, not `TongQuan.md` | 2026-07-24 |

**Confidence**: All issues Verified (confirmed by source code analysis)
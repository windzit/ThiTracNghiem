# 13 — Workflow

## Authentication Flow (a/)

```
[Start] → User enters username + password
  → login_teacher(username, password)
    → if "GV"/"GV" → role = TEACHER → Show GV Menu
  → login_student(dsLop, username, password)
    → iterate all classes → iterate all students
      → if MASV + password match → role = STUDENT → Show SV Menu
    → if not found → "Sai mã đăng nhập hoặc mật khẩu" → retry
```

**Status**: ✅ Implemented  
**Note**: `login_student()` uses `Init.h` legacy types (raw dsLop struct, not Class class)  
**Confidence**: Verified

## Class CRUD Flow (b/, c/)

### Create Class (b/)
```
[GV] → Enter MALOP → Enter TENLOP
  → Class::find(MALOP) → if exists → error
  → Class::insert(new Lop) → success
```

### List Classes (c/)
```
[GV] → Class::load() → iterate dsLop
  → PrintClassListReport(dsLop) → console table
```

**Status**: ✅ Implemented  
**Confidence**: Verified

## Student CRUD Flow (d/)

```
[GV] → Enter MALOP → Class::find(MALOP)
  → if not found → "Lớp không tồn tại"
  → Enter MASV, HO, TEN, PHAI, password
  → Student::insert(SinhVien) → linked list
  → Repeat? (Y/N)
```

**Status**: ✅ Implemented  
**Note**: Insert maintains sorted order by MASV  
**Confidence**: Verified

## Subject CRUD Flow (e/)

### Create Subject
```
[GV] → Enter MAMH → Enter TENMH
  → Subject::find(MAMH) → if exists → error
  → Subject::insert(MonHoc) → BST (auto-balanced)
```

### Delete Subject
```
[GV] → Enter MAMH → Subject::find(MAMH)
  → if not found → error
  → (BR-04 check: any questions used?) → if yes → reject
  → Subject::remove(MAMH) → BST delete
```

### Edit Subject
```
[GV] → Enter MAMH → Subject::find(MAMH)
  → Update TENMH only (MAMH is immutable)
```

**Status**: ✅ Implemented  
**Note**: BR-04 (cannot delete if questions used) is NOT implemented in Subject::remove() — the code doesn't check used flag before deletion  
**Confidence**: Verified

## Question CRUD Flow (f/)

### Create Question
```
[GV] → Enter MAMH → Subject::find(MAMH)
  → System generates unique ID (NOT IMPLEMENTED)
  → Enter NOIDUNG, A, B, C, D, DAPAN_DUNG
  → Question::insert(CauHoi) → linked list
```

### Delete Question
```
[GV] → Enter MAMH + ID → Question::find(ID)
  → if cauhoi.used == true → reject (BR-03)
  → Question::remove(ID) → linked list delete
```

**Status**: ✅ Implemented (except ID generation)  
**Note**: BR-03 (cannot delete used question) IS implemented in Question::remove()  
**Confidence**: Verified

## Exam Flow (g/) — NOT IMPLEMENTED

```
[SV] → Select Subject → Check not already taken (BR-02)
  → Enter question count → Validate ≤ available (BR-07)
  → Enter time limit → Validate > 0 (BR-08)
  → Randomly select questions (no duplicates)
  → Display questions with countdown timer
  → Student answers → Submit (manual or auto on timeout)
  → Auto-grade → Round score (BR-09)
  → Save DiemThi → Mark questions as used
  → Show result
```

**Status**: ❌ Entirely missing  
**Dependencies**: Exam.cpp, Utils.cpp (random, timer), Question (used-flag), Score (insert)  
**Confidence**: Verified

## Report Flows (h/, i/)

### Exam Detail (h/)
```
[GV/SV] → Enter MASV + MAMH
  → Find Score record → Load exam detail
  → Print questions + answers + student choices + score
```

**Status**: ⚠ `PrintExamDetailReport()` is a stub  
**Confidence**: Verified

### Scoreboard (i/)
```
[GV] → Enter MALOP + MAMH
  → Iterate all students in class
  → For each: lookup score → print score or "Chưa thi"
  → Print summary (count, average)
```

**Status**: ⚠ `PrintScoreboardReport()` is a stub  
**Confidence**: Verified

## Data Persistence Flow

### Load (Startup)
```
Storage::LoadAllData() [STUB]
  → Class::load() → Student::load() → Score::load()
  → Subject::load() → Question::load()
```

### Save (Manual/Shutdown)
```
Storage::SaveAllData() [STUB]
  → Class::save() → Student::save() → Score::save()
  → Subject::save() → Question::save()
```

**Status**: ⚠ Storage orchestration is stub; individual module save/load works  
**Confidence**: Verified

---

## Build Baseline (Task 2.0 — 2026-07-24)

### Bước 1: CMake Configure
```
cmake -S . -B build
```
- Generator: Visual Studio 18 2026 (MSVC 19.51, x64, Windows SDK 10.0.26100.0)
- Config: ✅ Thành công
- Output: `build/server.vcxproj`

### Bước 2: CMake Build
```
cmake --build build
```
- **Target duy nhất**: `server` (từ `test.cpp`, dòng 11 CMakeLists.txt)
- **Kết quả**: ❌ Build lỗi — 7 errors, tất cả ở `test.cpp`
- **Lỗi cụ thể**: `NodeMH::monhoc` không phải member của `NodeMH` (test.cpp dòng 48,49,53,62,77,78,80)
  - `test.cpp` dùng field `monhoc` (style cũ từ Init.h), nhưng `include/Subject.h` đã đổi cấu trúc `NodeMH` — không còn field `monhoc`
- **Không sinh được exe** → không thể chạy thử runtime

### Chẩn đoán
| Vấn đề | Chi tiết |
|--------|----------|
| Build target sai | CMakeLists.txt dòng 11: `"test.cpp"` thay vì `"main.cpp"` hoặc `"server.cpp"` |
| test.cpp lỗi compile | Dùng API cũ không tương thích với class-based headers |
| Thiếu file server.cpp | `server.cpp` không có trong `add_executable` |
| Thiếu nhiều module | Auth, Report, Storage, Menu, Utils, Exam, Validation không được include |

### Kết luận
- **Nguyên nhân server chỉ chạy được trong VS Tím**: VS Tím cho phép chọn Startup Project khác (có thể là project khác build đúng main.cpp/server.cpp). CLI build mặc định theo CMakeLists.txt → build nhầm test.cpp.
- **Cần fix (Task 2.2)**: Sửa CMakeLists.txt để build `server.cpp` (hoặc `main.cpp`) thay vì `test.cpp`, thêm đủ các module còn thiếu.
- **Runtime chưa test được** vì build chưa qua.

**Confidence**: Verified (log thật từ CLI)

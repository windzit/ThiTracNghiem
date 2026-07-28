# 07 — Storage

## Persistence Strategy

**Flat Unified Storage** — toàn bộ dữ liệu lưu trong 9 file văn bản phẳng chuẩn hóa (UTF-8, LF `\n`, phân cách cột `|`) tại thư mục `storage/`. Dữ liệu nạp vào RAM lúc khởi động qua `StorageManager` và ghi lại đĩa qua Atomic Write (file `.tmp` + rename) theo Save Trigger Policy.

**Confidence**: Verified (Phase 7 Complete)

## Cấu trúc thư mục đĩa (9 file phẳng)

```text
storage/
├── metadata.txt         # SCHEMA_VERSION + LAST_QUESTION_ID
├── classes.txt          # MALOP|TENLOP
├── students.txt         # MALOP|MASV|HO|TEN|PHAI|PASSWORD
├── subjects.txt         # MAMH|TENMH|USED
├── questions.txt        # MAMH|ID|NOIDUNG|A|B|C|D|DAPAN_DUNG|USED
├── scores.txt           # MASV|MAMH|DIEM
├── exam_history.txt     # MASV|MAMH|THOIGIAN_BATDAU|DIEM|QUESTION_IDS|ANSWERS
├── exam_sessions.txt    # MASV|MAMH|THOIGIAN_BATDAU|TONGPHUT|IN_PROGRESS|LAST_ACT|QUESTION_IDS|ANSWERS
└── transaction.log      # (rỗng 0-byte, dự phòng replay logic)
```

## File Format Details

### metadata.txt
```text
SCHEMA_VERSION=2.0
LAST_QUESTION_ID=1045
```
- Lưu phiên bản schema và bộ đếm Question ID toàn cục (auto-increment).

### classes.txt
```text
MALOP|TENLOP
D22CQCN01-N|Lop Cong nghe thong tin 1 Khoa 2022
```
- Header dòng 1, dữ liệu từ dòng 2.

### students.txt
```text
MALOP|MASV|HO|TEN|PHAI|PASSWORD
D22CQCN01-N|N22DCCN001|Nguyen Van|An|Nam|123456
```
- Tất cả sinh viên của tất cả lớp trong 1 file. `MALOP` ở cột 1 để phân bổ.

### subjects.txt
```text
MAMH|TENMH|USED
C++|Lap trinh C++|1
```
- `USED` = 0 hoặc 1.

### questions.txt
```text
MAMH|ID|NOIDUNG|A|B|C|D|DAPAN_DUNG|USED
C++|1|Polymorphism la gi?|Da hinh|Dong goi|Kua ke|Truy van|A|1
```
- Tất cả câu hỏi của tất cả môn trong 1 file. `MAMH` ở cột 1 để phân bổ.

### scores.txt
```text
MASV|MAMH|DIEM
N22DCCN001|C++|9.50
```
- Tất cả điểm thi của tất cả sinh viên trong 1 file.

### exam_history.txt
```text
MASV|MAMH|THOIGIAN_BATDAU|DIEM|QUESTION_IDS|ANSWERS
N22DCCN001|C++|1722000000|9.50|1,5,12,8|A,B,C,D
```
- Lịch sử bài thi đã nộp. `QUESTION_IDS` và `ANSWERS` phải cùng thứ tự chỉ số.

### exam_sessions.txt
```text
MASV|MAMH|THOIGIAN_BATDAU|TONGPHUT|IN_PROGRESS|LAST_ACT|QUESTION_IDS|ANSWERS
N22DCCN002|C++|1722005000|15|1|1722005100|2,4,9|A,-,C
```
- Phiên thi đang làm dở. `LAST_ACT` = `lastServerActivityAt` cho downtime compensation.

### transaction.log
- File rỗng 0-byte, dự phòng cho replay logic (chưa triển khai).

## Storage Architecture

### StorageManager (Singleton)
- `include/StorageManager.h` + `src/StorageManager.cpp`
- Interface duy nhất cho Load/Save toàn bộ dữ liệu.
- Các module nghiệp vụ (`Class`, `Student`, `Subject`, `Question`, `Score`) chỉ gọi `markDirty()` + `incrementOpCount()`, không tự ghi đĩa.

### PathResolver
- `include/PathResolver.h` + `src/PathResolver.cpp`
- Giải quyết đường dẫn `storage/` từ bất kỳ working directory nào.

### Atomic Write
```cpp
bool StorageManager::atomicWriteFile(targetPath, content) {
    // 1. Ghi vào file .tmp
    // 2. flush + close
    // 3. std::filesystem::rename(.tmp, target) — atomic
}
```

### Save Trigger Policy
- Sau mỗi CRUD: `markDirty()` + `opCount++` (không ghi đĩa ngay).
- Ghi đĩa khi: `opCount >= 100` HOẶC `POST /api/admin/save` HOẶC thoát server.

## Serialization/Deserialization

### Load (Startup)
`StorageManager::loadAllData(dsl, dsmh)`:
1. Đọc `metadata.txt` → `LAST_QUESTION_ID`
2. Đọc `classes.txt` → nạp `dsl`
3. Đọc `students.txt` → phân bổ vào `Lop` theo `MALOP`
4. Đọc `subjects.txt` → nạp BST `dsmh`
5. Đọc `questions.txt` → phân bổ vào `MonHoc` theo `MAMH`
6. Đọc `scores.txt` → phân bổ vào `SinhVien` theo `MASV`

### Save (Shutdown / Manual)
`StorageManager::saveAllData(dsl, dsmh)`:
1. Ghi `metadata.txt`
2. Ghi `classes.txt` + `students.txt`
3. Ghi `subjects.txt` + `questions.txt`
4. Ghi `scores.txt`
5. Reset `dirty = false`, `opCount = 0`

## File I/O Summary (Post Phase 7)

| Entity | Save | Load | File |
|--------|------|------|------|
| Class | `StorageManager::markDirty()` | `StorageManager::loadClasses()` | `classes.txt` |
| Student | `StorageManager::markDirty()` | `StorageManager::loadStudents()` | `students.txt` |
| Subject | `StorageManager::markDirty()` | `StorageManager::loadSubjects()` | `subjects.txt` |
| Question | `StorageManager::markDirty()` | `StorageManager::loadQuestions()` | `questions.txt` |
| Score | `StorageManager::markDirty()` | `StorageManager::loadScores()` | `scores.txt` |
| ExamSession | `StorageManager::saveExamSession()` | `StorageManager::loadExamSessions()` | `exam_sessions.txt` |
| ExamHistory | `StorageManager::appendExamHistory()` | — | `exam_history.txt` |
| QuestionID | `StorageManager::getNextQuestionID()` | `metadata.txt` | `metadata.txt` |

## Legacy Removed (Phase 7)

- ❌ `storage/Students/` — removed
- ❌ `storage/Questions/` — removed
- ❌ `storage/ScoreList/` — removed
- ❌ `storage/ExamSessions/` — removed
- ❌ `storage/ID_Question.txt` — removed (merged into `metadata.txt`)
- ❌ `PATH_CLASSES_TEXT`, `PATH_FOLDER_STUDENTS`, `PATH_FOLDER_SCORELIST`, `PATH_SUBJECT_TXT`, `PATH_FOLDER_QUESTIONS`, `PATH_ID_TEXT` — removed
- ❌ `GenerateRandomQuestionId()` — removed
- ❌ `Subject::_save(NodeMH*&, std::ofstream&)` — removed

**Confidence**: Verified (Phase 7 Complete, TC-03 re-test PASS)
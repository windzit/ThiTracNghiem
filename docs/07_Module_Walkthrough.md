# 07. Luồng Xử lý Chi tiết các Mô-đun Nghiệp vụ (Module Walkthrough)

Tài liệu này đi sâu phân tích luồng xử lý chi tiết của từng **Mô-đun Nghiệp vụ** chính trong hệ thống từ khi nhận tín hiệu đến khi hoàn tất lưu trữ.

---

## 🔐 1. Mô-đun Xác thực & Phân quyền (Auth Flow)

**Tệp liên quan**: `server/handlers/AuthHandler.cpp`, `server/ServerContext.cpp`

### Luồng Đăng nhập (`POST /api/login`)

```
[Client gửi] { "username": "N21001", "password": "matkhau123" }
         │
         ▼
[AuthHandler::handle_login]
         │
         ├── username == "GV"?
         │     ├── Đúng: Verify pass == "GV" (plain text)
         │     │         ├── Match → Return { role:"GV", username:"GV", displayName:"Giảng viên" }
         │     │         └── Sai  → 401 "Sai mật khẩu"
         │     │
         │     └── Sai: Coi là Mã Sinh Viên → DB_READ_LOCK
         │
         ├── findStudentGlobal("N21001")
         │     ├── Duyệt toàn bộ dsLop[0..n] → Với mỗi lớp: duyệt dsSinhVien
         │     ├── Tìm thấy? → So sánh password (plain/hash)
         │     │     ├── Match → Return { role:"SV", masv:"N21001", ho:"Nguyen", ten:"An" }
         │     │     └── Sai  → 401 "Sai mật khẩu"
         │     └── Không tìm thấy → 404 "Không tìm thấy sinh viên"
```

---

## 📚 2. Mô-đun Quản lý Danh mục

### 2.1 Thêm Môn học mới (Cây BST)

**Tệp**: `server/handlers/SubjectHandler.cpp`, `src/Subject.cpp`

```
[POST /api/subjects] { "mamh": "CTDL", "tenmh": "Cấu trúc dữ liệu" }
         │
         ▼
[1] Normalize: mamh = toUpperCase("CTDL") = "CTDL"
[2] DB_WRITE_LOCK
[3] dsmh.insert(monhoc):
    ├── strcmp("CTDL", root->MAMH)?
    │     == 0 → Lỗi "Mã môn học đã tồn tại" (422)
    │     < 0  → Đệ quy nhánh left
    │     > 0  → Đệ quy nhánh right
    └── Chèn nút mới NodeMH vào đúng vị trí BST

[4] StorageManager::appendSubject(monhoc, outOffset)
    └── Định dạng 71 bytes → Ghi vào cuối subjects.txt → Lấy offset

[5] IndexManager::updateSubjectOffset("CTDL", offset)
    └── m_subjectIndex.insert("CTDL", offset) → Cập nhật subject.idx

→ Response 201: { "mamh": "CTDL", "tenmh": "Cấu trúc dữ liệu" }
```

### 2.2 Xóa Câu hỏi Thi (Hard Delete vs Soft Delete)

**Tệp**: `server/handlers/QuestionHandler.cpp`, `src/Question.cpp`

```
[DELETE /api/questions/105]
         │
         ▼
[1] DB_WRITE_LOCK
[2] IndexManager::getQuestionOffset(105, offset) → offset = 40480
[3] StorageManager::readQuestionAt(40480) → Lấy CauHoi object
[4] Kiểm tra cờ used:

    used == false (Câu hỏi CHƯA bao giờ được thi)
    ├── Duyệt BST tìm NodeMH chứa câu hỏi này
    ├── dsCHT.removeNode(105)          ← Hủy nút khỏi RAM
    ├── markQuestionStatusAt(40480, '1')  ← Ghi cờ '1' vào byte 734 trên đĩa
    ├── IndexManager::removeQuestionOffset(105) ← Xóa khỏi HashTable
    └── incrementDeletedCount("question") ← Bộ đếm rác++
                                            Nếu >= 50 → compactQuestions()!

    used == true (Câu hỏi ĐÃ TỪNG được thi — phải giữ cho lịch sử!)
    ├── cauhoi.deleted = true           ← Đánh dấu trên RAM
    ├── markQuestionStatusAt(40480, '2')  ← Cờ '2' (Soft Deleted, khác '1'!)
    └── IndexManager::removeQuestionOffset(105) ← Xóa khỏi HashTable

→ Response 204: No Content
```

**Tại sao cờ '2' khác cờ '1'?**
```
Cờ '1' (DELETED):       Rác thực sự → Compaction sẽ XÓA khỏi file .txt
Cờ '2' (SOFT_DELETED):  Đã thi → Compaction GIỮ LẠI để tra cứu lịch sử bài thi
```

---

## 🎯 3. Mô-đun Thi Trắc nghiệm

### 3.1 Bắt đầu Thi (`POST /api/exams/start`)

**Tệp**: `server/handlers/ExamHandler.cpp`, `src/Exam.cpp`

```
[POST /api/exams/start]
{ "masv": "N21001", "mamh": "CTDL", "soCau": 10, "thoiGianPhut": 15 }
         │
         ▼
[1] DB_WRITE_LOCK
[2] Tìm SinhVien "N21001" trên RAM (findStudentGlobal)
[3] Kiểm tra đã thi chưa: dsdiemthi.findByMAMH("CTDL")
    └── Nếu tồn tại → 422 "Sinh viên đã hoàn thành bài thi môn này"

[4] Tìm NodeMH "CTDL" trên BST
[5] Thu thập câu hỏi hợp lệ:
    ├── Duyệt dsCHT của môn "CTDL"
    ├── Lọc: chỉ lấy câu hỏi có deleted == false và used == false
    └── Đủ soCau câu hỏi hợp lệ? Nếu thiếu → 422 "Không đủ câu hỏi"

[6] Fisher-Yates Shuffle (Tráo ngẫu nhiên):
    Mảng: [101, 102, 103, 104, 105, 106, 107, 108, 109, 110]
    for i = 9 → 1:
        j = random(0, i)
        swap(arr[i], arr[j])
    Lấy 10 câu đầu tiên sau khi xáo

[7] Tạo ExamSession:
    session.MASV = "N21001"
    session.MAMH = "CTDL"
    session.questionIds = [103, 107, 101, ...]  ← Thứ tự ngẫu nhiên
    session.answers = ['-','-','-',...]          ← Chưa trả lời
    session.thoiGianBatDau = time(nullptr)       ← Timestamp bắt đầu
    session.tongThoiGianPhut = 15
    session.in_progress = true

[8] saveExamSession() → Ghi xuống exam_sessions.txt (Phòng mất điện!)

[9] Trả về đề thi (ẨN trường DAPAN_DUNG, chỉ gửi ID + NOIDUNG + A + B + C + D)
```

### 3.2 Phục hồi Phiên thi (`GET /api/exams/resume?masv=N21001`)

```
[Sinh viên F5 trang / Mở lại tab]
         │
         ▼
[1] Tìm ExamSession của "N21001" trong cachedExamSessions

[2] Tính thời gian còn lại:
    remainingSeconds = (tongThoiGianPhut × 60) - (now - thoiGianBatDau)
    └── remainingSeconds <= 0? → Tự động submit bài (nộp muộn)

[3] Trả về:
    ├── Danh sách câu hỏi (đúng thứ tự cũ)
    ├── Mảng answers đã chọn ['A', '-', 'C', '-', ...] ← Đáp án dở dang
    └── remainingSeconds = 847  ← Thời gian còn lại chính xác!
```

### 3.3 Lưu tạm Đáp án (`PUT /api/exams/answer`)

```
[Sinh viên chọn đáp án "B" cho câu số 3]
[PUT] { "masv": "N21001", "questionIndex": 2, "answer": "B" }
         │
         ▼
[1] Tìm session của "N21001" trong cachedExamSessions
[2] session.answers[2] = 'B'    ← Cập nhật RAM
[3] flushExamSessionsFile()     ← Ghi ngay xuống exam_sessions.txt

→ Kể cả mất điện ngay lúc này: Đáp án 'B' đã được bảo toàn trên đĩa!
```

---

## ✅ 4. Mô-đun Nộp Bài & Chấm Điểm

**Tệp**: `server/handlers/ExamHandler.cpp`, `src/Score.cpp`, `src/StorageManager.cpp`

### Luồng Nộp bài (`POST /api/exams/submit`)

```
[POST /api/exams/submit] { "masv": "N21001" }
         │
         ▼
[1] Tìm ExamSession "N21001" (RAM)
[2] Chấm điểm từng câu:
    for i = 0 to 9:
        questionId = session.questionIds[i]
        SV đã chọn: session.answers[i] = 'A'
        Đáp án đúng: dsCauHoi.findById(questionId).DAPAN_DUNG = 'A'
        if answers[i] == DAPAN_DUNG → correctCount++

    FinalScore = (correctCount × 10.0) / totalQuestions
    VD: 7 đúng / 10 câu → 7.0 điểm

[3] DB_WRITE_LOCK
[4] sv->dsdiemthi.insert({MAMH="CTDL", DIEM=7.0})  ← RAM: Gắn điểm vào SV
[5] Đánh dấu cờ used = true:
    ├── monhoc.used = true           ← Môn học này đã được thi
    └── Với mỗi questionId trong đề:
        cauhoi.used = true            ← Câu hỏi này đã được thi

[6] StorageManager::saveScores(dsl) ← Ghi điểm xuống scores.txt
[7] StorageManager::appendExamHistory(session, 7.0):
    ├── Thêm dòng mới vào exam_history.txt (chi tiết từng câu + đáp án SV)
    ├── IndexManager::rebuildHistoryIndex() ← Cập nhật history.idx
    └── IndexManager::saveHistoryIndex()

[8] StorageManager::removeExamSession("N21001")
    └── Xóa phiên thi khỏi RAM + exam_sessions.txt

→ Response 200: { "diem": 7.0, "soCauDung": 7, "tongSoCau": 10 }
```

---

## 📊 5. Mô-đun Báo cáo & Thống kê

**Tệp**: `server/handlers/ReportHandler.cpp`, `src/Report.cpp`

### 5.1 Xem Chi tiết Bài thi Lịch sử (`GET /api/reports/exam?masv=N21001&mamh=CTDL`)

```
[1] IndexManager::getHistoryOffsets("N21001", offsets)
    └── m_historyIndex.get("N21001") → [147200, 256500, ...]  ← O(1) HashTable

[2] Với mỗi offset trong offsets:
    ├── StorageManager::readHistoryAt(offset) → ExamHistory record
    ├── Lọc: Record có MAMH == "CTDL" không?
    └── Nếu có: Lấy danh sách questionIds + answers

[3] Với mỗi câu hỏi trong bài thi:
    ├── Tra question.idx → Lấy offset của câu hỏi
    ├── readQuestionAt(offset) → Lấy NOIDUNG, A, B, C, D, DAPAN_DUNG
    └── Ghép cặp: { question, svAnswer, correctAnswer }

[4] Response: Danh sách đầy đủ từng câu:
    [
      { "noidung": "BST là gì?", "a":"Cây...", ..., "svAnswer":"A", "correct":"A", "isRight": true },
      { "noidung": "Heap là gì?", ..., "svAnswer":"B", "correct":"C", "isRight": false },
      ...
    ]
```

### 5.2 Bảng điểm Lớp (`GET /api/reports/scoreboard?malop=D21CQCN01&mamh=CTDL`)

```
[1] Tìm Lop "D21CQCN01" trong dsLop (duyệt mảng O(N))
[2] Duyệt dsSinhVien của lớp đó:
    Với mỗi SinhVien sv:
    ├── sv.dsdiemthi.findByMAMH("CTDL")
    │     ├── Tìm thấy → ghi nhận DIEM = 8.5
    │     └── Không có → ghi nhận "Chưa thi"
    └── Thêm vào danh sách kết quả

[3] Sắp xếp theo TÊN + HỌ (đã sẵn sàng từ cấu trúc dsSinhVien có thứ tự)

→ Response: [ { masv, ho, ten, diem: 8.5 }, { masv, ho, ten, diem: "Chưa thi" }, ... ]
```

---

## 📁 6. Các File Mã Nguồn Liên quan

| File | Nhiệm vụ |
| :--- | :--- |
| [AuthHandler.cpp](file:///server/handlers/AuthHandler.cpp) | Đăng nhập GV / SV |
| [ClassHandler.cpp](file:///server/handlers/ClassHandler.cpp) | CRUD Lớp học |
| [StudentHandler.cpp](file:///server/handlers/StudentHandler.cpp) | CRUD Sinh viên |
| [SubjectHandler.cpp](file:///server/handlers/SubjectHandler.cpp) | CRUD Môn học trên BST |
| [QuestionHandler.cpp](file:///server/handlers/QuestionHandler.cpp) | CRUD Câu hỏi (Hard/Soft delete) |
| [ExamHandler.cpp](file:///server/handlers/ExamHandler.cpp) | Bắt đầu thi, Phục hồi, Ghi tạm đáp án, Nộp bài |
| [ReportHandler.cpp](file:///server/handlers/ReportHandler.cpp) | Báo cáo chi tiết bài thi, Bảng điểm lớp |
| [AdminHandler.cpp](file:///server/handlers/AdminHandler.cpp) | Lưu khẩn cấp, Rebuild cờ used, Cài đặt hệ thống |
| [Exam.cpp](file:///src/Exam.cpp) | Fisher-Yates Shuffle, tính điểm |

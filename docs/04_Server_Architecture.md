# 04. Kiến trúc Server C++ (Server Architecture)

Tài liệu này giải thích chi tiết kiến trúc tầng **Web Server C++**, mô hình đồng bộ đa luồng (Multithreaded Concurrency), cơ chế phân giải API Route, quản lý Trạng thái Toàn cục và xử lý ngoại lệ.

---

## 🏛️ 1. Kiến trúc Tổng quan Web Server

```
          [ HTTP Request đến cổng 8080 ]
                       │
                       ▼
          ┌────────────────────────────┐
          │   httplib::Server          │
          │   (Thread Pool Threads)    │  ← Mỗi request → 1 thread xử lý
          └────────────┬───────────────┘
                       │
                       ▼
          ┌────────────────────────────┐
          │   RouteRegistry            │  ← Khớp URL + HTTP Method
          │   POST /api/students       │
          │   GET  /api/classes        │
          │   ...                      │
          └────────────┬───────────────┘
                       │
                       ▼
          ┌────────────────────────────┐
          │   Handlers (7 handlers)    │  ← Parse JSON, Validate, Gọi Core
          │   AuthHandler              │
          │   ClassHandler             │
          │   StudentHandler           │
          │   SubjectHandler           │
          │   QuestionHandler          │
          │   ExamHandler              │
          │   ReportHandler + Admin    │
          └────────────┬───────────────┘
                       │
                       ▼
          ┌────────────────────────────┐
          │   ServerContext            │  ← Shared State toàn hệ thống
          │   Class dsl                │  ← Tất cả Lớp + Sinh viên + Điểm
          │   Subject dsmh             │  ← Tất cả Môn học + Câu hỏi
          │   shared_mutex g_dbMutex   │  ← Khóa đồng bộ đa luồng
          └────────────────────────────┘
```

---

## 🔐 2. Đồng bộ Đa luồng — Read-Write Lock

### 2.1 Vấn đề: Data Race (Dữ liệu bị hỏng)

```
🚗 Phép so sánh: Ngân hàng có 1 quầy giao dịch

Tình huống: 10 sinh viên nộp bài THI cùng lúc (10 luồng ghi đồng thời)

❌ Không có khóa (Nguy hiểm!):
   Thread 1: Đọc điểm_hiện_tại = 8.5, chuẩn bị ghi...
   Thread 2: Đọc điểm_hiện_tại = 8.5, chuẩn bị ghi...
   Thread 1: Ghi điểm = 9.0
   Thread 2: Ghi điểm = 7.5   ← Ghi đè mất kết quả của Thread 1!
   → Dữ liệu bị hỏng (Data Corruption) ❌
```

### 2.2 Giải pháp: `std::shared_mutex` (Read-Write Lock)

```cpp
// ServerContext.h
extern std::shared_mutex g_dbMutex;

// Macros tiện ích dùng trong mọi Handler:
#define DB_READ_LOCK  std::shared_lock<std::shared_mutex> _rl(g_dbMutex)
#define DB_WRITE_LOCK std::unique_lock<std::shared_mutex> _wl(g_dbMutex)
```

**Quy tắc đơn giản:**
```
📖 DB_READ_LOCK (shared_lock):
   ├── Dùng cho: GET /api/classes, GET /api/students, GET /api/reports
   ├── Cho phép: NHIỀU luồng đọc đồng thời (không chặn nhau)
   └── Ngăn chặn: Luồng ghi bắt đầu ghi trong khi đang có luồng đọc

✏️ DB_WRITE_LOCK (unique_lock):
   ├── Dùng cho: POST, PUT, DELETE (tất cả thao tác ghi)
   ├── Cho phép: CHỈ 1 luồng được thực thi tại một thời điểm
   └── Ngăn chặn: Mọi luồng đọc/ghi khác phải chờ hàng đợi

Khi scope kết thúc: Lock TỰ ĐỘNG được giải phóng (RAII pattern)
```

**Minh họa đồng thời:**
```
Thời gian: T1     T2     T3     T4     T5
Thread A (GET):    ══════ READ  ══════
Thread B (GET):          ══════ READ  ══════   ← Song song với A ✓
Thread C (POST):               ║WAIT║══ WRITE ══  ← Phải chờ A và B xong
Thread D (GET):                              ══════ READ ══  ← Sau khi C xong
```

---

## 🚀 3. Khởi động Server (`ServerBootstrap`)

### 3.1 Chu trình Khởi động Hoàn chỉnh

```
main.cpp / server.cpp
    │
    ▼
[1] PathResolver::init()
    └── Xác định đường dẫn tuyệt đối đến storage/ (bất kể thư mục thực thi)

[2] StorageManager::checkAndExecuteStartupCompaction()
    └── Dọn rác TRƯỚC khi nạp — đảm bảo file .txt sạch khi build index

[3] IndexManager::auditAndLoadIndexes()
    ├── Kiểm tra từng .idx file
    └── Rebuild (nếu thiếu) → Nạp vào HashTable RAM

[4] LoadAllData(dsl, dsmh)
    ├── loadClasses()    → Nạp dsLop + dsSinhVien vào RAM
    ├── loadStudents()   → Nạp thông tin SV active
    ├── loadSubjects()   → Nạp BST NodeMH vào RAM
    ├── loadQuestions()  → Nạp dsCHT câu hỏi vào BST
    ├── loadScores()     → Nạp dsDiemThi vào từng SinhVien
    └── loadExamSessions()  → Phục hồi phiên thi dang dở

[5] adjustSessionsForDowntime()
    └── Bù trừ thời gian server bị tắt vào phiên thi đang chạy

[6] registerRoutes(svr)
    └── Đăng ký tất cả API Endpoints vào RouteRegistry

[7] svr.listen("0.0.0.0", 8080)
    └── Server bắt đầu lắng nghe cổng 8080 - Sẵn sàng phục vụ!

[8] PrintStartupReport()
    └── In báo cáo toàn bộ trạng thái hệ thống lên terminal
```

### 3.2 CLI Flags (Tham số Khởi động)

```bash
server.exe                     # Chạy bình thường
server.exe --reset-storage     # Backup dữ liệu cũ + Reset về trạng thái trắng
server.exe --test-validation   # Chạy bộ kiểm thử chẩn đoán hợp lệ dữ liệu
```

---

## 🌐 4. Trạng thái Toàn cục (`ServerContext`)

**Tệp**: `server/ServerContext.h`, `server/ServerContext.cpp`

```cpp
// Biến toàn cục shared giữa tất cả threads:
extern Class   dsl;              // Toàn bộ Lớp + Sinh viên + Điểm
extern Subject dsmh;             // Toàn bộ Môn học + Câu hỏi
extern std::shared_mutex g_dbMutex;
extern bool    g_fullscreenRequired;  // Cấu hình thi toàn màn hình
```

**Hàm helper toàn cục:**

```cpp
// Tìm sinh viên trong TOÀN BỘ hệ thống (duyệt mảng dsLop → dsSinhVien)
SinhVien* findStudentGlobal(const std::string& masv, Lop** outLop = nullptr);

// Tìm môn học trên Cây BST (khớp chính xác MAMH)
NodeMH* find_subject_smart(const std::string& mamh);
```

---

## 📋 5. Danh sách các Server Handlers

| Handler File | Endpoint Chính | Nhiệm vụ |
| :--- | :--- | :--- |
| [AuthHandler.cpp](file:///server/handlers/AuthHandler.cpp) | `POST /api/login`, `POST /api/logout` | Đăng nhập GV/SV, xác thực password |
| [ClassHandler.cpp](file:///server/handlers/ClassHandler.cpp) | `GET/POST/PUT/DELETE /api/classes` | CRUD Lớp học |
| [StudentHandler.cpp](file:///server/handlers/StudentHandler.cpp) | `GET/POST/PUT/DELETE /api/students` | CRUD Sinh viên, xóa hàng loạt |
| [SubjectHandler.cpp](file:///server/handlers/SubjectHandler.cpp) | `GET/POST/PUT/DELETE /api/subjects` | CRUD Môn học trên BST |
| [QuestionHandler.cpp](file:///server/handlers/QuestionHandler.cpp) | `POST/PUT/DELETE /api/questions` | CRUD Câu hỏi (Hard/Soft Delete tự động) |
| [ExamHandler.cpp](file:///server/handlers/ExamHandler.cpp) | `POST /api/exams/start`, `GET /api/exams/resume`, `POST /api/exams/submit` | Tạo đề ngẫu nhiên, Phục hồi phiên, Chấm điểm |
| [ReportHandler.cpp](file:///server/handlers/ReportHandler.cpp) | `GET /api/reports/exam`, `GET /api/reports/scoreboard` | Xem chi tiết bài thi, Bảng điểm lớp |
| [AdminHandler.cpp](file:///server/handlers/AdminHandler.cpp) | `POST /api/admin/save`, `POST /api/admin/rebuild-used` | Lưu khẩn cấp, Cấu hình hệ thống |

### 5.5 Luồng Validate & Normalize Chuẩn hóa trong Handler

Mọi Handler ghi (`POST`, `PUT`) đều tuân theo quy trình kiểm định 4 bước cố định:

```
[Request JSON từ Client]
         │
         ▼
[Bước 1: Parse JSON → C++ Struct]
         │ (Đọc fields từ body, gán vào struct Lop/SinhVien/MonHoc/CauHoi)
         ▼
[Bước 2: StringNormalizer]
         │ ├── SinhVien: MASV -> normalizeIdentifier (IN HOA), HO/TEN -> toTitleCase
         │ └── CauHoi: NOIDUNG -> normalizeHumanText (giữ nguyên hoa/thường người dùng)
         ▼
[Bước 3: StorageValidator]
         │ ├── Kiểm tra chứa ký tự cấm (|, tab, \r, \n)
         │ └── Kiểm tra trùng phương án (hasDuplicateOptionsAfterNormalization)
         ▼
[Bước 4: Business Rule Check & RAM Insert]
         │ ├── Tra cứu trùng Mã trên RAM (HashTable/BST)
         │ └── Chèn vào RAM & gọi StorageManager sync xuống đĩa
         ▼
[Response 200/201 Envelope]
```

---

## 🔧 6. Cấu trúc JSON Response Chuẩn

```cpp
// ServerContext.h — hai hàm helper dùng trong mọi handler:

// Thành công
json_response(res, data, 200);
// → { "success": true, "data": data }

// Thất bại
error_response(res, "Mã sinh viên đã tồn tại!", 422);
// → { "success": false, "message": "Mã sinh viên đã tồn tại!" }
```

**HTTP Status Codes được dùng:**
```
200 OK          → GET thành công, PUT thành công
201 Created     → POST tạo mới thành công
204 No Content  → DELETE thành công (không có body)
400 Bad Request → Dữ liệu đầu vào sai định dạng
401 Unauthorized→ Sai password
404 Not Found   → Không tìm thấy entity
422 Unprocessable → Vi phạm ràng buộc nghiệp vụ (đã thi rồi, lớp có SV...)
500 Server Error→ Lỗi ghi đĩa hoặc lỗi hệ thống
```

---

## 📁 7. Các File Mã Nguồn Liên quan

| File | Nhiệm vụ |
| :--- | :--- |
| [ServerContext.h](file:///server/ServerContext.h) / [.cpp](file:///server/ServerContext.cpp) | Khai báo shared state, locking macros, JSON helpers |
| [RouteRegistry.h](file:///server/RouteRegistry.h) / [.cpp](file:///server/RouteRegistry.cpp) | Đăng ký tất cả HTTP API Endpoints |
| [ServerBootstrap.h](file:///server/ServerBootstrap.h) / [.cpp](file:///server/ServerBootstrap.cpp) | Khởi tạo, load dữ liệu, mở cổng 8080 |
| [StringNormalizer.h](file:///include/StringNormalizer.h) / [.cpp](file:///src/StringNormalizer.cpp) | Bộ chuẩn hóa chuỗi C++ Backend |
| [StorageValidator.h](file:///include/StorageValidator.h) / [.cpp](file:///src/StorageValidator.cpp) | Bộ kiểm định hợp lệ dữ liệu C++ Backend |
| Thư mục [server/handlers/](file:///server/handlers/) | Tất cả 8 Handler files |


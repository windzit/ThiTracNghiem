# Lộ trình Đọc Code theo Thứ tự Phụ thuộc (Codebase Learning Order)

Tài liệu này xây dựng lộ trình đọc mã nguồn (codebase) theo đúng **Thứ tự Phụ thuộc (Dependency Order)** từ thấp đến cao. Việc tuân thủ đúng thứ tự này kết hợp với quan sát trực quan tệp đĩa phân cách Pipe (`|`) và Báo cáo khởi động Server (Startup Report) sẽ giúp bạn làm chủ 100% hệ thống C++, sẵn sàng tự viết lại từng mô-đun và bảo vệ đồ án đạt điểm tối đa.

---

## 🗺️ Sơ đồ Thứ tự Đọc Code

```text
[Bước 1: Domain Entities & Common Constants]
       │
       ▼
[Bước 2: Custom Core Data Structures]
       │
       ▼
[Bước 3: Business Classes Implementation]
       │
       ▼
[Bước 4: Storage Pipe-Delimited TXT & Index Engine]
       │
       ▼
[Bước 5: C++ Web Server Infrastructure]
       │
       ▼
[Bước 6: REST API Route Handlers]
       │
       ▼
[Bước 7: Frontend Integration Layer]
```

---

## 📌 BƯỚC 1: Đọc Định nghĩa Mô hình Miền (Domain Entities & Common Constants)

### 🎯 Mục tiêu cần hiểu:
Nắm vững các struct đại diện cho các thực thể trong đồ án (Lớp, Sinh viên, Môn học, Câu hỏi, Điểm thi, Phiên thi) và namespace `StorageConfig` cấu hình kích thước bản ghi đĩa.

### 📁 Các file cần đọc:
1. `[CommonTypes.h](file:///include/CommonTypes.h)` (Đặc biệt chú ý namespace `StorageConfig`, `RECORD_SIZE_*`, `STATUS_OFFSET_*`)
2. `[Class.h](file:///include/Class.h)` (Phần `struct Lop` và `struct dsLop`)
3. `[Student.h](file:///include/Student.h)` (Phần `struct SinhVien` và `struct dsSinhVien`)
4. `[Subject.h](file:///include/Subject.h)` (Phần `struct MonHoc` và `struct NodeMH`)
5. `[Question.h](file:///include/Question.h)` (Phần `struct CauHoi` và `struct dsCHT`)
6. `[Score.h](file:///include/Score.h)` (Phần `struct DiemThi` và `struct dsDiemThi`)
7. `[Exam.h](file:///include/Exam.h)` (Phần `struct ExamSession`)

### 💡 Kiến thức cần nắm:
- Độ dài các trường dữ liệu cố định trong bản ghi pipe-delimited: `MALOP` (15), `MASV` (10), `MAMH` (15), `HO` (50), `TEN` (15), `PHAI` (4), `PASSWORD` (32), `TENLOP` (50), `TENMH` (50).
- Ký tự phân cách cột `FIELD_DELIMITER = '|'`.
- Phân loại 3 cờ trạng thái `status`: `'0'` (Active), `'1'` (Deleted), `'2'` (Soft-deleted question used in exam history).

### ✅ Điều kiện chuyển bước tiếp theo:
Giải thích được tại sao `STATUS_OFFSET_STUDENT = 132` nằm ở vị trí byte áp chót của bản ghi 134 bytes.

---

## 📌 BƯỚC 2: Đọc các Cấu trúc Dữ liệu Tự triển khai (Custom Utility Data Structures)

### 🎯 Mục tiêu cần hiểu:
Làm chủ cách triển khai các Cấu trúc dữ liệu thuần C++ (Dynamic Array, HashTable, Single LinkedList, Queue, Stack) không dùng thư viện chuẩn `std::vector` hay `std::map`.

### 📁 Các file cần đọc:
1. `[DArray.h](file:///include/DArray.h)`
2. `[HashTable.h](file:///include/HashTable.h)`
3. `[LinkedList.h](file:///include/LinkedList.h)`
4. `[Queue.h](file:///include/Queue.h)`
5. `[Stack.h](file:///include/Stack.h)`
6. `[StringNormalizer.h](file:///include/StringNormalizer.h)` / `[StringNormalizer.cpp](file:///src/StringNormalizer.cpp)`

### 💡 Kiến thức cần nắm:
- Thuật toán Dò tuyến tính (Linear Probing) và băm chuỗi/số trong `HashTable<K, V>`.
- Thuật toán tự tăng dung lượng gấp đôi (Capacity Doubling) trong `DArray<T>`.
- Các thao tác chuẩn trên Ngăn xếp `Stack` và Hàng đợi `Queue` phục vụ duyệt Cây.

### ✅ Điều kiện chuyển bước tiếp theo:
Trình bày được cơ chế giải quyết đụng độ của `HashTable<K, V>` khi băm vị trí Byte Offset.

---

## 📌 BƯỚC 3: Đọc Thuật toán Thao tác Domain Entities (Business Logic Operations)

### 🎯 Mục tiêu cần hiểu:
Nắm vững toàn bộ thuật toán Thêm, Xóa, Sửa, Tìm kiếm trên RAM đối với từng Cấu trúc dữ liệu đại diện cho 5 thực thể miền.

### 📁 Các file cần đọc:
1. `[Class.cpp](file:///src/Class.cpp)`
2. `[Student.cpp](file:///src/Student.cpp)`
3. `[Subject.cpp](file:///src/Subject.cpp)`
4. `[Question.cpp](file:///src/Question.cpp)`
5. `[Score.cpp](file:///src/Score.cpp)`
6. `[Exam.cpp](file:///src/Exam.cpp)`

### 💡 Kiến thức cần nắm:
- Thuật toán chèn giữ thứ tự TÊN+HỌ trong danh sách liên kết đơn sinh viên `Student::insert()`.
- Thuật toán chèn đệ quy và xóa nút trên Cây nhị phân tìm kiếm `Subject::insert()` và `Subject::remove()`.
- Logic Xóa cứng vs Xóa mềm câu hỏi thi trong `Question::removeNode()` và `Question::setDeleted()`.
- Thuật toán tráo ngẫu nhiên câu hỏi (Fisher-Yates Shuffle) trong `Exam.cpp`.

### ✅ Điều kiện chuyển bước tiếp theo:
Vẽ được các bước xử lý khi xóa một nút có 2 con trên Cây BST Môn học.

---

## 📌 BƯỚC 4: Đọc Cơ chế Lưu trữ Pipe-Delimited TXT & Chỉ mục IDX (Storage Engine)

### 🎯 Mục tiêu cần hiểu:
Nắm trọn vẹn giải pháp lưu trữ đĩa: Định dạng bản ghi phân cách Pipe cố định độ dài, Đọc/Ghi ngẫu nhiên $O(1)$ qua Byte Offset, Quản lý Chỉ mục nhị phân `.idx`, Compaction Engine và Báo cáo Kiểm định Khởi động.

### 📁 Các file cần đọc & Thực hành mở file:
1. Mở xem trực tiếp các tệp mẫu trong `storage/data/`: `classes.txt`, `students.txt`, `subjects.txt`, `questions.txt`. Quan sát dấu phân cách `|` và cấu trúc dòng.
2. `[PathResolver.h](file:///include/PathResolver.h)` / `[PathResolver.cpp](file:///src/PathResolver.cpp)`
3. `[StorageDeserializer.h](file:///include/StorageDeserializer.h)` / `[StorageDeserializer.cpp](file:///src/StorageDeserializer.cpp)`
4. `[IndexManager.h](file:///include/IndexManager.h)` / `[IndexManager.cpp](file:///src/IndexManager.cpp)` (Chú ý hàm `auditAndLoadIndexes()`)
5. `[StorageManager.h](file:///include/StorageManager.h)` / `[StorageManager.cpp](file:///src/StorageManager.cpp)`
6. `[Storage.cpp](file:///src/Storage.cpp)` (Chú ý hàm `PrintStartupReport()`)

### 💡 Kiến thức cần nắm:
- Công thức vị trí byte offset: $\text{Offset} = \text{HeaderLen} + K \times \text{RECORD\_SIZE}$.
- Các hàm `readStudentAt()`, `writeStudentAt()`, `appendStudent()`, `markStudentStatusAt()` sử dụng `f.seekg()` và `f.seekp()`.
- Thuật toán nén dữ liệu `compactAll()` dựa trên ngưỡng `StorageConfig` và cờ status `'1'`.
- Báo cáo kiểm định tính nhất quán dữ liệu đĩa vs RAM trong `PrintStartupReport()`.

### ✅ Điều kiện chuyển bước tiếp theo:
Giải thích được màn hình in Startup Report khi khởi động Server và chứng minh tính $O(1)$ của truy vấn byte offset.

---

## 📌 BƯỚC 5: Đọc Hạ tầng Web Server C++ (Server Architecture & Concurrency)

### 🎯 Mục tiêu cần hiểu:
Hiểu cách Web Server multithreaded dựa trên `httplib` vận hành, quản lý shared state và khóa đọc/ghi `std::shared_mutex`.

### 📁 Các file cần đọc:
1. `[ServerContext.h](file:///server/ServerContext.h)` / `[ServerContext.cpp](file:///server/ServerContext.cpp)`
2. `[ServerBootstrap.h](file:///server/ServerBootstrap.h)` / `[ServerBootstrap.cpp](file:///server/ServerBootstrap.cpp)`
3. `[RouteRegistry.h](file:///server/RouteRegistry.h)` / `[RouteRegistry.cpp](file:///server/RouteRegistry.cpp)`

### 💡 Kiến thức cần nắm:
- Các biến toàn cục `dsl`, `dsmh`, `g_dbMutex`.
- Sự khác biệt giữa `DB_READ_LOCK` (`shared_lock`) và `DB_WRITE_LOCK` (`unique_lock`).
- Cơ chế khởi động Server: Load dữ liệu RAM -> Rebuild Indexes -> Register Routes -> Listen Port 8080.
- Các hàm helper `json_response()`, `error_response()`, `findStudentGlobal()`, `find_subject_smart()`.

### ✅ Điều kiện chuyển bước tiếp theo:
Phân biệt được khi nào dùng `DB_READ_LOCK` và khi nào bắt buộc dùng `DB_WRITE_LOCK`.

---

## 📌 BƯỚC 6: Đọc các Handlers Xử lý HTTP REST Request (Server Handlers)

### 🎯 Mục tiêu cần hiểu:
Nắm rõ luồng xử lý chi tiết từ khi nhận request HTTP JSON đến khi trả về response cho từng chức năng nghiệp vụ.

### 📁 Các file cần đọc:
1. `[AuthHandler.cpp](file:///server/handlers/AuthHandler.cpp)` (Đăng nhập, Đăng xuất)
2. `[ClassHandler.cpp](file:///server/handlers/ClassHandler.cpp)` (CRUD Lớp)
3. `[StudentHandler.cpp](file:///server/handlers/StudentHandler.cpp)` (CRUD Sinh viên)
4. `[SubjectHandler.cpp](file:///server/handlers/SubjectHandler.cpp)` (CRUD Môn học)
5. `[QuestionHandler.cpp](file:///server/handlers/QuestionHandler.cpp)` (CRUD Câu hỏi)
6. `[ExamHandler.cpp](file:///server/handlers/ExamHandler.cpp)` (Bắt đầu thi, Phục hồi, Lưu tạm, Nộp bài)
7. `[ReportHandler.cpp](file:///server/handlers/ReportHandler.cpp)` (In chi tiết bài thi, In bảng điểm lớp)
8. `[AdminHandler.cpp](file:///server/handlers/AdminHandler.cpp)` (Lưu khẩn cấp, Rebuild used)

### 💡 Kiến thức cần nắm:
- Cách parse JSON body bằng `nlohmann::json`.
- Cách bọc dữ liệu envelope `{ "success": true, "data": ... }`.
- Luồng kiểm tra ràng buộc toàn vẹn trước khi gọi Core Domain Method.

### ✅ Điều kiện chuyển bước tiếp theo:
Theo dõi và giải thích trôi chảy toàn bộ luồng code của API `POST /api/exams/submit` từ Handler xuống Storage.

---

## 📌 BƯỚC 7: Đọc Tầng Tích hợp Frontend API (Frontend API Call Layer)

### 🎯 Mục tiêu cần hiểu:
Hiểu cách Frontend React TypeScript gọi API Backend, unwrap dữ liệu và bắt lỗi mạng.

### 📁 Các file cần đọc:
1. `[api.ts](file:///frontend/src/shared/api/api.ts)` (Axios Interceptors client)
2. `[ApiErrorHandler.ts](file:///frontend/src/shared/api/ApiErrorHandler.ts)`
3. `[authService.ts](file:///frontend/src/services/authService.ts)` / `[classService.ts](file:///frontend/src/services/classService.ts)` / `[examService.ts](file:///frontend/src/services/examService.ts)` / `[questionService.ts](file:///frontend/src/services/questionService.ts)` / `[reportService.ts](file:///frontend/src/services/reportService.ts)`

### 💡 Kiến thức cần nắm:
- Cấu hình Axios Base URL `http://localhost:8080`.
- Cơ chế interceptor tự động bóc tách envelope `response.data.data`.
- Cơ chế kích hoạt màn hình Reconnect khi Server rớt mạng hoặc phản hồi 502/503.

### ✅ Điều kiện HOÀN THÀNH TOÀN BỘ LỘ TRÌNH:
Bạn có thể tự tin đứng trước hội đồng vấn đáp đồ án để vẽ sơ đồ kiến trúc, giải thích bất kỳ dòng code backend C++ nào và giải thích cách dữ liệu lưu trữ từ RAM xuống đĩa TXT+IDX.

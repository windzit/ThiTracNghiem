# 08. Bảng Trách nhiệm Class & Struct (Class Responsibilities)

Tài liệu này tổng hợp và phân định rõ ràng **Trách nhiệm Đơn lẻ (Single Responsibility Principle)** của tất cả các Struct, Class C++ và Handler trong hệ thống.

---

## 1. Nhóm Thực thể Miền (Domain Entities & Data Structures)

| Class / Struct | File Header | Trách nhiệm chính | Phương thức cốt lõi |
| :--- | :--- | :--- | :--- |
| `Lop` | `[Class.h](file:///include/Class.h)` | Struct đại diện cho 1 Lớp học (Chứa MALOP, TENLOP, dssinhvien). | N/A (Plain Struct) |
| `dsLop` | `[Class.h](file:///include/Class.h)` | Struct quản lý mảng con trỏ cố định 10,000 lớp (`Lop* dslop[10000]`). | Constructor khởi tạo `nullptr` |
| `Class` | `[Class.h](file:///include/Class.h)` | Class bao đóng quản lý mảng con trỏ `dsLop`, hỗ trợ CRUD Lớp. | `insert()`, `remove()`, `update()`, `find()` |
| `SinhVien` | `[Student.h](file:///include/Student.h)` | Struct đại diện cho 1 Sinh viên (MASV, HO, TEN, PHAI, password, dsdiemthi). | N/A (Plain Struct) |
| `dsSinhVien` | `[Student.h](file:///include/Student.h)` | Struct nút của Danh sách liên kết đơn chứa Sinh viên. | Constructor nút mới |
| `Student` | `[Student.h](file:///include/Student.h)` | Class bao đóng quản lý Danh sách liên kết đơn Sinh viên thuộc 1 Lớp. | `insert()`, `remove()`, `update()`, `find()` |
| `MonHoc` | `[Subject.h](file:///include/Subject.h)` | Struct đại diện cho 1 Môn học (MAMH, TENMH, cờ used, dsCauHoi). | N/A (Plain Struct) |
| `NodeMH` | `[Subject.h](file:///include/Subject.h)` | Nút trên Cây nhị phân tìm kiếm BST chứa 1 Môn học và 2 con trỏ `left`, `right`. | Constructor nút cây |
| `Subject` | `[Subject.h](file:///include/Subject.h)` | Class bao đóng quản lý Cây nhị phân tìm kiếm BST các Môn học. | `insert()`, `remove()`, `update()`, `find()` |
| `CauHoi` | `[Question.h](file:///include/Question.h)` | Struct đại diện cho 1 Câu hỏi thi (ID, NOIDUNG, A, B, C, D, DAPAN, cờ used, deleted). | N/A (Plain Struct) |
| `dsCHT` | `[Question.h](file:///include/Question.h)` | Nút của Danh sách liên kết đơn chứa các Câu hỏi thi. | Constructor nút câu hỏi |
| `Question` | `[Question.h](file:///include/Question.h)` | Class bao đóng quản lý Danh sách liên kết đơn các câu hỏi thuộc 1 môn. | `insert()`, `removeNode()`, `setDeleted()`, `find()` |
| `DiemThi` | `[Score.h](file:///include/Score.h)` | Struct đại diện cho 1 Điểm bài thi (MAMH, DIEM). | N/A (Plain Struct) |
| `dsDiemThi` | `[Score.h](file:///include/Score.h)` | Nút của Danh sách liên kết đơn chứa điểm các môn đã thi của Sinh viên. | Constructor nút điểm |
| `Score` | `[Score.h](file:///include/Score.h)` | Class bao đóng quản lý Danh sách liên kết đơn Điểm thi của 1 Sinh viên. | `insert()`, `remove()`, `find()` |
| `ExamSession` | `[Exam.h](file:///include/Exam.h)` | Struct lưu trữ trạng thái phiên thi dang dở của 1 Sinh viên. | N/A (Session State Struct) |

---

## 2. Nhóm Quản lý Lưu trữ & Chỉ mục (Storage & Index Engine)

| Class / Component | File Header / Source | Trách nhiệm chính | Phương thức cốt lõi |
| :--- | :--- | :--- | :--- |
| `StorageConfig` | `[CommonTypes.h](file:///include/CommonTypes.h)` | Namespace chứa hằng số cấu hình lưu trữ: Pipe Delimiter `\|`, Kích thước bản ghi `RECORD_SIZE_*`, `STATUS_OFFSET_*`, Cờ status ('0','1','2'), Compaction Thresholds & Ratios. | Constant Definitions |
| `StorageManager` | `[StorageManager.h](file:///include/StorageManager.h)` | Singleton quản lý lưu vết đĩa TXT pipe-delimited fixed-length, seekg/seekp $O(1)$, Compaction Engine, Reset Backup. | `loadAllData()`, `saveAllData()`, `readStudentAt()`, `compactAll()` |
| `IndexManager` | `[IndexManager.h](file:///include/IndexManager.h)` | Singleton quản lý Bảng băm chỉ mục RAM và nạp/ghi/rebuild các tệp nhị phân `.idx`. | `auditAndLoadIndexes()`, `rebuildAllIndexes()`, `loadQuestionIndex()`, `getStudentOffset()` |
| `Storage.cpp` | `[Storage.cpp](file:///src/Storage.cpp)` | Quản lý vòng đời lưu trữ khởi động, in Báo cáo Khởi động Server (`PrintStartupReport`), đo thời gian nạp và Audit tính nhất quán tập tin đĩa. | `LoadAllData()`, `SaveAllData()`, `PrintStartupReport()` |
| `StorageDeserializer` | `[StorageDeserializer.h](file:///include/StorageDeserializer.h)` | Parse dòng text pipe-delimited fixed-length thành đối tượng C++. | `deserializeClass()`, `deserializeStudent()`, `deserializeQuestion()` |
| `StorageValidator` | `[StorageValidator.h](file:///include/StorageValidator.h)` | Kiểm tra tính hợp lệ của định dạng bản ghi dòng text. | `validateClassRecord()`, `validateStudentRecord()` |
| `StorageVerifier` | `[StorageVerifier.h](file:///include/StorageVerifier.h)` | Xác minh tính toàn vẹn giữa chỉ mục `.idx` và dữ liệu tệp `.txt`. | `verifyStudentIndex()`, `verifyQuestionIndex()` |
| `StorageIntegrityChecker` | `[StorageIntegrityChecker.h](file:///include/StorageIntegrityChecker.h)` | Kiểm tra toàn diện hệ thống tập tin đĩa trước khi khởi động. | `checkAll()` |
| `PathResolver` | `[PathResolver.h](file:///include/PathResolver.h)` | Quản lý đường dẫn tuyệt đối/tương đối tới thư mục `storage/`. | `getDataDirPath()`, `getIndexDirPath()`, `getFilePath()`, `getIndexPath()` |

---

## 3. Nhóm Web Server & Handlers

| Component / Handler | File Source | Trách nhiệm chính |
| :--- | :--- | :--- |
| `ServerContext` | `[ServerContext.h](file:///server/ServerContext.h)` | Khai báo shared state (`dsl`, `dsmh`), `shared_mutex` locking macros, JSON helpers. |
| `RouteRegistry` | `[RouteRegistry.cpp](file:///server/RouteRegistry.cpp)` | Đăng ký toàn bộ HTTP API Endpoints và cấu hình CORS. |
| `ServerBootstrap` | `[ServerBootstrap.cpp](file:///server/ServerBootstrap.cpp)` | Khởi tạo lưu trữ, load dữ liệu RAM, xây chỉ mục và mở HTTP Port 8080. |
| `AuthHandler` | `[AuthHandler.cpp](file:///server/handlers/AuthHandler.cpp)` | Xử lý API đăng nhập và đăng xuất. |
| `ClassHandler` | `[ClassHandler.cpp](file:///server/handlers/ClassHandler.cpp)` | Handlers xử lý CRUD Lớp học. |
| `StudentHandler` | `[StudentHandler.cpp](file:///server/handlers/StudentHandler.cpp)` | Handlers xử lý CRUD Sinh viên. |
| `SubjectHandler` | `[SubjectHandler.cpp](file:///server/handlers/SubjectHandler.cpp)` | Handlers xử lý CRUD Môn học trên BST. |
| `QuestionHandler` | `[QuestionHandler.cpp](file:///server/handlers/QuestionHandler.cpp)` | Handlers xử lý CRUD Câu hỏi thi (Hard/Soft delete). |
| `ExamHandler` | `[ExamHandler.cpp](file:///server/handlers/ExamHandler.cpp)` | Handlers xử lý Bắt đầu thi, Phục hồi phiên, Ghi tạm đáp án, Nộp bài chấm điểm. |
| `ReportHandler` | `[ReportHandler.cpp](file:///server/handlers/ReportHandler.cpp)` | Handlers kết xuất báo cáo bài thi chi tiết và bảng điểm lớp. |
| `AdminHandler` | `[AdminHandler.cpp](file:///server/handlers/AdminHandler.cpp)` | Handlers lưu dữ liệu khẩn cấp, rebuild cờ used, cài đặt hệ thống. |

---

## 4. Nhóm Cấu trúc Dữ liệu Phụ trợ (Utilities)

| Class Template | File Header | Trách nhiệm chính |
| :--- | :--- | :--- |
| `HashTable<K, V>` | `[HashTable.h](file:///include/HashTable.h)` | Bảng băm Open Addressing Linear Probing $O(1)$ dùng cho Index Accelerator. |
| `DArray<T>` | `[DArray.h](file:///include/DArray.h)` | Mảng động tự co giãn dung lượng tương tự `std::vector`. |
| `LinkedList<T>` | `[LinkedList.h](file:///include/LinkedList.h)` | Lớp bao đóng Danh sách liên kết đơn tổng quát. |
| `Queue<T>` | `[Queue.h](file:///include/Queue.h)` | Cấu trúc Hàng đợi (FIFO) phục vụ duyệt cây. |
| `Stack<T>` | `[Stack.h](file:///include/Stack.h)` | Cấu trúc Ngăn xếp (LIFO) phục vụ duyệt cây. |
| `StringNormalizer` | `[StringNormalizer.h](file:///include/StringNormalizer.h)` | Chuẩn hóa chuỗi (Xóa khoảng trắng thừa, viết hoa mã). |

---

## 5. Sơ đồ Mối quan hệ giữa các Thành phần

```
  [ React Client ]
         │ (HTTP REST Request)
         ▼
  [ Server Handlers ] ──(DB_READ_LOCK / DB_WRITE_LOCK)──► [ ServerContext ]
         │                                                      │
         ▼                                                      ▼
  [ Domain Classes ] ◄─────────────────────────────── [ Global dsl & dsmh ]
  (Class, Student, Subject, Question, Score)
         │
         ▼
  [ Storage Engine ] ──(Pipe-Delimited Fixed-Length)─► [ storage/data/*.txt ]
  [ Index Manager  ] ──(Binary Offsets Hash Table)───► [ storage/indexes/*.idx ]
```

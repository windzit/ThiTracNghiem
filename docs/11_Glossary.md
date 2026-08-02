# 11. Thuật ngữ & Giải thích Khái niệm (Glossary)

Tài liệu này tổng hợp và giải thích toàn bộ các **Thuật ngữ Chuyên môn, Khái niệm Kỹ thuật & Từ viết tắt** được sử dụng trong hệ thống Thi Trắc Nghiệm C++.

---

## 1. Thuật ngữ Thực thể Miền (Domain Terms)

- **`MASV` (Mã Sinh viên)**: Chuỗi định danh duy nhất của sinh viên (tối đa **10 ký tự**, viết hoa, không khoảng trắng). Mỗi bản ghi SV trong `students.txt` chiếm đúng **134 bytes**.
- **`MALOP` (Mã Lớp)**: Chuỗi định danh duy nhất của lớp học (tối đa 15 ký tự, viết hoa).
- **`MAMH` (Mã Môn học)**: Chuỗi định danh duy nhất của môn học (tối đa 15 ký tự, viết hoa), là khóa sắp xếp trên Cây BST.
- **`ID` Câu hỏi**: Số nguyên tự động tăng toàn cục (Global Auto-Increment ID) định danh duy nhất cho từng câu hỏi thi trong toàn hệ thống.
- **`used` Flag (Cờ đã sử dụng)**: Cờ bolean đánh dấu môn học hoặc câu hỏi thi đã từng xuất hiện trong bài thi của sinh viên hay chưa.
- **`deleted` Flag (Cờ xóa mềm)**: Cờ bolean đánh dấu câu hỏi thi đã bị xóa mềm (không lấy vào đề thi mới nhưng giữ cho lịch sử).
- **`ExamSession` (Phiên thi)**: Cấu trúc lưu giữ toàn bộ trạng thái bài thi đang diễn ra của sinh viên (đề thi, đáp án đã chọn, thời gian bắt đầu, số phút).

---

## 2. Thuật ngữ Cấu trúc Dữ liệu & Giải thuật (Data Structures Terms)

- **BST (Binary Search Tree - Cây nhị phân tìm kiếm)**: Cấu trúc cây trong đó nút con bên trái có khóa nhỏ hơn nút cha, nút con bên phải có khóa lớn hơn nút cha. Dùng quản lý Môn học.
- **Pointer Array (Mảng con trỏ)**: Mảng cấp phát chứa các con trỏ trỏ tới đối tượng. Dùng cho `dsLop` (mảng 10,000 con trỏ `Lop*`).
- **Singly Linked List (Danh sách liên kết đơn)**: Cấu trúc dữ liệu tuyến tính bao gồm các nút, mỗi nút chứa dữ liệu và con trỏ `next` trỏ đến nút kế tiếp. Dùng cho Sinh viên, Điểm thi, Câu hỏi thi.
- **Open Addressing (Địa chỉ mở)**: Phương pháp giải quyết đụng độ trong Bảng băm bằng cách tìm vị trí ô trống khác trong bảng.
- **Linear Probing (Dò tuyến tính)**: Thuật toán trong Địa chỉ mở, kiểm tra lần lượt các ô tiếp theo `(index + 1), (index + 2)...` khi xảy ra đụng độ băm.
- **`DArray` (Dynamic Array)**: Mảng động tự động thay đổi kích thước dung lượng (Capacity) khi tràn phần tử.

---

## 3. Thuật ngữ Lưu trữ Đĩa & Chỉ mục (Storage Engine Terms)

- **Fixed-Length Record (Bản ghi cố định độ dài)**: Định dạng tệp đĩa trong đó mọi dòng/bản ghi có số lượng bytes bằng nhau (đệm khoảng trắng). Dùng cho tệp `.txt`.
- **Byte Offset (Vị trí Byte)**: Khoảng cách tính bằng byte từ đầu tệp đến vị trí bắt đầu của một bản ghi dữ liệu. Dùng cho truy xuất trực tiếp `seekg`/`seekp` $O(1)$.
- **`.idx` (Index Accelerator File)**: Tệp chỉ mục nhị phân lưu trữ cặp `Key -> Byte Offset` để tăng tốc truy vấn đĩa.
- **`metadata.txt`**: Tệp lưu trữ các tham số hệ thống toàn cục như `LAST_QUESTION_ID`, `DELETED_*_COUNT`, `SCHEMA_VERSION=2.0`.
- **`SystemSettings.txt`**: Tệp lưu cài đặt runtime hệ thống (ví dụ: `fullscreenRequired=true`).
- **`transaction.log`**: Tệp nhật ký ghi vết thao tác hệ thống phục vụ chẩn đoán lỗi.
- **Soft Delete (Xóa mềm)**: Kỹ thuật xóa bằng cách ghi cờ `'1'` vào **byte áp chót** (`STATUS_OFFSET` = byte 132 với SV, byte 734 với câu hỏi) thay vì xóa vật lý. File vẫn giữ nguyên bản ghi — Compaction Engine sẽ dọn sạch sau.
- **Hard Delete (Xóa cứng)**: Xóa vật lý nút khỏi Cấu trúc Dữ liệu RAM và đánh dấu cờ `'1'` trên đĩa. Chỉ áp dụng cho câu hỏi `used == false`.
- **Compaction Engine**: Tiến trình quét tệp đĩa loại bỏ các bản ghi đã xóa mềm `status = '1'` và tái cấu trúc lại tệp `.txt` gọn nhẹ cùng chỉ mục `.idx`.
- **Atomic Safe Write**: Kỹ thuật ghi dữ liệu ra tệp tạm `.tmp` rồi mới tiến hành ghi đè hệ điều hành để đảm bảo an toàn tuyệt đối khi rớt điện.
- **2-Layer Validation (Kiểm định 2 Tầng)**: Mô hình bảo vệ dữ liệu gồm Tầng 1 (Form UI Frontend React) và Tầng 2 (C++ Backend Server Validator).
- **Title Case**: Quy tắc viết hoa chữ cái đầu tiên của mỗi từ (dùng cho Tên Lớp, Tên Môn, Họ Tên sinh viên).
- **Sentence Case**: Quy tắc chỉ viết hoa chữ cái đầu tiên của từ đầu tiên trong câu (dùng cho Phương án chọn A, B, C, D).

---

## 4. Thuật ngữ Server & Đa luồng (Server & Concurrency Terms)

- **`httplib`**: Thư viện C++ header-only lightweight dùng tạo HTTP Web Server và REST API.
- **Read-Write Lock (RWLock / Shared Mutex)**: Cơ chế khóa đồng bộ cho phép nhiều luồng đọc đồng thời (`DB_READ_LOCK`) nhưng chỉ 1 luồng được ghi (`DB_WRITE_LOCK`).
- **Data Race / Race Condition**: Tình trạng lỗi khi nhiều luồng cùng truy cập và thay đổi một vùng nhớ chung không được khóa đồng bộ.
- **DTO (Data Transfer Object)**: Đối tượng JSON dùng để truyền tải dữ liệu giữa Client (React) và Server (C++).
- **CORS (Cross-Origin Resource Sharing)**: Cơ chế an toàn trình duyệt cho phép Frontend từ domain/port này (`localhost:5173`) gọi API tới Server ở domain/port khác (`localhost:8080`).
- **Axios Interceptor**: Hàm chặn giữa của client HTTP cho phép tự động xử lý request/response JSON hoặc bắt lỗi mạng.
- **`StringNormalizer`**: Lớp xử lý chuẩn hóa văn bản backend C++ (xóa space thừa, toUpper mã, toTitleCase tên).
- **`StorageValidator`**: Lớp kiểm định quy tắc ký tự cấm, độ dài và phát hiện trùng đáp án backend C++.
- **`SubjectAutocomplete`**: Component React chọn môn học thông minh (tìm kiếm realtime theo mã/tên, highlight từ khóa, keyboard navigation).
- **`ClassAutocomplete`**: Component React chọn lớp học thông minh (tìm kiếm realtime theo mã/tên, giữ icon ChevronDown).


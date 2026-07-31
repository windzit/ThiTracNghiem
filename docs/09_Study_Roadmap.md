# 09. Lộ trình Học & Tự Viết lại Backend (Study Roadmap)

Tài liệu này cung cấp **Lộ trình 4 Pha phát triển** giúp sinh viên nắm vững toàn bộ tư duy thiết kế, có khả năng tự xây dựng lại toàn bộ Backend C++ từ con số 0 và bảo vệ đồ án tự tin.

---

## 1. Pha 1: Làm chủ Cấu trúc Dữ liệu Cốt lõi (Core Data Structures)

### 🎯 Mục tiêu:
Tự viết được 5 cấu trúc dữ liệu thuần bằng C++ không dùng thư viện ngoài ngoại trừ `std::string`.

### 📋 Nhiệm vụ thực hành:
1. **Mảng con trỏ `dsLop`**:
   - Khai báo mảng con trỏ `Lop* dslop[10000]`.
   - Viết các hàm: `insert(Lop*)`, `remove(MALOP)`, `update(MALOP, newTen)`, `find(MALOP)`.
   - Đảm bảo giải phóng bộ nhớ sạch sẽ khi hủy đối tượng.
2. **Danh sách liên kết đơn `dsSinhVien` và `dsDiemThi`**:
   - Viết struct nút và class bao đóng.
   - Triển khai thuật toán chèn giữ thứ tự bảng chữ cái cho sinh viên.
3. **Cây nhị phân tìm kiếm BST `Subject` (`NodeMH`)**:
   - Viết thuật toán chèn nút `insert` đệ quy theo khóa `MAMH`.
   - Viết thuật toán tìm kiếm nút `find` trên BST.
   - Viết thuật toán xóa nút trên BST xử lý đầy đủ trường hợp nút 2 con (tìm nút thế mạng).
4. **Bảng băm custom `HashTable<K, V>`**:
   - Viết bảng băm địa chỉ mở (Open Addressing) với thuật toán dò tuyến tính (Linear Probing).
   - Triển khai các phương thức `put(K, V)`, `get(K, V&)`, `remove(K)`.

---

## 2. Pha 2: Làm chủ Engine Lưu trữ TXT & Chỉ mục IDX (Storage Engine)

### 🎯 Mục tiêu:
Hiểu và tự triển khai được cơ chế Đọc/Ghi bản ghi cố định độ dài và chỉ mục nhị phân $O(1)$.

### 📋 Nhiệm vụ thực hành:
1. **Thiết kế Định dạng bản ghi Cố định**:
   - Viết hàm đệm khoảng trắng (space-padding) để biến chuỗi động thành chuỗi có độ dài cố định (`formatField`).
   - Đọc/Ghi dòng bản ghi 69 bytes (`classes.txt`), 71 bytes (`subjects.txt`), 134 bytes (`students.txt`), 736 bytes (`questions.txt`).
2. **Truy xuất Trực tiếp $O(1)$ qua Byte Offset**:
   - Sử dụng `f.seekg(offset)` và `f.seekp(offset)` để đọc/ghi đè bản ghi tại vị trí cụ thể.
   - Thực hiện chức năng đánh dấu xóa mềm `status = '1'` tại vị trí `STATUS_OFFSET` (ở byte áp chót trước `\n` của bản ghi).
3. **Quản lý Chỉ mục Nhị phân (`.idx`)**:
   - Viết hàm `saveIndex()` và `loadIndex()` ghi/đọc dữ liệu nhị phân ra tệp `.idx`.
   - Đảm bảo khi nạp từ `.idx`, Bảng băm `HashTable` trên RAM được nạp đầy đủ các cặp `Key -> offset`.
4. **Compaction Engine (Động cơ Nén)**:
   - Viết hàm quét tệp `.txt`, bỏ qua các dòng `status == '1'`, ghi bản ghi `status == '0'` sang tệp tạm và đổi tên đè lên tệp gốc. Rebuild lại chỉ mục `.idx`.

---

## 3. Pha 3: Triển khai Logic Nghiệp vụ Miền (Domain Business Rules)

### 🎯 Mục tiêu:
Ràng buộc chặt chẽ tính hợp lệ của dữ liệu trước khi thực thi các thao tác.

### 📋 Nhiệm vụ thực hành:
1. **Kiểm tra Điều kiện Xóa**:
   - Xóa Lớp: Kiểm tra danh sách sinh viên có rỗng hay không.
   - Xóa Sinh viên: Kiểm tra danh sách điểm thi có rỗng hay không.
   - Xóa Môn học: Kiểm tra cờ `used == false`.
   - Xóa Câu hỏi: Phân luồng xóa cứng (`used == false`) vs xóa mềm (`used == true`).
2. **Thi trắc nghiệm & Chấm điểm**:
   - Thuật toán tráo ngẫu nhiên câu hỏi (Fisher-Yates Shuffle).
   - Quản lý phiên thi `ExamSession` và tính toán thời gian trôi qua.
   - Chấm điểm tự động thang điểm 10: $\text{Điểm} = (\text{Số câu đúng} \times 10.0) / \text{Tổng số câu}$.

---

## 4. Pha 4: Tích hợp Web Server REST API (Server Architecture)

### 🎯 Mục tiêu:
Dùng `httplib` dựng Web Server C++ đa luồng an toàn.

### 📋 Nhiệm vụ thực hành:
1. **Khởi tạo Server & CORS**:
   - Khởi tạo `httplib::Server`, thiết lập Headers CORS cho phép React Client truy cập.
2. **Khóa Đồng bộ Đa luồng (Read-Write Lock)**:
   - Sử dụng `std::shared_mutex g_dbMutex`.
   - Áp dụng `DB_READ_LOCK` cho các hàm đọc `GET` và `DB_WRITE_LOCK` cho các hàm ghi `POST`, `PUT`, `DELETE`.
3. **Viết các Handler API**:
   - Parse JSON request body bằng `nlohmann::json`.
   - Bọc JSON response chuẩn `{ "success": true, "data": ... }`.

---

## 5. Tự Kiểm tra Tiến độ (Checklist Đánh giá)

- [ ] Bạn có thể giải thích sự khác biệt giữa mảng con trỏ `dsLop` và danh sách liên kết đơn `dsSinhVien` không?
- [ ] Bạn có thể vẽ thuật toán xóa nút trên Cây BST Môn học trên bảng không?
- [ ] Bạn có thể giải thích tại sao đọc tệp `.txt` định dạng cố định qua offset trong `.idx` lại đạt $O(1)$ không?
- [ ] Bạn có thể giải thích cơ chế RWLock làm thế nào để ngăn chặn dữ liệu bị hỏng khi 10 sinh viên nộp bài cùng một giây không?

Nếu bạn trả lời trôi chảy tất cả 4 câu hỏi trên, bạn đã sẵn sàng bảo vệ đồ án đạt điểm tối đa!

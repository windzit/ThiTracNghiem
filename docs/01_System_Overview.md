# 01. Tổng quan Hệ thống (System Overview)

Tài liệu này cung cấp cái nhìn toàn cảnh về hệ thống **Thi Trắc Nghiệm C++**, bao gồm yêu cầu đề tài gốc, kiến trúc 3 tầng tổng thể và các quy tắc nghiệp vụ (Business Rules) áp dụng xuyên suốt hệ thống.

---

## 1. Yêu cầu Đề tài Gốc

Hệ thống được thiết kế để phục vụ công tác quản lý và tổ chức thi trắc nghiệm với các thành phần dữ liệu chính:

1. **Danh sách Môn học**:
   - Cấu trúc: Cây nhị phân tìm kiếm BST (`Subject` / `NodeMH`).
   - Trường dữ liệu: `MAMH` (tối đa 15 ký tự), `TENMH` (tối đa 50 ký tự), danh sách câu hỏi thi `dsCauHoi` thuộc môn học.

2. **Danh sách Câu hỏi thi**:
   - Cấu trúc: Danh sách liên kết đơn (`Question` / `dsCHT`).
   - Trường dữ liệu: `ID` (số nguyên tự động tăng toàn cục, không trùng lặp giữa các môn), `NOIDUNG`, `A`, `B`, `C`, `D`, `DAPAN_DUNG` (`'A'`, `'B'`, `'C'`, `'D'`), cờ `used` (đã được thi hay chưa) và `deleted` (xóa mềm).

3. **Danh sách Lớp học**:
   - Cấu trúc: Mảng con trỏ cố định 10,000 phần tử (`Class` / `dsLop`).
   - Trường dữ liệu: `MALOP` (tối đa 15 ký tự), `TENLOP` (tối đa 50 ký tự), danh sách sinh viên `dssinhvien` thuộc lớp.

4. **Danh sách Sinh viên**:
   - Cấu trúc: Danh sách liên kết đơn (`Student` / `dsSinhVien`).
   - Trường dữ liệu: `MASV` (10 ký tự), `HO` (50 ký tự), `TEN` (15 ký tự), `PHAI` (`"Nam"` / `"Nu"`), `password` (mật khẩu mã hóa/plain), danh sách điểm thi `dsdiemthi`.

5. **Danh sách Điểm thi**:
   - Cấu trúc: Danh sách liên kết đơn (`Score` / `dsDiemThi`).
   - Trường dữ liệu: `MAMH` (15 ký tự), `DIEM` (số thực thang điểm 10).

---

## 2. Kiến trúc Tổng thể 3 Tầng (3-Tier Architecture)

```
[ Frontend: React + TypeScript + Vite ]
         │ (HTTP REST API / JSON)
         ▼
[ Server: C++ httplib + RouteRegistry + Handlers ]
         │ (C++ Domain Method Calls + RWLock Synchronization)
         ▼
[ Core Backend: BST, Pointer Array, LinkedList, Custom HashTable ]
         │ (Direct File Offset seekg / seekp)
         ▼
[ Storage Engine: Pipe-Delimited Fixed-Length TXT + Binary Index IDX ]
```

- **Tầng 1: Presentation (Frontend)**
  - Giao diện web hiện đại viết bằng **React + TypeScript + Vite**.
  - Gọi API backend thông qua Axios client (`[api.ts](file:///frontend/src/shared/api/api.ts)`).
  - Không truy xuất trực tiếp file hay cơ sở dữ liệu.

- **Tầng 2: Application / Server (Web Server C++)**
  - Web Server multithreaded viết bằng C++ xây dựng trên thư viện `[httplib.h](file:///include/httplib.h)`.
  - [RouteRegistry.cpp](file:///server/RouteRegistry.cpp) định nghĩa các HTTP Rest Endpoints.
  - Các Handlers (`AuthHandler`, `ClassHandler`, `StudentHandler`, `SubjectHandler`, `QuestionHandler`, `ExamHandler`, `ReportHandler`) tiếp nhận request JSON, kiểm tra quyền và gọi đến Core Business Logic.
  - Đồng bộ đa luồng an toàn bằng `std::shared_mutex` (`DB_READ_LOCK` và `DB_WRITE_LOCK`).

- **Tầng 3: Core Domain & Storage Engine (Backend & Storage)**
  - Bộ nhớ RAM: Dữ liệu được nạp lên các Cấu trúc dữ liệu C++ thuần (BST, Mảng con trỏ, DS liên kết đơn).
  - Lưu trữ đĩa: Định dạng tệp văn bản cố định độ dài phân cách dấu Pipe `|` (`.txt`) kết hợp tệp chỉ mục nhị phân (`.idx`) tăng tốc truy vấn $O(1)$ qua `[StorageManager](file:///include/StorageManager.h)` và `[IndexManager](file:///include/IndexManager.h)`.

---

## 2.1 Bí quyết Tốc độ: So sánh Khởi động Với & Không có `.idx`

Đây là điểm **độc đáo nhất** của hệ thống — giải thích tại sao Server khởi động cực nhanh dù có hàng trăm nghìn bản ghi.

```
🐢 KHÔNG CÓ .idx (Cách cổ điển):
   Server khởi động
   ├── Mở students.txt
   ├── Đọc từng ký tự từ đầu đến cuối file (O(N))
   ├── Gọi split('|'), trim(), ép kiểu... với MỖI dòng
   └── 200 SV ≈ 50ms | 20,000 SV ≈ 5,000ms | 1,000,000 SV ≈ 4 phút ❌

⚡ CÓ .idx (Cách của hệ thống này):
   Server khởi động
   ├── Đọc student.idx (nhị phân, cực nhỏ)
   ├── Nạp ngay vào HashTable RAM: "N21001"→26800, "N21002"→26934 ...
   └── 200 SV ≈ 2ms | 20,000 SV ≈ 15ms | 1,000,000 SV ≈ 300ms ✅
```

**Khi có `.idx` rồi → Mọi thao tác tra cứu về sau đều $O(1)$:**
```
Muốn lấy thông tin SV "N21001"?
  HashTable.get("N21001") → offset=26800 (nanosecond!)
  file.seekg(26800)        → Nhảy thẳng đến Byte 26800
  Đọc đúng 134 bytes       → Xong! (Không đọc bất kỳ byte nào khác)
```

---

## 3. Quy tắc Nghiệp vụ (Business Rules)

### 3.1 Quy định Khóa chính & Định dạng Dữ liệu
- **Mã Lớp (`MALOP`)**: Không được rỗng, viết hoa, không khoảng trắng, tối đa 15 ký tự. Là duy nhất trong toàn bộ hệ thống.
- **Mã Sinh viên (`MASV`)**: Không được rỗng, viết hoa, không khoảng trắng, tối đa 10 ký tự. Là duy nhất trong toàn hệ thống.
- **Mã Môn học (`MAMH`)**: Không được rỗng, viết hoa, không khoảng trắng, tối đa 15 ký tự. Duy nhất trên Cây BST Môn học.
- **ID Câu hỏi (`ID`)**: Số nguyên dương ($1, 2, 3, \dots$). Tự động tăng toàn cục (Global Auto-Increment) bởi `StorageManager::getNextQuestionID()`.

### 3.2 Quy tắc Xóa & Ràng buộc Dữ liệu (Integrity Rules)
1. **Xóa Lớp**: Chỉ được xóa lớp nếu danh sách sinh viên của lớp đó rỗng (`dssinhvien.getRoot() == nullptr`).
2. **Xóa Sinh viên**: Chỉ được xóa sinh viên nếu sinh viên đó chưa thi môn nào (`dsdiemthi.empty() == true`).
3. **Xóa Môn học**: Chỉ được xóa môn học nếu không có câu hỏi nào thuộc môn học đó đã từng được thi (`MonHoc::used == false`).
4. **Xóa Câu hỏi**:
   - Nếu câu hỏi **chưa thi** (`used == false`): Có thể xóa cứng (Physical Delete - xóa khỏi DS liên kết đơn).
   - Nếu câu hỏi **đã từng thi** (`used == true`): Không xóa cứng mà đánh dấu xóa mềm (`status = '2'`). Câu hỏi bị xóa mềm sẽ không bao giờ xuất hiện trong đề thi mới, nhưng vẫn được bảo tồn để phục vụ xem lại chi tiết bài thi lịch sử.

### 3.3 Quy tắc Thi & Chấm điểm
- Mẫu đề thi được tạo ngẫu nhiên từ ngân hàng câu hỏi của môn học tương ứng.
- Chỉ các câu hỏi hợp lệ (`status == '0'`) mới được lấy vào đề thi.
- Sinh viên làm bài thi theo phiên (`ExamSession`), đáp án được lưu tạm liên tục lên đĩa `exam_sessions.txt` để phòng chống mất dữ liệu khi mất điện hoặc rớt mạng.
- Thang điểm thi: Thang điểm 10. Điểm mỗi câu hỏi = $10.0 / N$ (với $N$ là tổng số câu hỏi trong đề thi). Điểm được làm tròn 2 chữ số thập phân.
- Mỗi sinh viên chỉ được thi 1 môn duy nhất 1 lần (nếu đã có điểm trong `dsDiemThi` thì không được thi lại môn đó).

---

## 4. Các File Mã Nguồn Liên quan

- [CommonTypes.h](file:///include/CommonTypes.h): Khai báo `StorageConfig` với kích thước bản ghi cố định pipe-delimited và kiểu dữ liệu chung.
- [Class.h](file:///include/Class.h): Định nghĩa struct `Lop`, `dsLop` và class `Class`.
- [Student.h](file:///include/Student.h): Định nghĩa struct `SinhVien`, `dsSinhVien` và class `Student`.
- [Subject.h](file:///include/Subject.h): Định nghĩa struct `MonHoc`, `NodeMH` và class `Subject`.
- [Question.h](file:///include/Question.h): Định nghĩa struct `CauHoi`, `dsCHT` và class `Question`.
- [Score.h](file:///include/Score.h): Định nghĩa struct `DiemThi`, `dsDiemThi` và class `Score`.

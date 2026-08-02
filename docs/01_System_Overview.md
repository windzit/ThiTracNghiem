# 01. Tổng quan Hệ thống (System Overview)

Tài liệu này cung cấp cái nhìn toàn cảnh về hệ thống **Thi Trắc Nghiệm C++**, bao gồm yêu cầu đề tài gốc, kiến trúc 3 tầng tổng thể, cơ chế kiểm định dữ liệu 2 tầng (2-Layer Validation & Normalization) và các quy tắc nghiệp vụ (Business Rules) áp dụng xuyên suốt hệ thống.

---

## 💡 Phép so sánh Trực quan: Hệ thống Giống như "Sân bay Quốc tế"

Để người không chuyên về CNTT cũng có thể hình dung ngay:

```
✈️ SÂN BAY QUỐC TẾ                      🖥️ HỆ THỐNG THI TRẮC NGHIỆM
┌────────────────────────────────┐     ┌────────────────────────────────┐
│ 1. Cổng An ninh Ban đầu        │ ──► │ 1. Frontend (React + TS)       │
│    (Kiểm tra vé, xóa khoảng    │     │    (Xóa space thừa, sửa viết   │
│     trắng, tự viết hoa mã)     │     │     hoa UI, báo lỗi ngay lập tức)│
├────────────────────────────────┤     ├────────────────────────────────┤
│ 2. Hải quan & Máy quét Hải quan│ ──► │ 2. Backend Server (C++)        │
│    (Kiểm tra hộ chiếu trùng,   │     │    (Chuẩn hóa tuyệt đối, tra    │
│     lưu hồ sơ vào máy chủ)     │     │     HashTable trùng lặp O(1))   │
├────────────────────────────────┤     ├────────────────────────────────┤
│ 3. Kho Lưu trữ Hồ sơ          │ ──► │ 3. Storage TXT + IDX           │
│    (Ghi sổ định dạng cố định,  │     │    (Bản ghi cố định độ dài,    │
│     có mục lục tra cứu nhanh)  │     │     truy xuất byte offset O(1)) │
└────────────────────────────────┘     └────────────────────────────────┘
```

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

## 2. Kiến trúc Tổng thể 3 Tầng & Kiểm soát 2 Tầng (2-Layer Security)

```
[ TẦNG 1: Frontend - React + TypeScript + Vite ]
   ├── Client-Side Form Normalization (toTitleCase, normalizeIdentifier...)
   └── Client-Side Form Validation (formValidation.ts - phản hồi tức thì UI)
         │ (HTTP REST API / JSON Envelope)
         ▼
[ TẦNG 2: Web Server C++ - httplib + RouteRegistry + Handlers ]
   ├── StringNormalizer (Chuẩn hóa C++ backend trước khi xử lý)
   ├── StorageValidator (Kiểm tra ký tự cấm, độ dài, trùng đáp án)
   └── DB_READ_LOCK / DB_WRITE_LOCK (Đồng bộ đa luồng safe)
         │ (Direct File Offset seekg / seekp O(1))
         ▼
[ TẦNG 3: Core Domain & Storage Engine ]
   ├── RAM: BST, Mảng con trỏ, DS liên kết, Open Addressing HashTable
   └── DISK: Pipe-Delimited Fixed-Length TXT + Binary Index IDX
```

---

## 3. Quy tắc Kiểm định & Chuẩn hóa Dữ liệu (Validation & Normalization Rules)

Hệ thống áp dụng nghiêm ngặt **5 nhóm quy tắc** tại cả 2 tầng Frontend và Backend:

### 3.1 Nhóm 1: MÃ (Mã Lớp `MALOP`, Mã Môn `MAMH`, Mã Sinh viên `MASV`)
- **Ký tự cho phép**: Chỉ gồm chữ cái (`A-Z`, `a-z`), chữ số (`0-9`) và dấu gạch ngang (`-`). Không cho phép khoảng trắng hay ký tự đặc biệt khác.
- **Tự động chuẩn hóa**: Tự động xóa sạch khoảng trắng (ở đầu, cuối, ở giữa) và chuyển toàn bộ thành **IN HOA** (Ví dụ: `" d 22-cq cn01 "` → `"D22-CQCN01"`).
- **Ràng buộc duy nhất**: Bắt buộc tra cứu Bảng băm / Cây BST để đảm bảo không trùng lặp trước khi tạo hoặc cập nhật.

### 3.2 Nhóm 2: TÊN MÔN HỌC / TÊN LỚP
- **Ký tự cho phép**: Chữ cái (bao gồm cả tiếng Việt có dấu), chữ số, khoảng trắng, dấu `-`, `_`, `(`, `)`.
- **Tự động chuẩn hóa (Title Case)**: Xóa khoảng trắng đầu/cuối, gộp nhiều khoảng trắng liên tiếp thành 1 khoảng trắng duy nhất, viết hoa chữ cái đầu tiên của **mỗi từ** (Ví dụ: `"cOng   nGhE  thOng   tin"` → `"Cong Nghe Thong Tin"`).
- **So sánh trùng lặp**: Backend thực hiện kiểm tra trùng bằng chuỗi chuẩn hóa (trim + collapse space + lowercase).

### 3.3 Nhóm 3: TÊN SINH VIÊN (`HO` & `TEN`)
- **Ký tự cho phép**: Chỉ cho phép chữ cái tiếng Việt và khoảng trắng. Không chứa chữ số hay ký tự đặc biệt.
- **Tự động chuẩn hóa (Title Case)**: Tương tự nhóm 2, viết hoa chữ cái đầu mỗi từ (Ví dụ: `"ngUyEn   vaN   aN"` → `"Nguyen Van An"`).

### 3.4 Nhóm 4: CÂU HỎI THI & PHƯƠNG ÁN CHỌN
- **Nội dung câu hỏi (`NOIDUNG`)**:
  - Cho phép tất cả ký tự văn bản thông thường (trừ cấm dấu pipe `|`, tab `\t`, xuống dòng `\r`, `\n`).
  - Xóa khoảng trắng thừa (trim + collapse spaces).
  - **GIỮ NGUYÊN nội dung và viết hoa/thường nguyên bản của người dùng** (KHÔNG tự ý viết hoa chữ cái đầu hay ép kiểu).
- **Phương án chọn (`A`, `B`, `C`, `D`)**:
  - Xóa khoảng trắng thừa (trim + collapse spaces).
  - **Sentence Case**: Chỉ tự động viết hoa chữ cái đầu tiên của từ đầu tiên trong câu (Ví dụ: `"phuong an   chuc   nang"` → `"Phuong an chuc nang"`).
- **Kiểm tra trùng đáp án**: Backend (`StorageValidator::hasDuplicateOptionsAfterNormalization`) tự động chuyển cả 4 đáp án về chữ thường để so sánh, phát hiện và từ chối nếu có 2 đáp án trùng nội dung nhau.

### 3.5 Nhóm 5: MẬT KHẨU (`PASSWORD`)
- **Giữ nguyên 100%**: Tuyệt đối **KHÔNG trim**, KHÔNG viết hoa hay biến đổi ký tự của mật khẩu người dùng nhập.

---

## 4. Các Tệp Quản lý Hệ thống & Metadata Đĩa

Ngoài các tệp dữ liệu thực thể chính (`classes.txt`, `students.txt`, `subjects.txt`, `questions.txt`, `scores.txt`), hệ thống còn tự động duy trì 3 tệp quản lý đặc biệt:

| Tệp Hệ thống | Đường dẫn | Trách nhiệm |
| :--- | :--- | :--- |
| **`metadata.txt`** | `storage/data/metadata.txt` | Lưu trạng thái toàn cục: `LAST_QUESTION_ID` (ID câu hỏi lớn nhất để cấp phát tự động không trùng), bộ đếm số bản ghi đã xóa mềm `DELETED_*_COUNT` để kích hoạt Compaction, và `SCHEMA_VERSION=2.0`. Đồng bộ tức thì xuống đĩa ngay khi khởi động hoặc thêm câu hỏi. |
| **`SystemSettings.txt`** | `storage/data/SystemSettings.txt` | Lưu các cài đặt hệ thống runtime (ví dụ: cờ `fullscreenRequired` bắt buộc sinh viên thi toàn màn hình). |
| **`transaction.log`** | `storage/data/transaction.log` | Nhật ký ghi vết các thao tác quan trọng để phục vụ kiểm tra và chẩn đoán lỗi. |

---

## 5. Quy tắc Nghiệp vụ Ràng buộc (Integrity Rules)

1. **Xóa Lớp**: Chỉ được xóa lớp nếu danh sách sinh viên của lớp đó rỗng (`dssinhvien.getRoot() == nullptr`).
2. **Xóa Sinh viên**: Chỉ được xóa sinh viên nếu sinh viên đó chưa thi môn nào (`dsdiemthi.empty() == true`).
3. **Xóa Môn học**: Chỉ được xóa môn học nếu không có câu hỏi nào thuộc môn học đó đã từng được thi (`MonHoc::used == false`).
4. **Xóa Câu hỏi**:
   - Nếu câu hỏi **chưa thi** (`used == false`): Xóa cứng (Physical Delete - gỡ khỏi DS liên kết RAM, ghi cờ `'1'` trên đĩa).
   - Nếu câu hỏi **đã từng thi** (`used == true`): Xóa mềm (Soft Delete - giữ nút RAM, ghi cờ `'2'` trên đĩa). Câu hỏi xóa mềm bị ẩn khỏi đề thi mới nhưng vẫn phục vụ xem lại lịch sử làm bài.

---

## 6. Các File Mã Nguồn Liên quan

- [formValidation.ts](file:///frontend/src/shared/lib/formValidation.ts): Bộ quy tắc Validate và Normalization phía Frontend React.
- [StringNormalizer.h](file:///include/StringNormalizer.h) / [.cpp](file:///src/StringNormalizer.cpp): Bộ chuẩn hóa chuỗi phía C++ Backend.
- [StorageValidator.h](file:///include/StorageValidator.h) / [.cpp](file:///src/StorageValidator.cpp): Bộ kiểm tra ký tự cấm và tính hợp lệ dữ liệu Backend.
- [CommonTypes.h](file:///include/CommonTypes.h): Khai báo `StorageConfig` với kích thước bản ghi cố định pipe-delimited và cờ trạng thái.
- [Class.h](file:///include/Class.h): Struct `Lop`, `dsLop` và class `Class`.
- [Student.h](file:///include/Student.h): Struct `SinhVien`, `dsSinhVien` và class `Student`.
- [Subject.h](file:///include/Subject.h): Struct `MonHoc`, `NodeMH` và class `Subject`.
- [Question.h](file:///include/Question.h): Struct `CauHoi`, `dsCHT` và class `Question`.
- [Score.h](file:///include/Score.h): Struct `DiemThi`, `dsDiemThi` và class `Score`.


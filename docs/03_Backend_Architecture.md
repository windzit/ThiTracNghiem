# 03. Kiến trúc Core Backend & Cấu trúc Dữ liệu

Tài liệu này chi tiết hóa toàn bộ các **Cấu trúc Dữ liệu (Data Structures)** tự định nghĩa trong C++ theo đúng yêu cầu đồ án, cách quản lý bộ nhớ con trỏ, tính toán độ phức tạp thuật toán và giải thuật thao tác.

---

## 🧠 1. Tại sao cần HIỂU Cấu trúc Dữ liệu?

```
🏗️ Phép so sánh: Tổ chức File trong Văn phòng

Lớp học  = Tủ hồ sơ (Mảng con trỏ) → Mỗi ngăn chứa 1 lớp
Sinh viên = Xếp theo bảng chữ cái trong ngăn (Danh sách liên kết đơn)
Môn học   = Sổ tra cứu chữ cái A-Z (Cây nhị phân BST)
Câu hỏi   = Tờ phiếu câu hỏi kẹp trong sổ môn học (DS liên kết đơn)
Điểm thi  = Phiếu điểm kẹp vào hồ sơ sinh viên (DS liên kết đơn)
```

---

## 🗃️ 2. Sơ đồ Tổng quan Cấu trúc Dữ liệu RAM

```
Class dsl (Toàn hệ thống)
│
└── dsLop [Mảng con trỏ - tối đa 10,000 phần tử]
    ├── [0] → Lop { MALOP="D21CQCN01", TENLOP="Lớp CN01" }
    │           └── dsSinhVien [Danh sách liên kết đơn - sắp xếp A-Z theo TÊN+HỌ]
    │               ├── SinhVien { MASV="N21001", HO="Le", TEN="An" }
    │               │   └── dsDiemThi [DS liên kết đơn]
    │               │       ├── DiemThi { MAMH="CTDL", DIEM=8.5 }
    │               │       └── DiemThi { MAMH="OOP", DIEM=7.0 }
    │               └── SinhVien { MASV="N21002", HO="Nguyen", TEN="Binh" }
    ├── [1] → Lop { MALOP="D21CQCN02", ... }
    └── [nullptr] × 9,998

Subject dsmh (Toàn hệ thống)
│
└── NodeMH [Cây nhị phân tìm kiếm BST - sắp xếp theo MAMH]
           CTDL
          /    \
        AI      OOP
       /  \    /   \
     ...  ... ...  ...
    └── Mỗi NodeMH chứa dsCauHoi [DS liên kết đơn câu hỏi]
        ├── CauHoi { ID=101, NOIDUNG="BST là gì?", ..., used=false, deleted=false }
        └── CauHoi { ID=102, NOIDUNG="Heap là gì?", ..., used=true,  deleted=false }
```

---

## 📊 3. Chi tiết Triển khai Từng Cấu trúc Dữ liệu

### 3.1 Mảng Con trỏ `dsLop` — Danh sách Lớp học

**Tệp**: `include/Class.h`, `src/Class.cpp`

```cpp
struct dsLop {
    int n;               // Số lớp đang có (hiện tại n <= 10,000)
    Lop* dslop[10000];  // Mảng 10,000 con trỏ, khởi tạo = nullptr
};
```

**Tại sao dùng Mảng Con trỏ?**
```
✅ Ưu điểm:
- Khi chưa tạo lớp: Mỗi phần tử chỉ chiếm 8 bytes (con trỏ nullptr)
- Thêm lớp mới: O(1) — chỉ gán dslop[n++] = new Lop()
- Truy cập theo chỉ số: dslop[i] → O(1)

❌ Nhược điểm:
- Tìm theo MALOP: O(N) — phải duyệt toàn bộ
- Xóa 1 lớp: O(N) — phải dịch mảng lên để lấp chỗ trống
```

| Thao tác | Độ phức tạp |
| :--- | :--- |
| Thêm lớp | $O(1)$ |
| Tìm lớp theo MALOP | $O(N)$ |
| Xóa lớp | $O(N)$ |

### 3.2 Danh sách Liên kết Đơn `dsSinhVien` — Sinh viên thuộc Lớp

**Tệp**: `include/Student.h`, `src/Student.cpp`

```cpp
struct dsSinhVien {
    SinhVien sinhvien;   // Dữ liệu: MASV, HO, TEN, PHAI, password, dsdiemthi
    dsSinhVien* next;    // Con trỏ đến sinh viên kế tiếp trong danh sách
};
```

**Đặc điểm quan trọng — Chèn theo thứ tự bảng chữ cái:**
```
Danh sách hiện tại: An → Bình → Dũng → Minh
Chèn "Cuong":
  ├── So sánh với An: "Cuong" > "An" → tiếp tục
  ├── So sánh với Bình: "Cuong" > "Binh" → tiếp tục
  ├── So sánh với Dũng: "Cuong" < "Dung" → chèn vào giữa Bình và Dũng
  └── Kết quả: An → Bình → Cuong → Dũng → Minh
```

### 3.3 Danh sách Liên kết Đơn `dsDiemThi` — Bảng Điểm Sinh viên

**Tệp**: `include/Score.h`, `src/Score.cpp`

```cpp
struct dsDiemThi {
    DiemThi diemthi;    // MAMH, DIEM
    dsDiemThi* next;    // Con trỏ đến điểm môn kế tiếp
};
```

**Vai trò quan trọng:**
- `dsdiemthi.empty()` → Kiểm tra sinh viên có thể bị xóa không
- Tìm `MAMH` trong `dsdiemthi` → Kiểm tra sinh viên đã thi môn này chưa
- Mỗi khi nộp bài → Chèn `DiemThi` mới vào đầu danh sách

### 3.4 Cây Nhị phân Tìm kiếm BST `NodeMH` — Danh sách Môn học

**Tệp**: `include/Subject.h`, `src/Subject.cpp`

```cpp
struct NodeMH {
    MonHoc data;     // MAMH, TENMH, used, dsCauHoi
    NodeMH* left;    // Nhánh trái: MAMH < node này
    NodeMH* right;   // Nhánh phải: MAMH > node này
};
```

**Minh họa BST với các môn học:**
```
         CTDL
        /    \
      AI      OOP
     /  \    /   \
  NULL NULL INT   PLT

Tìm "OOP": CTDL → (OOP > CTDL) → phải → OOP ✓  (Chỉ 2 bước!)
Tìm "INT": CTDL → phải → OOP → (INT < OOP) → trái → INT ✓  (3 bước)
```

**Thuật toán Xóa nút BST (3 trường hợp):**
```
Xóa "CTDL" khỏi cây trên:
┌─────────────────────────────────────────────────────┐
│ CTDL có 2 con (AI và OOP)                           │
│ → Tìm "In-order Successor" (nút nhỏ nhất bên phải) │
│ → Successor = "INT" (trái nhất của cây con bên phải)│
│ → Copy data "INT" vào nút "CTDL"                   │
│ → Xóa nút "INT" cũ (nút này chắc chắn 0 hoặc 1 con)│
└─────────────────────────────────────────────────────┘
Kết quả:     INT
            /    \
          AI      OOP
                 /   \
              NULL   PLT
```

| Thao tác | Trung bình | Xấu nhất |
| :--- | :--- | :--- |
| Tìm kiếm | $O(\log N)$ | $O(N)$ (cây lệch) |
| Thêm mới | $O(\log N)$ | $O(N)$ |
| Xóa nút | $O(\log N)$ | $O(N)$ |

### 3.5 Danh sách Liên kết Đơn `dsCHT` — Ngân hàng Câu hỏi Thi

**Tệp**: `include/Question.h`, `src/Question.cpp`

```cpp
struct CauHoi {
    int ID;                  // ID duy nhất toàn hệ thống (auto-increment)
    std::string NOIDUNG;     // Nội dung câu hỏi
    std::string A, B, C, D; // 4 phương án lựa chọn
    char DAPAN_DUNG;         // Đáp án đúng: 'A', 'B', 'C', hoặc 'D'
    bool used = false;       // true = đã từng được thi
    bool deleted = false;    // true = đã bị xóa mềm
};
```

**Logic Hard Delete vs Soft Delete:**
```
Xóa câu hỏi ID=105:
├── used == false (Chưa thi bao giờ)?
│     → Hard Delete: Hủy nút dsCHT khỏi RAM + Xóa khỏi question.idx
│                    markQuestionStatusAt(offset, '1')  ← đĩa
│                    incrementDeletedCount("question")
│
└── used == true (Đã từng thi)?
      → Soft Delete: Gán cauhoi.deleted = true  ← RAM vẫn GIỮ nút!
                     markQuestionStatusAt(offset, '2')  ← đĩa: '2' không phải '1'!
                     Câu hỏi vẫn tồn tại cho: Xem lại lịch sử bài thi
                     Câu hỏi KHÔNG xuất hiện: Trong đề thi mới (lọc deleted==true)
```

---

## 🔧 4. Cấu trúc Dữ liệu & Utility Phụ trợ

### 4.1 Bảng Băm `HashTable<K, V>` — Index Accelerator

**Tệp**: `include/HashTable.h`

**Phương pháp**: Open Addressing + Linear Probing

```
Lưu: ("N21001" → offset 26800)
  Hash("N21001") = 42         → Slot 42 còn trống? → Lưu vào Slot 42

Lưu: ("N21002" → offset 26934) → Slot 43 còn trống? → Lưu vào Slot 43

Tra cứu: get("N21001")
  Hash("N21001") = 42         → Slot 42 có Key "N21001"? ✓ → Return 26800

Xử lý đụng độ (Collision):
  Hash("N21003") = 42         → Slot 42 đã có N21001 → Kiểm tra Slot 43 → 44 → ...
```

### 4.2 Mảng Động `DArray<T>`

**Tệp**: `include/DArray.h`

- Tương tự `std::vector` nhưng tự triển khai thuần C++.
- Tự động tăng dung lượng gấp đôi khi tràn (Capacity Doubling).
- Dùng cho: Danh sách ID câu hỏi trong `ExamSession`, Danh sách offset lịch sử thi.

### 4.3 Chuẩn hóa Chuỗi C++ `StringNormalizer` & Kiểm định `StorageValidator`

**Tệp**: `include/StringNormalizer.h`, `include/StorageValidator.h`

- **`StringNormalizer`**:
  - `normalizeIdentifier()`: Xóa khoảng trắng, biến mã thành IN HOA (`" d22-cqcn01 "` → `"D22-CQCN01"`).
  - `toTitleCase()`: Viết hoa chữ cái đầu mỗi từ cho tên Lớp, tên Môn, Họ và Tên sinh viên (`"cOng  nGhE"` → `"Cong Nghe"`).
  - `normalizeHumanText()`: Trim và collapse space, giữ nguyên newline/tabs nguyên bản để validator phát hiện.
- **`StorageValidator`**:
  - Kiểm tra các ký tự cấm: dấu pipe `|`, tab `\t`, xuống dòng `\r`, `\n`, control chars.
  - `hasDuplicateOptionsAfterNormalization()`: Chuyển 4 phương án câu hỏi về chữ thường (`std::tolower`) để phát hiện và ngăn chặn trùng lặp đáp án.

---

## 📈 5. Bảng Tóm tắt Độ phức tạp Thuật toán

| Thực thể | Cấu trúc RAM | Thêm | Tìm kiếm | Xóa |
| :--- | :--- | :--- | :--- | :--- |
| **Lớp học** | Mảng con trỏ | $O(1)$ | $O(N)$ | $O(N)$ |
| **Sinh viên** | DS liên kết đơn | $O(K)$ (giữ thứ tự) | $O(K)$ | $O(K)$ |
| **Môn học** | Cây BST | $O(\log M)$ | $O(\log M)$ | $O(\log M)$ |
| **Câu hỏi** | DS liên kết đơn | $O(1)$ (chèn đầu) | $O(Q)$ | $O(Q)$ |
| **Chỉ mục IDX** | Bảng Băm (HashTable) | $O(1)$ TB | $O(1)$ TB | $O(1)$ TB |

> $K$ = số SV trong lớp đó, $M$ = số môn học, $Q$ = số câu hỏi trong môn đó

---

## 💾 6. Bảng Định dạng Lưu trữ Đĩa (Pipe-Delimited Fixed-Length)

RAM → Đĩa theo định dạng `FIELD1(n bytes) | FIELD2(m bytes) | ... | STATUS(1) \n`:

| Thực thể | `RECORD_SIZE` | `STATUS_OFFSET` | Cấu trúc cột (bytes) |
| :--- | :--- | :--- | :--- |
| **Lớp** | **69** | **Byte 67** | `MALOP`(15) \| `TENLOP`(50) \| `S`(1) |
| **Sinh viên** | **134** | **Byte 132** | `MALOP`(15) \| `MASV`(10) \| `HO`(50) \| `TEN`(15) \| `PHAI`(4) \| `PASSWORD`(32) \| `S`(1) |
| **Môn học** | **71** | **Byte 69** | `MAMH`(15) \| `TENMH`(50) \| `U`(1) \| `S`(1) |
| **Câu hỏi** | **736** | **Byte 734** | `MAMH`(15) \| `ID`(10) \| `NOIDUNG`(300) \| `A`(100) \| `B`(100) \| `C`(100) \| `D`(100) \| `DAPAN`(1) \| `S`(1) |

---

## 📁 7. Các File Mã Nguồn Liên quan

| File | Nhiệm vụ |
| :--- | :--- |
| [CommonTypes.h](file:///include/CommonTypes.h) | `StorageConfig`, `RECORD_SIZE_*`, `STATUS_OFFSET_*` |
| [Class.h](file:///include/Class.h) / [.cpp](file:///src/Class.cpp) | Struct `Lop`, `dsLop`, Class `Class` |
| [Student.h](file:///include/Student.h) / [.cpp](file:///src/Student.cpp) | Struct `SinhVien`, `dsSinhVien`, Class `Student` |
| [Subject.h](file:///include/Subject.h) / [.cpp](file:///src/Subject.cpp) | Struct `MonHoc`, `NodeMH`, Class `Subject` (BST) |
| [Question.h](file:///include/Question.h) / [.cpp](file:///src/Question.cpp) | Struct `CauHoi`, `dsCHT`, Class `Question` |
| [Score.h](file:///include/Score.h) / [.cpp](file:///src/Score.cpp) | Struct `DiemThi`, `dsDiemThi`, Class `Score` |
| [HashTable.h](file:///include/HashTable.h) | Bảng Băm Open Addressing tự triển khai |
| [DArray.h](file:///include/DArray.h) | Mảng động tự triển khai |
| [StringNormalizer.h](file:///include/StringNormalizer.h) / [.cpp](file:///src/StringNormalizer.cpp) | Bộ chuẩn hóa chuỗi Backend C++ |
| [StorageValidator.h](file:///include/StorageValidator.h) / [.cpp](file:///src/StorageValidator.cpp) | Bộ kiểm định dữ liệu Backend C++ |


# Chi Tiết Kiến Trúc Backend C++ (ThiTracNghiem Backend)

Tài liệu chi tiết về thiết kế hướng đối tượng (OOP), cấu trúc dữ liệu giải thuật, mô hình bất đồng bộ/đồng thời, và các submodule của Backend C++.

---

## 1. Cấu Trúc Dữ Liệu Nâng Cao (Core Data Structures)

Backend sử dụng các cấu trúc dữ liệu tự định nghĩa tối ưu bộ nhớ và tốc độ truy vấn:

### A. Môn Học (Subject) — Cây Tìm Kiếm Nhị Phân (Binary Search Tree - BST)
- **Cấu trúc nút (`NodeMH`)**:
  ```cpp
  struct MonHoc {
      char MAMH[15];
      std::string TENMH;
      bool used = false;
      Question dsCauHoi; // Danh sách câu hỏi thuộc môn này
  };

  struct NodeMH {
      MonHoc data;
      NodeMH* left;
      NodeMH* right;
  };
  ```
- **Đặc tính**: Các nút được sắp xếp theo khóa `MAMH` (mã môn học). Tốc độ tìm kiếm môn học đạt **O(log N)**.
- **Giải thuật**: Thêm nút (`_insert`), xóa nút (`_remove` hỗ trợ thay thế nút con phải nhỏ nhất khi nút có 2 con), duyệt cây in-order để giải phóng bộ nhớ (`clear`).

### B. Danh Sách Câu Hỏi (Question) — Danh Sách Liên Kết Đơn + Auto-Increment
- **Cấu trúc nút (`dsCHT`)**:
  ```cpp
  struct CauHoi {
      int ID;
      std::string NOIDUNG, A, B, C, D;
      char DAPAN_DUNG; // 'A' | 'B' | 'C' | 'D'
      bool used = false;    // Đã được sử dụng trong bài thi hay chưa
      bool deleted = false; // Đã bị xóa mềm hay chưa
  };

  struct dsCHT {
      dsCHT* next;
      CauHoi cauhoi;
  };
  ```
- **Cơ chế Xóa Lai (Hybrid Delete)**:
  - Nếu `used == false`: Xóa cứng (`removeNode`), giải phóng nút khỏi bộ nhớ.
  - Nếu `used == true`: Xóa mềm (`setDeleted`), đánh dấu `deleted = true` để bảo toàn lịch sử bài thi đã nộp của sinh viên.
- **Tự Động Sinh ID (Auto-Increment)**: Tự động tìm ID lớn nhất hiện có và tăng thêm 1 khi tạo câu hỏi mới.

### C. Lớp Học (Class) — Mảng Động Con Trỏ Con Cháu (`dsLop`)
- **Cấu trúc**:
  ```cpp
  struct Lop {
      std::string MALOP;
      std::string TENLOP;
      Student dssinhvien; // Danh sách sinh viên thuộc lớp
  };

  struct dsLop {
      int n;
      Lop* dslop[MAXLOP];
  };
  ```
- **Đặc tính**: Mảng con trỏ quản lý tối đa `MAXLOP` lớp học, hỗ trợ chèn, sửa tên lớp và kiểm tra sĩ số.

### D. Sinh Viên (Student) — Danh Sách Liên Kết Đơn
- **Cấu trúc (`dsSinhVien`)**: Con trỏ đầu `root` trỏ tới chuỗi các nút `SinhVien`. Mỗi sinh viên chứa một danh sách điểm thi `dsdiemthi`.

### E. Điểm Thi (Score) — Danh Sách Liên Kết Đơn
- **Cấu trúc (`dsDiemThi`)**: Chứa mã môn học `MAMH` và điểm số `DIEM` (thang điểm 10.0, làm tròn 2 chữ số thập phân).

---

## 2. Các Submodule Server (Modular Server Breakdown)

Thư mục `server/` bao gồm các thành phần được tách bạch trách nhiệm:

```
server/
├── ServerContext.h / .cpp    # Quản lý biến toàn cục (dsl, dsmh, mutex) & response helpers
├── ServerBootstrap.h / .cpp  # Điểm khởi tạo, CLI flags, startup timing, svr.listen
├── RouteRegistry.h / .cpp    # Khai báo và mapping toàn bộ 30+ HTTP REST endpoints
├── Diagnostics.h / .cpp      # Bộ 20 test kiểm tra tính toàn vẹn dữ liệu tự động
└── handlers/                 # Các REST Controller theo miền nghiệp vụ
    ├── AuthHandler.cpp       # Dang nhap, dang xuat
    ├── ClassHandler.cpp      # Lay, tao, sua, xoa lop hoc
    ├── StudentHandler.cpp    # Lay, tao, sua, xoa, xoa hang loat sinh vien
    ├── SubjectHandler.cpp    # Lay, tao, sua, xoa mon hoc
    ├── QuestionHandler.cpp   # Tao, sua, xoa, xoa hang loat cau hoi
    ├── ExamHandler.cpp       # Bat dau, tiep tuc, luu dap an, nop bai thi
    ├── ReportHandler.cpp     # Xem chi tiet bai thi, bang diem lop, xoa diem
    └── AdminHandler.cpp      # Luu du lieu, cap nhat cơ che giam sat, rebuild used flags
```

---

## 3. Cơ Chế Quản Lý Đồng Thời & Thread-Safety

Toàn bộ ứng dụng backend được bảo mật đọc/ghi đồng thời thông qua mutex toàn cục:

```cpp
// Thư viện C++17 std::shared_mutex
extern std::shared_mutex g_dbMutex;

// Lock Macros
#define DB_READ_LOCK  std::shared_lock<std::shared_mutex> _rl(g_dbMutex)
#define DB_WRITE_LOCK std::unique_lock<std::shared_mutex> _wl(g_dbMutex)
```

- Các yêu cầu đọc dữ liệu (`GET /api/classes`, `GET /api/subjects`, ...) sử dụng `DB_READ_LOCK`, cho phép hàng trăm sinh viên đọc dữ liệu đồng thời mà không bị block.
- Các yêu cầu ghi dữ liệu (`POST`, `PUT`, `DELETE`) sử dụng `DB_WRITE_LOCK`, chặn tất cả thao tác đọc/ghi khác trong khoảng thời gian cực ngắn để đảm bảo tính nhất quán của cấu trúc cây/danh sách.

<div align="center">

# 🎓 PTIT Computer-Based Testing System (CBT)
### Hệ Thống Quản Lý & Thi Trắc Nghiệm Trực Tuyến Tối Ưu CTDL&GT

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![React](https://img.shields.io/badge/React-19.2-61DAFB?style=for-the-badge&logo=react&logoColor=black)](https://react.dev/)
[![TypeScript](https://img.shields.io/badge/TypeScript-5.x-3178C6?style=for-the-badge&logo=typescript&logoColor=white)](https://www.typescriptlang.org/)
[![Vite](https://img.shields.io/badge/Vite-8.x-646CFF?style=for-the-badge&logo=vite&logoColor=white)](https://vitejs.dev/)
[![TailwindCSS](https://img.shields.io/badge/Tailwind_CSS-v4-38B2AC?style=for-the-badge&logo=tailwind-css&logoColor=white)](https://tailwindcss.com/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)](https://cmake.org/)
[![PTITHCM](https://img.shields.io/badge/PTIT-HCM-E30019?style=for-the-badge)](https://ptithcm.edu.vn/)

<p align="center">
  <b>Hệ thống quản lý và tổ chức thi trắc nghiệm học đường đa nền tảng (Web SPA + High-Performance C++ REST API Server + Console Terminal CLI).</b><br>
  Được xây dựng với <b>100% Cấu trúc Dữ liệu & Giải thuật thuần (Zero STL Containers / Algorithms)</b> kết hợp kiến trúc Full-stack hiện đại.
</p>

[Tính Năng Chính](#-tính-năng-nổi-bật) •
[Cấu Trúc Dữ Liệu](#-cấu-trúc-dữ-liệu--thuật-toán-cốt-lõi) •
[Kiến Trúc Hệ Thống](#-kiến-trúc-hệ-thống) •
[Cài Đặt & Khởi Chạy](#-hướng-dẫn-cài-đặt--khởi-chạy) •
[Tài Khoản Thử Nghiệm](#-tài-khoản-trải-nghiệm) •
[Tác Giả](#-tác-giả)

---

</div>

## 🌟 Giới Thiệu Dự Án

**PTIT CBT (Computer-Based Testing)** là đồ án chuyên sâu môn **Cấu trúc Dữ liệu & Giải thuật (CTDL&GT)** tại Học viện Công nghệ Bưu chính Viễn thông (PTITHCM). 

Không dừng lại ở một ứng dụng Terminal/Console đơn giản như các đồ án truyền thống, dự án đã nâng cấp toàn diện lên một hệ sinh thái Full-stack hoàn chỉnh:
- **Core Engine C++17**: Tự cài đặt toàn bộ CTDL từ con trỏ (Pointer, Dynamic Array, Linked List, Binary Search Tree, Hash Table). **Tuyệt đối không dùng STL** (`std::vector`, `std::map`, `std::sort`, `std::find`,...).
- **Multi-threaded HTTP REST API Server**: Đóng gói Core Engine thành Web API hiệu năng cao với `std::shared_mutex` (Read/Write Lock), cơ chế phục hồi phiên thi khi mất điện (*Downtime Gap Recovery*), phân quyền Token-based, và giám sát toàn màn hình (*Anti-cheat Fullscreen*).
- **Interactive Terminal Console**: Ứng dụng Console trực quan, thân thiện, chạy độc lập không cần mạng.
- **Modern Web Client (React 19 + TypeScript)**: Giao diện người dùng chuẩn UX/UI giáo dục hiện đại, Dashboard trực quan hóa dữ liệu với Recharts, quy trình thi 3 bước chống gian lận, xuất bảng điểm ra Excel.

---

## 🚀 Tính Năng Nổi Bật

### 👨‍🏫 Dành cho Giảng Viên / Quản Trị Viên (Admin)
- **Quản lý Môn học**: Thêm, xóa, hiệu chỉnh tên môn học (lưu trữ trên Cây Nhị Phân Tìm Kiếm BST). Ràng buộc bảo vệ dữ liệu (chỉ cho phép xóa môn khi chưa có câu hỏi nào từng ra thi).
- **Ngân hàng Câu hỏi Thi**: Soạn thảo, nhập ngân hàng câu hỏi theo từng môn học; cơ chế phân biệt Xóa mềm (*Soft-delete* để bảo toàn lịch sử bài thi) và Xóa cứng (*Hard-delete*).
- **Quản lý Lớp & Sinh viên**: Quản lý mảng con trỏ lớp học, danh sách liên kết sinh viên theo từng lớp; tự động cấp mã sinh viên và mật khẩu.
- **Báo cáo & Thống kê**: 
  - Xem và in bảng điểm tổng hợp của toàn bộ sinh viên trong lớp theo từng môn học.
  - Tra cứu chi tiết từng bài thi của sinh viên (danh sách câu hỏi đã thi, đáp án sinh viên chọn, đáp án đúng).
  - Xuất báo cáo điểm sang định dạng file Excel (`.xlsx`).
- **Cấu hình Hệ thống**: Bật/tắt chế độ giám sát thi toàn màn hình (Fullscreen Supervision), dọn dẹp và nén dung lượng bộ nhớ đĩa (*Storage Compaction*).

### 🎓 Dành cho Sinh Viên
- **Đăng nhập & Xác thực**: Tra cứu tức thì $O(1)$ thông qua cơ chế Index Manager / Bảng băm.
- **Thi Trắc Nghiệm Thông Minh**:
  - Chọn môn thi, số lượng câu hỏi và thời gian làm bài theo quy chuẩn.
  - Thuật toán sinh đề ngẫu nhiên không trùng lặp (*Fisher-Yates Shuffle* kết hợp mảng con trỏ).
  - Đồng hồ đếm ngược thời gian thực, tự động nộp bài khi hết giờ.
  - Chống nộp bài trùng lặp (*Duplicate Submit Protection*).
  - Tự động bù giờ khi server gặp sự cố mất điện đột ngột (*Power-outage Downtime Gap Adjustment*).
- **Chấm Điểm Tức Thì**: Chấm điểm tự động thang điểm 10, hiển thị kết quả và lưu vết điểm thi vào hồ sơ cá nhân ngay lập tức.
- **Lịch Sử Điểm Thi**: Theo dõi toàn bộ lịch sử các môn đã thi, điểm số và chi tiết từng bài làm.

---

## 🧠 Cấu Trúc Dữ Liệu & Thuật Toán Cốt Lõi

Tuân thủ nghiêm ngặt quy định học thuật: **100% tự phát triển từ đầu, không phụ thuộc thư viện thuật toán STL**.

| Thực Thể | Cấu Trúc Dữ Liệu | Cơ Chế & Tối Ưu Hóa |
| :--- | :--- | :--- |
| **Môn Học** (`Subject`) | **Cây Nhị Phân Tìm Kiếm (BST)** | Tìm kiếm, thêm, sửa theo mã môn học $MAMH$. Duyệt cây cân bằng để tuần tự hóa dữ liệu. Thao tác hủy cây duyệt *Post-order* chống rò rỉ RAM. |
| **Lớp Học** (`Class`) | **Mảng Con Trỏ (`Lop* dslop[10000]`)** | Cấp phát động theo nhu cầu. Tối ưu chi phí hoán vị và sắp xếp chỉ bằng cách đổi con trỏ (8 bytes) thay vì di chuyển toàn bộ struct. |
| **Sinh Viên** (`Student`) | **Danh Sách Liên Kết Đơn (Singly LL)** | Thêm, xóa, cập nhật sinh viên linh hoạt theo từng lớp. Tự động giải phóng bộ nhớ tuần tự khi giải tán lớp. |
| **Câu Hỏi Thi** (`Question`) | **Danh Sách Liên Kết Đơn + Con Trỏ Đuôi (`tail`)** | Thao tác chèn câu hỏi vào cuối danh sách đạt $O(1)$ thay vì $O(N)$. Hỗ trợ cờ đánh dấu câu đã thi (`used`) và xóa mềm (`deleted`). |
| **Điểm Thi** (`Score`) | **Danh Sách Liên Kết Đơn (Singly LL)** | Gắn liền trong từng sinh viên, lưu trữ điểm các môn đã thi kèm chi tiết đáp án đã làm. |
| **Bảng Băm Tăng Tốc** (`HashTable`) | **Bảng băm tự sinh (Custom Hash Table)** | Ánh xạ tức thì $MASV \to \text{Offset}$ và $ID \to \text{Question}$, đẩy tốc độ tra cứu sinh viên toàn cục từ $O(L \times S)$ xuống $O(1)$. |
| **Mảng Động Tự Viết** (`DArray`) | **Dynamic Array Wrapper** | Thay thế `std::vector`, tự động quản lý mở rộng dung lượng $(\times 2)$ và hỗ trợ Deep Copy an toàn. |
| **Sinh Đề Ngẫu Nhiên** | **Fisher-Yates Shuffle Algorithm** | Gom con trỏ các câu hỏi vào mảng tạm và xáo trộn ngẫu nhiên. Đảm bảo phân phối đều và không bao giờ lặp câu hỏi. |

---

## 🏗 Kiến Trúc Hệ Thống

```
ThiTracNghiem/
├── 🖥️ Web Frontend (React 19 + TypeScript + Vite + TailwindCSS v4 + shadcn/ui)
│    │
│    ▼ (REST API / HTTP JSON)
├── ⚡ Backend HTTP Server (C++17, Multi-threaded, cpp-httplib, nlohmann/json)
│    │
│    ├── 🔒 Thread Safety: std::shared_mutex (DB_READ_LOCK / DB_WRITE_LOCK)
│    ├── ⏱️ Session Recovery: Downtime Gap Time Re-calculator
│    ├── 🚀 Index Engine: HashTable & Offset Index Manager
│    │
│    ├── 📦 Core DSA Layer (Strict No-STL: BST, Singly LL, Pointer Array)
│    │
│    ▼ (Fixed-Length Records + Atomic Write)
└── 💾 Storage Layer (Flat-File Engine: subjects.txt, questions.txt, classes.txt, students/, scores/)
```

### 💾 Đột phá tầng Lưu trữ Đĩa (Storage Engine)
- **Fixed-Length Records**: Mỗi bản ghi được định dạng cố định kích thước byte (Môn học: 71B, Lớp: 69B, Sinh viên: 134B, Câu hỏi: 736B, Điểm: 35B). Cho phép truy xuất trực tiếp vị trí bản ghi thông qua `seekg()` trong thời gian $O(1)$.
- **Atomic File Write**: Ghi dữ liệu ra file tạm (`.tmp`) rồi thực hiện atomic swap/rename (`fs::rename`) giúp ngăn ngừa hỏng hóc cơ sở dữ liệu nếu xảy ra mất điện bất ngờ trong lúc ghi.
- **Compaction (Nén đĩa tự động)**: Tự động gom dọn các bản ghi đã bị xóa mềm khi vượt quá ngưỡng rác (25%), trả lại không gian đĩa nguyên vẹn.

---

## 🛠 Tech Stack

### Backend & Core
- **Ngôn ngữ**: C++17
- **Hệ thống Build**: CMake 3.20+ (Tương thích Visual Studio / MSVC / GCC / Clang)
- **Networking**: `cpp-httplib` (Multi-threaded HTTP Server)
- **Serialization**: `nlohmann/json` (Giao tiếp API tiêu chuẩn)
- **Platform API**: Winsock2 (`ws2_32`) trên Windows, tương thích kiến trúc POSIX

### Frontend SPA
- **Framework**: React 19 + TypeScript
- **Bundler**: Vite 8
- **Styling**: Tailwind CSS v4 + Tw-Animate-CSS
- **Component System**: Custom shadcn/ui + Radix UI Primitives
- **Data Visualization**: Recharts (PieChart, BarChart biểu diễn phân phổ điểm)
- **Icons**: Lucide React
- **Export Utility**: ExcelJS

---

## 📂 Cấu Trúc Thư Mục

```bash
ThiTracNghiem/
├── CMakeLists.txt              # Cấu hình biên dịch CMake cho cả Server & Console
├── RULES.MD                    # Quy tắc danh mục cấm dùng STL
├── khomau.md                   # Bộ 65 câu hỏi vấn đáp mổ xẻ mã nguồn chi tiết
│
├── include/                    # Header files: Khai báo toàn bộ CTDL và Storage
│   ├── Subject.h               # Cây BST Môn học
│   ├── Class.h                 # Mảng con trỏ Lớp học
│   ├── Student.h               # DSLK Đơn Sinh viên
│   ├── Question.h              # DSLK Đơn Câu hỏi thi
│   ├── Score.h                 # DSLK Đơn Điểm thi
│   ├── HashTable.h             # Bảng băm tra cứu tự viết
│   ├── DArray.h                # Mảng động tự viết
│   ├── StorageManager.h        # Quản lý đọc/ghi đĩa
│   └── IndexManager.h          # Bộ nhớ đệm chỉ mục Offset
│
├── src/                        # Implementation chi tiết của các CTDL & Storage
├── server/                     # Multi-threaded REST API Server
│   ├── ServerBootstrap.cpp     # Điểm khởi chạy Server, xử lý Downtime gap
│   ├── RouteRegistry.cpp       # Đăng ký các endpoints API
│   └── handlers/               # Controller phân lớp: Auth, Class, Student, Exam...
│
├── console/                    # Ứng dụng Console Terminal tương tác
│   ├── main.cpp                # Điểm khởi chạy Console
│   ├── Menu.cpp                # Hệ thống Menu phân cấp cho GV & SV
│   ├── ConsoleExam.cpp         # Module làm bài thi trực tiếp trên Terminal
│   └── Report.cpp              # In bảng điểm và bài thi trên màn hình console
│
├── frontend/                   # Ứng dụng Web SPA React 19 + TypeScript
│   ├── src/pages/              # 25+ Màn hình giao diện chuyên nghiệp
│   ├── src/components/         # UI Components, Charts, Exam Flow Lock
│   └── package.json            # Thư viện Frontend
│
└── storage/                    # Cơ sở dữ liệu Flat-File dạng text
```

---

## ⚡ Hướng Dẫn Cài Đặt & Khởi Chạy

### 1. Yêu cầu tiên quyết
- **C++**: Trình biên dịch hỗ trợ C++17 (Visual Studio 2019/2022 với C++ Desktop Development, hoặc GCC 9+, Clang 10+).
- **CMake**: Phiên bản 3.20 trở lên.
- **Node.js**: Node 18+ (khuyên dùng Node 20 LTS) và npm.

---

### 2. Biên dịch C++ Backend Server & Console App

```bash
# Clone dự án về máy
git clone https://github.com/windzit/ThiTracNghiem.git
cd ThiTracNghiem

# Tạo thư mục build và sinh cấu hình với CMake
cmake -B build -S .

# Biên dịch dự án (Release mode)
cmake --build build --config Release
```

Sau khi biên dịch thành công, hai file thực thi sẽ nằm trong thư mục `build/` (hoặc `build/Release/`):
- `server.exe` (hoặc `server` trên Linux/macOS): Chạy máy chủ Web API tại cổng `http://localhost:8080`.
- `console.exe` (hoặc `console` trên Linux/macOS): Chạy ứng dụng trực tiếp trên màn hình dòng lệnh.

```bash
# Chạy HTTP REST API Server
./build/Release/server.exe

# Hoặc khởi chạy giao diện Console
./build/Release/console.exe
```

---

### 3. Khởi chạy Web Frontend

Mở một cửa sổ Terminal mới:

```bash
cd frontend

# Cài đặt các gói phụ thuộc
npm install

# Khởi chạy máy chủ phát triển
npm run dev
```

Truy cập ứng dụng tại: **`http://localhost:5173`**

---

## 🔐 Tài Khoản Trải Nghiệm

| Vai Trò | Tên Đăng Nhập / Mã Số | Mật Khẩu | Quyền Hạn |
| :--- | :--- | :--- | :--- |
| **Giảng Viên (Teacher)** | `gvptit` *(hoặc `GV`)* | `123456` | Toàn quyền: Quản lý môn, đề, câu hỏi, lớp, sinh viên, xem và xuất bảng điểm |
| **Sinh Viên (Student)** | `N24DECE046` | `123456` | Làm bài thi trắc nghiệm, xem điểm, tra cứu lịch sử bài làm |

---

## 📡 Tổng Quan REST API

| Nhóm API | Method | Endpoint | Mô Tả |
| :--- | :--- | :--- | :--- |
| **Xác thực** | `POST` | `/api/auth/login` | Đăng nhập hệ thống (Sinh viên / Giảng viên) |
| | `GET` | `/api/auth/me` | Lấy thông tin phiên đăng nhập hiện tại |
| **Môn Học** | `GET` | `/api/subjects` | Lấy danh sách toàn bộ môn học từ cây BST |
| | `POST` | `/api/subjects` | Thêm mới một môn học |
| | `PUT` | `/api/subjects/:id` | Hiệu chỉnh tên môn học |
| | `DELETE`| `/api/subjects/:id` | Xóa môn học (kiểm tra ràng buộc câu hỏi) |
| **Câu Hỏi Thi** | `GET` | `/api/subjects/:id/questions` | Danh sách câu hỏi của một môn học (phân trang) |
| | `POST` | `/api/subjects/:id/questions` | Thêm câu hỏi mới vào ngân hàng đề |
| | `DELETE`| `/api/questions/:id` | Xóa câu hỏi (Xóa mềm hoặc Xóa vật lý) |
| **Thi Cử** | `POST` | `/api/exams/start` | Sinh đề ngẫu nhiên (Fisher-Yates) và bắt đầu phiên |
| | `POST` | `/api/exams/submit` | Nộp bài thi, chấm điểm tự động và lưu lịch sử |
| | `GET` | `/api/exams/session` | Khôi phục trạng thái làm bài (nếu reload/mất kết nối) |
| **Báo Cáo** | `GET` | `/api/reports/class-scores` | Lấy bảng điểm tổng hợp của một lớp theo môn |
| | `GET` | `/api/reports/student-exams` | Lấy chi tiết lịch sử thi của một sinh viên |

---

## 👨‍💻 Tác Giả

* **Đỗ Minh Triết**
  * 🎓 Sinh viên ngành Kỹ thuật Điện tử - Viễn thông, Học viện Công nghệ Bưu chính Viễn thông Cơ sở TP.HCM (PTITHCM).
  * 🆔 MSSV: `N24DECE046`
  * 📧 Email: [n24dece046@student.ptithcm.edu.vn](mailto:n24dece046@student.ptithcm.edu.vn)
  * 🐙 GitHub: [@windzit](https://github.com/windzit)

---

<div align="center">
  <sub>Đồ án môn học Cấu trúc Dữ liệu & Giải thuật • PTIT HCM 2026</sub><br>
  <sub>Được phát triển với niềm đam mê dành cho C++ hiện đại và kiến trúc phần mềm tối ưu.</sub>
</div>

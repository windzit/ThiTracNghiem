# PTIT CBT — Computer Based Testing System (Frontend)

Hệ thống thi trắc nghiệm trực tuyến cho Học viện Công nghệ Bưu chính Viễn thông (PTIT).

## 🛠 Stack công nghệ

- **Framework**: React 18 + TypeScript
- **Build tool**: Vite
- **Styling**: Tailwind CSS v4
- **UI Components**: shadcn/ui (tự viết, không dùng package)
- **Charts**: recharts (PieChart, BarChart, LineChart)
- **Routing**: react-router-dom v6
- **Icons**: lucide-react

## 📦 Cài đặt

```bash
npm install
```

## 🚀 Chạy dự án

```bash
npm run dev       # Dev server tại http://localhost:5173
npm run build     # Build production
npm run preview   # Preview bản build
npm run lint      # Kiểm tra ESLint
```

## 🔐 Test credentials

| Vai trò | Tên đăng nhập | Mật khẩu |
|---------|---------------|----------|
| Sinh viên | `N24DECE046` | `123456` |
| Giảng viên | `gvptit` | `123456` |

## 📁 Cấu trúc thư mục

```
src/
├── components/
│   ├── ui/          # shadcn/ui components (Button, Card, Input, Table, Badge...)
│   ├── shared/      # Shared components (StatCard, Pagination, Drawer, StatusBadge)
│   └── layouts/     # TeacherLayout, StudentLayout (sidebar + topbar)
├── pages/           # 25+ màn hình (xem docs/TASKS.md)
├── data/            # Mock data (mockStudents, mockClasses, mockSubjects...)
├── services/        # Service layer (authService, classService, examService...)
└── lib/             # Utils, helpers
```

## 🏗 Kiến trúc

```
Page → Service → Mock Data
```

- UI không biết data đến từ đâu
- Khi có backend thật: chỉ cần thay Service implementation thành HTTP call → UI giữ nguyên

## 📋 Màn hình đã implement

### Nhóm 0 — Setup
- [x] T00 — Design tokens & shadcn theme

### Nhóm 1 — Đăng nhập
- [x] T01 — Role Selection
- [x] T02 — Login Sinh viên
- [x] T03 — Login Giảng viên

### Nhóm 2 — Dashboard
- [x] T04 — Dashboard Sinh viên
- [x] T05 — Dashboard Giảng viên

### Nhóm 3 — Quản lý lớp
- [x] T06 — Danh sách lớp học
- [x] T06b — Chi tiết lớp (2 tabs)

### Nhóm 4 — Quản lý sinh viên
- [x] T07 — Danh sách sinh viên
- [x] T08 — Hồ sơ sinh viên (4 tabs)

### Nhóm 5 — Quản lý môn học
- [x] T09 — Danh sách môn học
- [x] T10 — Chi tiết môn học (5 tabs)

### Nhóm 6 — Ngân hàng câu hỏi
- [x] T11 — Danh sách câu hỏi
- [x] T12 — Chi tiết câu hỏi

### Nhóm 7 — Tạo đề thi & Làm bài thi
- [x] T13 — Tạo đề thi mới
- [x] T14 — Xem trước đề thi
- [x] T15 — Làm bài thi (3-screen flow + screen lock)

### Nhóm 8 — Kết quả & Bảng điểm
- [x] T16 — Kết quả thi (GV)
- [x] T17 — Bảng điểm tổng hợp
- [x] T18 — Chi tiết bài thi đã làm

### Nhóm 9 — Hoàn thiện
- [x] T19 — Refactor shared components
- [x] T20 — Testing responsive

## 📖 Tài liệu

- `docs/debai.md` — Đề bài chuẩn
- `docs/TASKS.md` — Checklist màn hình + ảnh thiết kế
- `SETUP.md` — Hướng dẫn cài đặt chi tiết
- `.clinerules/` — Quy tắc code cho Cline
- `memory-bank/` — Tiến độ & quyết định kỹ thuật

## ⚠️ Lưu ý

- Dự án này là **frontend-only**, chưa có backend thật
- Dữ liệu hiện tại là mock data trong `src/data/`
- Thiết kế UI bám sát file ảnh trong `design/`
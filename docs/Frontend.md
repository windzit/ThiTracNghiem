# Chi Tiết Kiến Trúc Frontend React FSD (ThiTracNghiem Frontend)

Tài liệu chi tiết về kiến trúc Frontend được xây dựng trên **React 18 + TypeScript + Vite + TailwindCSS + shadcn/ui** áp dụng mô hình **Feature-Sliced Design (FSD)**.

---

## 1. Cấu Trúc Thư Mục Chuẩn Feature-Sliced Design (FSD)

```
frontend/src/
├── app/                  # Tầng Ứng Dụng (App Layer)
│   ├── providers/        # Context Providers (ConnectionContext, ToastContext)
│   ├── styles/           # index.css (Tailwind & CSS Variables)
│   ├── App.tsx           # Config Router toàn cục & Error Boundary
│   └── main.tsx          # Client Entry Point
├── pages/                # Tầng Trang (Pages Layer)
│   ├── auth/             # RoleSelection, StudentLogin, TeacherLogin
│   ├── dashboard/        # StudentDashboard, TeacherDashboard
│   ├── classes/          # ClassManagement, ClassDetail
│   ├── students/         # StudentDetail
│   ├── subjects/         # SubjectManagement, SubjectDetail
│   ├── questions/        # QuestionManagement, QuestionDetail
│   ├── exams/            # TakeExam, TakingExam, ExamConfirm, ExamSubmitted, ExamReview, ExamHistory, ExamDetail
│   └── reports/          # Gradebook
├── widgets/              # Tầng Widget (Widgets Layer)
│   ├── layouts/          # StudentLayout, TeacherLayout, ExamLayout
│   └── notifications/    # NotificationDropdown, NotificationList, NotificationItem
├── features/             # Tầng Tính Năng (Features Layer)
│   └── autocomplete/     # SubjectAutocomplete
├── entities/             # Tầng Thực Thể (Entities Layer)
│   ├── class/            # ClassMapper.ts, classService.ts
│   ├── student/          # StudentMapper.ts, studentService.ts
│   ├── subject/          # SubjectMapper.ts, subjectService.ts
│   ├── question/         # QuestionMapper.ts, questionService.ts
│   ├── exam/             # examService.ts, examSessionService.ts, resultService.ts
│   ├── report/           # reportService.ts
│   └── session/          # authService.ts, systemSettingsService.ts
└── shared/               # Tầng Dùng Chung (Shared Layer)
    ├── ui/               # shadcn/ui primitives (badge, button, card, form, input, select, table, switch, ...)
    ├── components/       # Component dùng chung (StatCard, Pagination, Drawer, PageToolbar, GlobalConnectionOverlay)
    ├── api/              # api.ts (Axios/Fetch Client), ApiErrorHandler.ts
    ├── lib/              # utils.ts, delay.ts, formValidation.ts, formatSubject.ts
    ├── types/            # backend.ts, index.ts
    └── config/           # difficulty.ts, mockCharts.ts
```

---

## 2. Quản Lý Trạng Thái & Context System

Frontend sử dụng React Context API kết hợp Local Storage cho trạng thái ứng dụng:

1. **`ConnectionContext`**:
   - Tự động gửi ping định kỳ tới `/api/system/settings` để kiểm tra kết nối với Backend C++.
   - Hiển thị ngay màn hình đè `GlobalConnectionOverlay` khi mất kết nối backend và tự phục hồi khi backend online trở lại.

2. **`ToastContext`**:
   - Hệ thống thông báo toast tự thiết kế nhẹ nhàng, hỗ trợ 4 loại: `success`, `error`, `warning`, `info`.

3. **`authService`**:
   - Quản lý thông tin đăng nhập của Giảng viên (`teacher`) và Sinh viên (`student`) trong `localStorage`.
   - Cung cấp hàm kiểm tra role `getCurrentUser()`, `isTeacher()`, `isStudent()`.

---

## 3. Luồng Bảo Vệ Thi (Exam Protection Flow)

Màn hình thi (`TakingExam.tsx`) được thiết kế với các quy tắc bảo vệ đặc biệt:

- **Giám Sát Màn Hình Rộng (`fullscreenRequired`)**: Khi Backend kích hoạt chế độ giám sát, giao diện bắt buộc sinh viên phải bật Fullscreen. Nếu thoát khỏi Fullscreen quá số lần cho phép hoặc chuyển tab, hệ thống cảnh báo và có thể tự động nộp bài.
- **Vô Hiệu Hóa Điều Hướng (Navigation Guard)**:
  - Khóa nút Back/Forward của trình duyệt.
  - Ẩn menu Đăng xuất trên Thanh tiêu đề khi sinh viên đang làm bài.
  - Tự động đồng bộ đáp án về Server mỗi khi chọn câu trả lời (`PUT /api/exams/answer`).
- **Phục Hồi Bài Thi (Session Resume)**: Nếu gặp sự cố mất điện/mất mạng, khi quay lại ứng dụng, `TakeExam.tsx` tự động gọi `GET /api/exams/resume` để khôi phục bài thi dở dang và thời gian còn lại chính xác.

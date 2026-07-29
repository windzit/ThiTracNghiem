# Tài Liệu Đặc Tả REST API (ThiTracNghiem REST API Specification)

Bảng đặc tả đầy đủ các API Endpoints của C++ Backend Server.

**Base URL**: `http://localhost:8080`  
**Content-Type**: `application/json`  
**Cấu trúc Response Chuẩn**:
```json
{
  "success": true,
  "data": { ... },
  "message": "Thao tác thành công"
}
```

---

## 1. Authentication (Xác Thực)

### `POST /api/login`
- **Mô tả**: Đăng nhập tài khoản Giảng viên hoặc Sinh viên.
- **Request Body**:
  ```json
  {
    "username": "N22DCCN001",
    "password": "123",
    "role": "student" // "teacher" | "student"
  }
  ```
- **Response Status**: `200 OK`, `401 Unauthorized`, `400 Bad Request`.

### `POST /api/logout`
- **Mô tả**: Đăng xuất tài khoản.
- **Response Status**: `200 OK`.

---

## 2. Admin & System Management (Quản Trị Hệ Thống)

### `POST /api/admin/save`
- **Mô tả**: Ghi ép toàn bộ dữ liệu từ RAM xuống ổ đĩa ngay lập tức.
- **Response Status**: `200 OK`, `500 Internal Server Error`.

### `POST /api/admin/rebuild-used`
- **Mô tả**: Duyệt lại toàn bộ lịch sử thi và ngân hàng câu hỏi để cập nhật lại các cờ `used`.
- **Response Status**: `200 OK`.

### `GET /api/system/settings`
- **Mô tả**: Lấy cài đặt hệ thống hiện tại (chế độ giám sát toàn màn hình `fullscreenRequired`).
- **Response Status**: `200 OK`.

### `POST /api/system/settings`
- **Mô tả**: Cập nhật cài đặt giám sát toàn màn hình.
- **Request Body**: `{"fullscreenRequired": true}`.
- **Response Status**: `200 OK`.

---

## 3. Class Management (Quản Lý Lớp Học)

| HTTP Method | Endpoint Path | Description | Status Code |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/classes` | Lấy danh sách tất cả các lớp học | `200 OK` |
| `GET` | `/api/classes/:id` | Lấy thông tin chi tiết lớp học theo `MALOP` | `200 OK`, `404 Not Found` |
| `POST` | `/api/classes` | Tạo mới lớp học (`MALOP`, `TENLOP`) | `201 Created`, `400 Bad Request`, `409 Conflict` |
| `PUT` | `/api/classes/:id` | Cập nhật tên lớp học `TENLOP` | `200 OK`, `404 Not Found` |
| `DELETE` | `/api/classes/:id` | Xóa lớp học (Chỉ khi sĩ số = 0) | `200 OK`, `400 Bad Request`, `404 Not Found` |

---

## 4. Student Management (Quản Lý Sinh Viên)

| HTTP Method | Endpoint Path | Description | Status Code |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/students` | Lấy toàn bộ danh sách sinh viên | `200 OK` |
| `GET` | `/api/students/:id` | Lấy thông tin sinh viên theo `MASV` | `200 OK`, `404 Not Found` |
| `POST` | `/api/students` | Thêm sinh viên mới vào lớp (`malop`, `masv`, `ho`, `ten`, `phai`, `password`) | `201 Created`, `400 Bad Request`, `409 Conflict` |
| `PUT` | `/api/students/:id` | Cập nhật thông tin sinh viên | `200 OK`, `404 Not Found` |
| `DELETE` | `/api/students/:id` | Xóa sinh viên (Chỉ khi chưa có điểm thi) | `200 OK`, `400 Bad Request`, `404 Not Found` |
| `POST` | `/api/students/bulk-delete` | Xóa danh sách nhiều sinh viên (`{"ids": ["SV01", "SV02"]}`) | `200 OK`, `400 Bad Request` |

---

## 5. Subject Management (Quản Lý Môn Học)

| HTTP Method | Endpoint Path | Description | Status Code |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/subjects` | Lấy danh sách các môn học | `200 OK` |
| `GET` | `/api/subjects/:id` | Lấy chi tiết môn học kèm danh sách câu hỏi | `200 OK`, `404 Not Found` |
| `POST` | `/api/subjects` | Tạo môn học mới (`MAMH`, `TENMH`) | `201 Created`, `400 Bad Request`, `409 Conflict` |
| `PUT` | `/api/subjects/:id` | Cập nhật tên môn học `TENMH` | `200 OK`, `404 Not Found` |
| `DELETE` | `/api/subjects/:id` | Xóa môn học (Chỉ khi chưa dùng trong bài thi) | `200 OK`, `400 Bad Request`, `404 Not Found` |

---

## 6. Question Management (Quản Lý Câu Hỏi)

| HTTP Method | Endpoint Path | Description | Status Code |
| :--- | :--- | :--- | :--- |
| `POST` | `/api/questions` | Thêm câu hỏi mới vào môn học (`mamh`, `noidung`, `a`, `b`, `c`, `d`, `dapan`) | `201 Created`, `400 Bad Request` |
| `PUT` | `/api/questions/:id` | Cập nhật nội dung/đáp án câu hỏi | `200 OK`, `404 Not Found` |
| `DELETE` | `/api/questions/:id` | Xóa câu hỏi (Xóa cứng nếu chưa thi, xóa mềm nếu đã thi) | `200 OK`, `404 Not Found` |
| `POST` | `/api/questions/bulk-delete` | Xóa nhiều câu hỏi (`{"ids": [1, 2, 3]}`) | `200 OK`, `400 Bad Request` |

---

## 7. Exam Execution (Thi Trắc Nghiệm)

### `POST /api/exams/start`
- **Mô tả**: Khởi tạo phiên thi trắc nghiệm mới cho sinh viên. Hệ thống tự chọn ngẫu nhiên bộ câu hỏi theo thuật toán Fisher-Yates.
- **Request Body**: `{"masv": "N22DCCN001", "mamh": "INT1339", "socauthi": 20, "thoigian": 15}`.
- **Response**: Trả về danh sách câu hỏi (không kèm đáp án đúng) và `remainingSeconds`.

### `GET /api/exams/resume`
- **Mô tả**: Khôi phục phiên thi dở dang khi bị rớt mạng hoặc khởi động lại trình duyệt.
- **Query Params**: `?masv=N22DCCN001`.

### `PUT /api/exams/answer`
- **Mô tả**: Lưu đáp án câu hỏi sinh viên vừa chọn.
- **Request Body**: `{"masv": "N22DCCN001", "questionId": 10, "selectedAnswer": "A"}`.

### `POST /api/exams/submit`
- **Mô tả**: Nộp bài thi, chấm điểm tự động và lưu điểm thi vào danh sách điểm.

---

## 8. Reports & Scoreboard (Báo Cáo & Xem Điểm)

### `GET /api/reports/exam`
- **Mô tả**: Lấy chi tiết lịch sử bài thi của sinh viên (câu hỏi, đáp án đã chọn, đáp án đúng).
- **Query Params**: `?masv=N22DCCN001&mamh=INT1339`.

### `GET /api/reports/scoreboard`
- **Mô tả**: Lấy bảng điểm thi môn học của toàn bộ sinh viên thuộc 1 lớp.
- **Query Params**: `?malop=D22CQCN01&mamh=INT1339`.

### `DELETE /api/scores`
- **Mô tả**: Xóa điểm thi của sinh viên để sinh viên được thi lại.
- **Query Params**: `?masv=N22DCCN001&mamh=INT1339`.

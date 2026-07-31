# 06. Danh mục API HTTP (API Reference)

Tài liệu này tổng hợp toàn bộ các **HTTP REST API Endpoints** được hệ thống Web Server C++ cung cấp, kèm theo mô tả Request Payload, Response JSON và mã trạng thái HTTP (Status Codes).

---

## 1. Quy chuẩn Chung của API

- **Base URL**: `http://localhost:8080`
- **Content-Type**: `application/json`
- **Response Format Envelope**:
  - Khi thành công ($200, 201$):
    ```json
    {
      "success": true,
      "data": { ... }
    }
    ```
  - Khi thất bại ($400, 401, 404, 500$):
    ```json
    {
      "success": false,
      "message": "Chi tiết câu thông báo lỗi"
    }
    ```
  - Thao tác Xóa thành công không có body ($204 No Content$).

---

## 2. Nhóm Xác thực & Hệ thống (Auth & Admin API)

### 2.1 `POST /api/login`
- **Mục đích**: Đăng nhập tài khoản Giảng viên hoặc Sinh viên.
- **Request Body**:
  ```json
  {
    "username": "GV",
    "password": "GV"
  }
  ```
  *(Hoặc Sinh viên: `"username": "N15DCCN001"`, `"password": "123"`)*
- **Response 200 OK**:
  ```json
  {
    "success": true,
    "data": {
      "role": "GV",
      "username": "GV",
      "displayName": "Giảng viên"
    }
  }
  ```

### 2.2 `POST /api/logout`
- **Mục đích**: Đăng xuất tài khoản.
- **Response 200 OK**: `{ "success": true, "data": { "message": "Logged out" } }`

### 2.3 `POST /api/admin/save`
- **Mục đích**: Yêu cầu lưu khẩn cấp toàn bộ dữ liệu từ RAM xuống các tệp đĩa `.txt` và chỉ mục `.idx`.
- **Response 200 OK**: `{ "success": true, "data": { "message": "Data saved successfully" } }`

### 2.4 `POST /api/admin/rebuild-used`
- **Mục đích**: Quét lại toàn bộ các bài thi lịch sử để xây dựng lại cờ `used` cho Môn học và Câu hỏi.
- **Response 200 OK**: `{ "success": true, "data": { "message": "Rebuilt used flags" } }`

### 2.5 `GET /api/system/settings` & `POST /api/system/settings`
- **Mục đích**: Xem và cập nhật cấu hình hệ thống (như yêu cầu thi toàn màn hình `fullscreenRequired`).

---

## 3. Nhóm Quản lý Lớp học (Class API)

| HTTP Method | Endpoint | Mô tả | Request Body |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/classes` | Lấy danh sách tất cả các Lớp | Không |
| `GET` | `/api/classes/:id` | Xem chi tiết Lớp theo `MALOP` | Không |
| `POST` | `/api/classes` | Tạo mới một Lớp học | `{ "malop": "D21CQCN01", "tenlop": "Lớp CN1" }` |
| `PUT` | `/api/classes/:id` | Cập nhật tên Lớp | `{ "tenlop": "Lớp CN1 - Mới" }` |
| `DELETE` | `/api/classes/:id` | Xóa Lớp (Chỉ khi Lớp chưa có SV) | Không |

---

## 4. Nhóm Quản lý Sinh viên (Student API)

| HTTP Method | Endpoint | Mô tả | Request Body |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/students` | Lấy DS Sinh viên (Lọc theo `malop`) | Query param: `?malop=D21CQCN01` |
| `GET` | `/api/students/:id` | Xem thông tin 1 Sinh viên theo `MASV` | Không |
| `POST` | `/api/students` | Thêm mới 1 Sinh viên vào Lớp | `{ "malop": "D21CQCN01", "masv": "N21DCCN001", "ho": "Nguyễn", "ten": "An", "phai": "Nam", "password": "123" }` |
| `PUT` | `/api/students/:id` | Cập nhật thông tin Sinh viên | `{ "ho": "Nguyễn", "ten": "An Bình", "phai": "Nam", "password": "456" }` |
| `DELETE` | `/api/students/:id` | Xóa 1 Sinh viên (Chỉ khi chưa thi) | Không |
| `POST` | `/api/students/bulk-delete` | Xóa nhiều Sinh viên | `{ "masvs": ["N21DCCN001", "N21DCCN002"] }` |

---

## 5. Nhóm Quản lý Môn học (Subject API)

| HTTP Method | Endpoint | Mô tả | Request Body |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/subjects` | Lấy danh sách tất cả Môn học từ BST | Không |
| `GET` | `/api/subjects/:id` | Tìm chi tiết Môn học theo `MAMH` | Không |
| `POST` | `/api/subjects` | Thêm Môn học mới vào Cây BST | `{ "mamh": "CTDL", "tenmh": "Cấu trúc dữ liệu" }` |
| `PUT` | `/api/subjects/:id` | Cập nhật Tên môn học | `{ "tenmh": "Cấu trúc dữ liệu & Giải thuật" }` |
| `DELETE` | `/api/subjects/:id` | Xóa Môn học (Chỉ khi chưa dùng) | Không |

---

## 6. Nhóm Quản lý Câu hỏi Thi (Question API)

| HTTP Method | Endpoint | Mô tả | Request Body & Trả về |
| :--- | :--- | :--- | :--- |
| `POST` | `/api/questions` | Thêm Câu hỏi thi mới vào Môn học | **Request Body**: `{ "mamh": "CTDL", "noidung": "...", "a": "..", "b": "..", "c": "..", "d": "..", "dapan": "A" }`<br>**Response 201**: `{ "id": 105, "mamh": "CTDL", "noidung": "...", "a": "..", "b": "..", "c": "..", "d": "..", "dapan": "A", "used": false, "deleted": false }` |
| `PUT` | `/api/questions/:id` | Cập nhật nội dung/đáp án câu hỏi | **Request Body**: `{ "noidung": "Nội dung mới", "a": "..", "b": "..", "c": "..", "d": "..", "dapan": "B" }`<br>**Response 200**: `{ "id": 105, "mamh": "CTDL", "noidung": "Nội dung mới", "a": "..", "b": "..", "c": "..", "d": "..", "dapan": "B", "used": false, "deleted": false }` |
| `DELETE` | `/api/questions/:id` | Xóa câu hỏi (Xóa cứng/mềm tự động) | Không (Trả về `204 No Content`) |
| `POST` | `/api/questions/bulk-delete` | Xóa nhiều câu hỏi theo DS ID | **Request Body**: `{ "ids": [101, 102, 103] }` |

---

## 7. Nhóm Thi Trắc nghiệm (Exam API)

### 7.1 `POST /api/exams/start`
- **Mục đích**: Bắt đầu một bài thi trắc nghiệm mới (Tạo đề ngẫu nhiên).
- **Request Body**:
  ```json
  {
    "masv": "N21DCCN001",
    "mamh": "CTDL",
    "soCau": 20,
    "thoiGianPhut": 15
  }
  ```
- **Response 200 OK**: Trả về danh sách câu hỏi (Không kèm đáp án đúng `DAPAN_DUNG`) và thời gian thi còn lại.

### 7.2 `GET /api/exams/resume`
- **Mục đích**: Phục hồi phiên thi dang dở nếu sinh viên reload trang hoặc rớt mạng.
- **Query Params**: `?masv=N21DCCN001`
- **Response 200 OK**: Trả về đề thi, các câu đáp án sinh viên đã chọn trước đó và thời gian còn lại (tự động trừ thời gian trôi qua).

### 7.3 `PUT /api/exams/answer`
- **Mục đích**: Lưu tạm thời câu trả lời của sinh viên lên đĩa `exam_sessions.txt` ngay khi chọn đáp án.
- **Request Body**: `{ "masv": "N21DCCN001", "questionIndex": 3, "answer": "B" }`

### 7.4 `POST /api/exams/submit`
- **Mục đích**: Nộp bài thi, tự động chấm điểm, ghi điểm vào `dsDiemThi` của Sinh viên, đánh dấu cờ `used` và xóa phiên thi.
- **Request Body**: `{ "masv": "N21DCCN001" }`
- **Response 200 OK**: `{ "success": true, "data": { "diem": 8.5, "soCauDung": 17, "tongSoCau": 20 } }`

---

## 8. Nhóm Báo cáo & Thống kê (Report API)

| HTTP Method | Endpoint | Mô tả | Query Params / Request Body |
| :--- | :--- | :--- | :--- |
| `GET` | `/api/reports/exam` | Xem chi tiết bài thi lịch sử của 1 SV | `?masv=N21DCCN001&mamh=CTDL` |
| `GET` | `/api/reports/scoreboard` | In Bảng điểm thi môn học của 1 Lớp | `?malop=D21CQCN01&mamh=CTDL` |
| `DELETE` | `/api/scores` | Xóa điểm thi của 1 SV | `?masv=N21DCCN001&mamh=CTDL` |

---

## 9. Các File Mã Nguồn Liên quan

- [RouteRegistry.cpp](file:///server/RouteRegistry.cpp): Đăng ký danh mục các tuyến đường API.
- Thư mục `[server/handlers/](file:///server/handlers/)`: Chứa toàn bộ mã nguồn xử lý chi tiết các API endpoints.

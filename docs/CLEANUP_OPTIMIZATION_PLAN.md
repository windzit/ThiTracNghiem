# KẾ HOẠCH CLEANUP VÀ TINH GỌN MÃ NGUỒN TỐI THIỂU (CLEANUP OPTIMIZATION PLAN)

> **TRẠNG THÁI:** CHỈ LẬP KẾ HOẠCH (PLANNING ONLY). **CHƯA MODIFY BẤT KỲ FILE CODE NÀO.**  
> **MỤC TIÊU:** Tinh gọn mã nguồn thực sự dư thừa, giữ lại cơ chế phòng thủ cần thiết, không over-engineer.

---

## I. CẤU TRÚC PHÂN KỲ TRIỂN KHAI (PHASED CLEANUP)

### 🔹 GIAI ĐOẠN 1: SAFE CLEANUP (Thực hiện)
Bao gồm các thay đổi có rủi ro cực thấp, không ảnh hưởng đến bất kỳ luồng dữ liệu hay logic nghiệp vụ nào:
- **CLEANUP-1:** Loại bỏ `set_cors_headers(res)` lặp lại trong 15 handlers.
- **CLEANUP-3:** Loại bỏ check độ dài thủ công `mamh.length() > 15` trong `SubjectHandler::handle_create_subject`.

### 🔹 GIAI ĐOẠN 2: DEFENSIVE / VALIDATION (Giữ nguyên)
- **CLEANUP-2:** `StorageValidator::validateScore` trong `saveScores()`.
  - *Quyết định:* **KEEP AS DEFENSE-IN-DEPTH**. Giữ nguyên như một lớp phòng vệ thứ cấp cho thao tác Full Rewrite file `scores.txt`.

### 🔹 GIAI ĐOẠN 3: OPTIONAL I/O / CACHE (Giữ nguyên)
- **CLEANUP-5:** Đọc `exam_history.txt` trong `rebuildUsedFlags()`.
  - *Quyết định:* **KEEP AS-IS**. Tránh over-engineering thêm Cache phụ trợ vì luồng bắt đầu thi (`exam_start`) đã không còn đọc đĩa.

### 🛑 KEEP AS-IS (Cố tình không sửa để đảm bảo an toàn)
- **CLEANUP-4:** `dsdiemthi.find(mamh)` tại `exam_start` và `exam_submit`.
  - *Lý do:* Bảo vệ tính Bất biến và Chống nộp bài trùng lặp (Idempotency & Concurrency Safety).

---

## II. CHI TIẾT TỪNG HẠNG MỤC CLEANUP

---

### CLEANUP-1: Loại bỏ lời gọi `set_cors_headers(res)` trong các Handler

#### 1. Current
Mỗi handler khi nhận request đều gọi `set_cors_headers(res)` ở dòng đầu tiên, trong khi `RouteRegistry.cpp` đã cấu hình `svr.set_default_headers(...)`.

#### 2. Problem
Dư thừa code boilerplate, thiết lập 2 lần cùng một tập HTTP headers cho mỗi request.

#### 3. Evidence
- `RouteRegistry.cpp` (L32-36): `svr.set_default_headers(...)` được cấu hình lúc server khởi động.
- `httplib.h` (L12412): `res.headers = default_headers_;` tự động gán cho mọi response.
- 15 handlers đều gọi thêm hàm `set_cors_headers(res)`.

#### 4. Proposed Change
1. Xóa lời gọi `set_cors_headers(res);` trong tất cả 15 handlers.
2. Xóa hàm `set_cors_headers` trong `ServerContext.h` và `ServerContext.cpp`.

#### 5. Files & Functions
- `server/ServerContext.h`: `set_cors_headers` (Xóa khai báo)
- `server/ServerContext.cpp`: `set_cors_headers` (Xóa định nghĩa)
- `server/handlers/AdminHandler.cpp`: `handle_admin_save`, `handle_get_system_settings`, `handle_post_system_settings`, `handle_rebuild_used`
- `server/handlers/AuthHandler.cpp`: `handle_login`, `handle_logout`
- `server/handlers/ClassHandler.cpp`: `handle_get_classes`, `handle_get_class_by_id`, `handle_create_class`, `handle_update_class`, `handle_delete_class`
- `server/handlers/ExamHandler.cpp`: `handle_exam_start`, `handle_exam_resume`, `handle_exam_answer`, `handle_exam_submit`
- `server/handlers/QuestionHandler.cpp`: `handle_create_question`, `handle_bulk_delete_questions`, `handle_update_question`, `handle_delete_question`, `handle_restore_question`
- `server/handlers/ReportHandler.cpp`: `handle_report_exam`, `handle_report_scoreboard`, `handle_delete_score`
- `server/handlers/StudentHandler.cpp`: `handle_get_students`, `handle_get_student_by_id`, `handle_create_student`, `handle_update_student`, `handle_delete_student`, `handle_bulk_delete_students`
- `server/handlers/SubjectHandler.cpp`: `handle_get_subjects`, `handle_get_subject_by_id`, `handle_create_subject`, `handle_update_subject`, `handle_delete_subject`

#### 6. Risk
- **Mức độ:** `LOW (Rất thấp)`.
- **Phân tích:** `httplib::Server` đảm bảo 100% response đều mang `default_headers_`.

#### 7. Benefit
- Giảm ~25 dòng code thừa.
- Loại bỏ sự phân tán cấu hình header; CORS chỉ quản lý tại một điểm duy nhất (`RouteRegistry.cpp`).

#### 8. Preconditions
`svr.set_default_headers()` đã được cấu hình trong `RouteRegistry.cpp`.

#### 9. Regression Risk
Không có. Đã kiểm chứng coverage 100% trong mã nguồn `httplib.h`.

#### 10. Why Not Over-Engineering
Đây là việc xóa code thừa (shrink), không tạo thêm bất kỳ class, wrapper hay abstraction nào.

#### 11. Complexity
- Before: $\mathcal{O}(1)$ header copy 2 lần.
- After: $\mathcal{O}(1)$ header copy 1 lần.

---

### CLEANUP-3: Đồng bộ Validation trong `handle_create_subject`

#### 1. Current
Trong `handle_create_subject`, có kiểm tra `if (mamh.length() > 15)` trước khi gọi `StorageValidator::validateSubject`.

#### 2. Problem
Kiểm tra độ dài bị lặp lại ở cả tầng Handler và tầng `StorageValidator`.

#### 3. Evidence
- `SubjectHandler.cpp` (L61-63):
  ```cpp
  if (mamh.length() > 15) {
      error_response(res, "mamh must be at most 15 characters", 400); return;
  }
  ```
- `StorageValidator.cpp` (L168-172):
  ```cpp
  if (mamh.length() > 15) {
      errReason = "MAMH exceeds maximum length 15: '" + mamh + "'";
      logValidationError("Subject", mamh, "MAMH", mamh, errReason);
      return false;
  }
  ```

#### 4. Proposed Change
Xóa đoạn `if (mamh.length() > 15)` trong `SubjectHandler.cpp`, để `StorageValidator::validateSubject` kiểm tra tập trung (giống như đã làm ở `ClassHandler.cpp`).

#### 5. Files & Functions
- `server/handlers/SubjectHandler.cpp`: `handle_create_subject`

#### 6. Risk
- **Mức độ:** `LOW (Rất thấp)`.
- **Phân tích:** Mã lỗi HTTP (400) giữ nguyên.

#### 7. Benefit
- Đảm bảo `StorageValidator` là **Single Source of Truth** cho các quy tắc hợp lệ của Môn học.

#### 8. Preconditions
`StorageValidator::validateSubject` đã có đầy đủ logic kiểm tra `length > 15`.

#### 9. Regression Risk
Không có.

#### 10. Why Not Over-Engineering
Chỉ xóa 3 dòng code lặp.

---

## III. CHI TIẾT CÁC HẠNG MỤC GIỮ NGUYÊN (KEEP AS-IS RATIONALE)

---

### 1. CLEANUP-2: `validateScore` trong `saveScores()`
- **Lý do giữ nguyên:**
  1. `saveScores` là hàm ghi đè toàn bộ file `scores.txt`.
  2. Hàm này hiện tại **chỉ chạy khi Admin xóa điểm** (`handle_delete_score`), không nằm trong luồng nộp bài của học sinh (`handle_exam_submit` đã dùng `appendScore`).
  3. Giữ lại `validateScore` đóng vai trò là chốt chặn an toàn phòng vệ (Defense-in-Depth), đảm bảo không bao giờ ghi dữ liệu rác/hỏng xuống đĩa nếu có lỗi bất thường trên RAM.

---

### 2. CLEANUP-4: `dsdiemthi.find(mamh)` ở cả `exam_start` và `exam_submit`
- **Lý do giữ nguyên:**
  1. **Không phải duplicate logic:** Lệnh ở `exam_start` kiểm tra điều kiện để phát đề thi; lệnh ở `exam_submit` kiểm tra tính bất biến để chống nộp bài 2 lần khi gặp lag mạng hoặc click đúp.
  2. Khoảng cách giữa 2 lần kiểm tra là cả một quá trình làm bài thi (15 - 120 phút).
  3. Xóa check ở `exam_submit` sẽ tạo lỗ hổng bảo mật nghiêm trọng (Race Condition & Double Submission).

---

### 3. CLEANUP-5: Đọc `exam_history.txt` trong `rebuildUsedFlags()`
- **Lý do giữ nguyên:**
  1. OPT-3 đã giải quyết triệt để nút thắt hiệu năng: **Luồng bắt đầu thi (`exam_start`) đã không còn đọc đĩa**.
  2. `rebuildUsedFlags` hiện chỉ chạy 1 lần khi nộp bài xong.
  3. Việc tạo thêm cơ chế Cache trong RAM cho lịch sử thi sẽ làm tăng độ phức tạp quản lý bộ nhớ, vòng đời đồng bộ và nguy cơ stale cache cao hơn nhiều so với việc đọc file văn bản nhỏ lúc kết thúc thi.

---

## IV. TIÊU CHÍ NGHIỆM THU (ACCEPTANCE CRITERIA)

Sau khi được phê duyệt và triển khai:
```text
✅ Server compile & link 100% thành công không có lỗi
✅ 20/20 Automated Tests (--test-validation) PASSED
✅ CORS headers hoạt động bình thường trên trình duyệt Frontend (React port 5173)
✅ Không có bất kỳ thay đổi nào phá vỡ business logic hay data structures
✅ Không phát sinh thêm abstraction hay over-engineering
```

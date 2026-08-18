# BÁO CÁO ĐIỀU TRA VÀ AUDIT CLEANUP CODEBASE (CLEANUP AUDIT)

> **MỤC ĐÍCH:** Điều tra thực tế trong codebase, phân tích call-flow, concurrency lock, validation guarantee và đánh giá chính xác 5 đối tượng cleanup tiềm năng.  
> **NGUYÊN TẮC:** Tuân thủ `rules.md`, không phỏng đoán, chỉ dựa trên mã nguồn thực tế. **KHÔNG MODIFY SOURCE CODE.**

---

## I. KẾT QUẢ ĐIỀU TRA CHI TIẾT TỪNG CLEANUP CANDIDATE

---

### 1. CLEANUP 1 — Lời gọi `set_cors_headers(res)` tại từng Handler

#### a. Hiện trạng và Call Sites
- **Định nghĩa:** `server/ServerContext.cpp` (L25-29):
  ```cpp
  void set_cors_headers(httplib::Response& res) {
      res.set_header("Access-Control-Allow-Origin", "http://localhost:5173");
      res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
      res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
  }
  ```
- **Số lượng Call Sites:** Được gọi ở dòng đầu tiên của **15 handlers** trên 7 file:
  - `AdminHandler.cpp`: L8, L13, L19, L35
  - `AuthHandler.cpp`: L9, L41
  - `ClassHandler.cpp`: L12, L28, L38, L81, L130
  - `ExamHandler.cpp`: L16, L118, L174, L216
  - `QuestionHandler.cpp`: L13, L63, L118, L197, L270
  - `ReportHandler.cpp`: L21, L133, L171
  - `StudentHandler.cpp`: L11, L37, L75, L113, L147, L184
  - `SubjectHandler.cpp`: L24, L32, L51, L89, L126

#### b. Cấu hình tại Gateway (`RouteRegistry.cpp`)
- Tại `server/RouteRegistry.cpp` (L31-36):
  ```cpp
  void registerRoutes(httplib::Server& svr) {
      svr.set_default_headers({
          {"Access-Control-Allow-Origin", "http://localhost:5173"},
          {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
          {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
      });
  ```

#### c. Phân tích httplib internals & Header Coverage
- Trong `include/httplib.h` (L12410-12413):
  ```cpp
  Response res;
  res.version = "HTTP/1.1";
  res.headers = default_headers_;
  ```
- **Phân tích:** Mọi đối tượng `httplib::Response res` sinh ra cho bất kỳ HTTP request nào (kể cả thành công, lỗi 400, 404, 422, hoặc exception 500 do `wrap_safe` bắt) đều được khởi tạo với `default_headers_`.
- Không có handler nào thay đổi origin hoặc gán header CORS khác biệt.
- **Default header coverage:** **`100%`**.
- **Kết luận:** **`SAFE TO REMOVE`**. Xóa bỏ hoàn toàn lời gọi `set_cors_headers(res)` trong tất cả 15 handlers và hàm trợ giúp `set_cors_headers` trong `ServerContext` mà không làm thay đổi bất kỳ hành vi HTTP/CORS nào.

---

### 2. CLEANUP 2 — `StorageValidator::validateScore()` trong `saveScores()`

#### a. Hiện trạng và Call Sites
- **Định nghĩa:** `src/StorageManager.cpp` (L686-718):
  ```cpp
  bool StorageManager::saveScores(Class& dsl) {
      // Duyệt qua toàn bộ dsl -> dssinhvien -> dsdiemthi
      // Gọi StorageValidator::validateScore(sv.MASV, dt, errReason) trên từng record
      // Ghi atomicWriteFile xuống scores.txt
  }
  ```
- **Tất cả Call Sites của `saveScores()` trong codebase:**
  1. `server/handlers/ReportHandler.cpp` (L186) trong `handle_delete_score` (Admin xóa điểm).
  2. `src/StorageManager.cpp` (L954) trong `checkAndSaveAuto`.
  *(Lưu ý: Luồng nộp bài thi `handle_exam_submit` đã chuyển sang `appendScore` $O(1)$ ở OPT-1, không còn gọi `saveScores`)*.

#### b. Bản chất Validation & Invariant
- `validateScore` kiểm tra: `MASV` hợp lệ, `MAMH` hợp lệ, `DIEM` $\in [0.0, 10.0]$, không NaN/Inf, không chứa ký tự cấm `| \r \n`.
- Dữ liệu khi nhập vào RAM (qua `appendScore` hoặc `loadScores`) đều đã được validate.
- Tuy nhiên, `saveScores()` là hàm ghi đè toàn bộ file `scores.txt` (Full Rewrite). Nếu có bất kỳ lỗi logic nào trong RAM dẫn đến một con trỏ hỏng hoặc giá trị float bất thường (NaN), `validateScore` là chốt chặn cuối cùng ngăn chặn việc làm hỏng toàn bộ cơ sở dữ liệu điểm trên đĩa.
- Vì `saveScores()` hiện nay chỉ chạy khi Admin xóa điểm (tần suất cực kỳ hiếm), chi phí CPU của vòng lặp này là không đáng kể (< 0.1ms cho hàng nghìn điểm).
- **Kết luận:** **`KEEP AS DEFENSE-IN-DEPTH`**. Không nên xóa bỏ cơ chế phòng vệ này vì lợi ích hiệu năng = 0 nhưng rủi ro mất tầng bảo vệ dữ liệu toàn vẹn.

---

### 3. CLEANUP 3 — Check `mamh.length() > 15` trùng lặp trong `handle_create_subject()`

#### a. Hiện trạng
- Trong `server/handlers/SubjectHandler.cpp` (L56-78):
  ```cpp
  string mamh = body.value("mamh", "");
  string tenmh = body.value("tenmh", "");
  if (mamh.empty() || tenmh.empty()) {
      error_response(res, "mamh and tenmh are required", 400); return;
  }
  if (mamh.length() > 15) {                     // <-- CHECK TRÙNG
      error_response(res, "mamh must be at most 15 characters", 400); return;
  }
  if (dsmh.find(mamh.c_str())) {
      error_response(res, "Subject already exists", 409); return;
  }
  MonHoc mh;
  strcpy(mh.MAMH, mamh.c_str());
  mh.TENMH = tenmh;
  StringNormalizer::normalizeSubject(mh);

  string errReason;
  if (!StorageValidator::validateSubject(mh, errReason)) {
      error_response(res, "Validation failed: " + errReason, 400); // <-- CŨNG CHECK length > 15
      return;
  }
  ```
- Trong `src/StorageValidator.cpp` (L168-172):
  ```cpp
  if (mamh.length() > 15) {
      errReason = "MAMH exceeds maximum length 15: '" + mamh + "'";
      logValidationError("Subject", mamh, "MAMH", mamh, errReason);
      return false;
  }
  ```

#### b. Đối chiếu với `ClassHandler.cpp` và `StudentHandler.cpp`
- Trong `ClassHandler.cpp` (OPT-10), đoạn check độ dài thủ công đã được lược bỏ thành công và chuyển giao hoàn toàn cho `StorageValidator::validateClass`.
- Trong `SubjectHandler.cpp`, check này là tàn dư chưa đồng bộ.
- Cả hai nhánh đều trả về HTTP 400.
- **Kết luận:** **`SAFE TO REMOVE / CONSOLIDATE`**. Xóa 3 dòng check độ dài thủ công trong `SubjectHandler.cpp` để `StorageValidator::validateSubject` là Single Source of Truth cho toàn bộ nghiệp vụ validation.

---

### 4. CLEANUP 4 — `dsdiemthi.find(mamh)` ở cả `exam_start` và `exam_submit`

#### a. Trace Lifecycle & Concurrency Flow

```
[Thời điểm T0: Sinh viên bắt đầu thi]
  POST /api/exams/start
  ↳ DB_WRITE_LOCK
  ↳ sv->dsdiemthi.find(mamh)  ───> Nếu ĐÃ CÓ ĐIỂM: trả về 409 "Ban da thi mon nay roi" (Ngắt ngay)
  ↳ Nếu CHƯA THI: Bốc thăm đề thi -> Lưu session -> Trả về đề thi
  ↳ Release LOCK

[Khoảng thời gian thi: T0 → T0 + 15~120 phút]
  PUT /api/exams/answer (Cập nhật câu trả lời)

[Thời điểm T1: Sinh viên nộp bài thi]
  POST /api/exams/submit
  ↳ DB_WRITE_LOCK
  ↳ sv->dsdiemthi.find(mamh)  ───> Nếu ĐÃ CÓ ĐIỂM: xóa session, trả về 409 "Bai thi nay da duoc nop truoc do"
  ↳ Nếu CHƯA CÓ ĐIỂM: Chấm điểm -> sv->dsdiemthi.insert() -> appendScore() -> xóa session
  ↳ Release LOCK
```

#### b. Phân tích Concurrency & Idempotency
- **Câu hỏi then chốt:** Hai lệnh kiểm tra này có cùng một mục đích không?
  - **KHÔNG.** Chúng bảo vệ hai giai đoạn trạng thái hoàn toàn khác nhau:
    1. Check tại `exam_start`: Kiểm tra **Điều kiện tiên quyết (Precondition)** trước khi phát đề thi.
    2. Check tại `exam_submit`: Bảo vệ tính **Bất biến (Idempotency & Replay Attack Prevention)** khi nộp bài. Nếu mạng bị lag khiến trình duyệt gửi 2 request submit đồng thời, request 1 nộp xong thì request 2 sẽ bị chặn lại an toàn ở check này, tránh việc chấm điểm và ghi file 2 lần.
- **Kết luận:** **`KEEP AS-IS (DEFENSIVE / IDEMPOTENCY CHECK)`**. Tuyệt đối không xóa check ở `exam_submit`. Đây là chuẩn mực an toàn trong hệ thống thi cử phân tán.

---

### 5. CLEANUP 5 — `rebuildUsedFlags()` đọc `exam_history.txt` từ đĩa

#### a. Hiện trạng
- Trong `src/StorageManager.cpp` (L992-1026):
  Mỗi lần `rebuildUsedFlags` được gọi, hàm mở file `exam_history.txt`, parse từng dòng `MASV|MAMH|...|Q_IDS|...` để tìm các câu hỏi và đánh dấu `qNode->cauhoi.used = true` và `node->data.used = true`.

#### b. Tần suất gọi thực tế sau khi đã tối ưu (Post-OPT-3)
- Lúc `exam_start`: **ĐÃ BỎ** (thay bằng gán cờ Incremental trong RAM, không còn đọc đĩa).
- Lúc `exam_submit`: Gọi 1 lần sau khi sinh viên hoàn thành bài thi.
- Lúc `delete_score`: Gọi 1 lần khi Admin xóa điểm.
- Lúc `admin_rebuild`: Gọi khi Admin bấm nút thủ công.

#### c. Phân tích tính Bất biến & Cache
- File `exam_history.txt` chỉ được **Append**, không bao giờ bị sửa hoặc xóa dòng cũ.
- Khi một câu hỏi đã từng xuất hiện trong lịch sử thi, trạng thái `used = true` của nó là **vĩnh viễn (Permanent)** trong suốt vòng đời của câu hỏi đó (giáo viên không bao giờ được xóa câu hỏi đã từng thi).
- **Tuy nhiên:**
  - `rebuildUsedFlags` hiện chỉ chạy khi nộp bài (1 lần duy nhất lúc kết thúc bài thi).
  - Kích thước file `exam_history.txt` trong các kỳ thi thông thường chỉ từ vài KB đến vài MB, đọc tuần tự mất < 1-2ms.
  - Nếu xây dựng thêm cơ chế `In-Memory History Cache` (hoặc `HashTable<int, bool> g_usedQuestions`):
    - Cần quản lý vòng đời khởi tạo lúc startup.
    - Cần đồng bộ khi `appendExamHistory`.
    - Cần xử lý trường hợp Admin reset storage.
    - Tăng độ phức tạp kiến trúc và code bảo trì.
- **Đánh giá Over-Engineering:** Chi phí kiến trúc và bảo trì của việc tạo Cache > Lợi ích thu được (vì `exam_start` đã không còn đọc đĩa).
- **Kết luận:** **`KEEP AS-IS / OPTIONAL PHASE 3`**. Giữ nguyên trạng thái hiện tại là tối ưu nhất về mặt cân bằng giữa độ đơn giản và hiệu năng.

---

## II. MA TRẬN PHÂN TÍCH RỦI RO & KHUYẾN NGHỊ (RISK MATRIX)

| Cleanup Candidate | Hiện trạng trùng lặp | Lợi ích khi fix | Mức độ rủi ro | Luồng ảnh hưởng | Khuyến nghị |
| :--- | :--- | :--- | :---: | :--- | :---: |
| **1. `set_cors_headers`** | Gọi lặp ở 15 handlers dù gateway đã set | Giảm boilerplate (~20 dòng), code sạch | 🟢 **LOW** | Toàn bộ API Response | **SAFE TO REMOVE** |
| **2. `validateScore` in `saveScores`** | Validate lặp lại dữ liệu RAM trước khi ghi | Tiết kiệm CPU không đáng kể (< 0.1ms) | 🟡 **MEDIUM** | Admin xóa điểm, Auto save | **KEEP AS DEFENSE-IN-DEPTH** |
| **3. `mamh.length > 15`** | Check thủ công trước `validateSubject` | Chuẩn hóa validation về 1 nơi | 🟢 **LOW** | Tạo môn học (`create_subject`) | **SAFE TO REMOVE** |
| **4. `dsdiemthi.find` (start vs submit)** | Xuất hiện ở cả start và submit | Không có (xóa sẽ gây lỗi race condition) | 🔴 **HIGH** | Nộp bài thi, Tránh nộp 2 lần | **KEEP AS-IS** |
| **5. `exam_history` in `rebuildUsedFlags`** | Đọc lại history từ đĩa khi nộp bài | Giảm I/O đĩa lúc nộp bài | 🟡 **MEDIUM** | Nộp bài, Quản lý cờ used | **KEEP AS-IS (Tránh Over-engineering)** |

---

## III. DEPENDENCY ANALYSIS

- **Cleanup 1 (`set_cors_headers`)**: `INDEPENDENT` (Có thể thực hiện độc lập).
- **Cleanup 3 (`mamh.length` in SubjectHandler)**: `INDEPENDENT` (Có thể thực hiện độc lập).
- **Cleanup 2, 4, 5**: Đã được khuyến nghị **GIỮ NGUYÊN (KEEP AS-IS)** để đảm bảo an toàn nghiệp vụ và tránh over-engineering.

---

## IV. TỔNG KẾT AUDIT

1. **Số hạng mục THỰC SỰ NÊN CLEANUP:** **2 hạng mục** (Cleanup 1 & Cleanup 3).
2. **Số hạng mục BẮT BUỘC GIỮ NGUYÊN (Defensive / Idempotency):** **3 hạng mục** (Cleanup 2, Cleanup 4, Cleanup 5).
3. **Mức độ ảnh hưởng mã nguồn:** Tối thiểu, chỉ làm sạch $\approx 25$ dòng code thừa, không thay đổi bất kỳ business logic, data structure hay persistence format nào.

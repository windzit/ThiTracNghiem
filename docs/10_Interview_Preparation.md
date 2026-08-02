# 10. Bộ Câu hỏi & Trả lời Vấn đáp Đồ án (Interview Preparation)

Tài liệu này tổng hợp các câu hỏi thi vấn đáp chuyên sâu mà Hội đồng phản biện/Giảng viên thường hỏi, kèm theo câu trả lời chuẩn xác bám sát codebase hiện tại.

---

## 📌 Chủ đề 1: Cấu trúc Dữ liệu & Giải thuật

### ❓ Câu 1: Tại sao dùng Cây BST quản lý Môn học, không dùng Mảng hay Danh sách liên kết?

**Trả lời tốt:**
- Môn học được **tìm kiếm rất thường xuyên** (mỗi lần tạo đề, nhập câu hỏi, xem bảng điểm đều cần tra theo MAMH).
- BST tìm kiếm đạt $O(\log N)$ trung bình, trong khi mảng hay danh sách liên kết tốn $O(N)$.
- Duyệt **In-order** (Trái → Gốc → Phải) tự động xuất danh sách môn học đã **sắp xếp A-Z theo MAMH** mà không cần thêm chi phí sắp xếp.

### ❓ Câu 2: Tại sao Danh sách Lớp dùng Mảng Con trỏ cố định 10,000 phần tử?

**Trả lời tốt:**
- Theo đề bài, tối đa 10,000 lớp. Mảng con trỏ `Lop* dslop[10000]` chỉ chiếm `10,000 × 8 bytes = 80KB RAM` khi chưa tạo lớp nào (tất cả `nullptr`).
- Bộ nhớ cho `Lop` thực sự chỉ được cấp phát khi `new Lop()` → Tiết kiệm hơn cấp phát liên tục.
- Truy cập lớp thứ `i` bằng `dslop[i]`: $O(1)$.

### ❓ Câu 3: Em giải thích thuật toán xóa nút trên Cây BST Môn học?

**Trả lời tốt:**
Xóa một nút có 3 trường hợp:
1. **Nút lá** (không có con): Giải phóng bộ nhớ, gán con trỏ cha về `nullptr`.
2. **Nút 1 con**: Cho con trỏ cha trỏ thẳng đến con duy nhất, giải phóng nút cần xóa.
3. **Nút 2 con**: Tìm **In-order Successor** (nút trái nhất của cây con bên phải — nút nhỏ nhất lớn hơn nút cần xóa), copy dữ liệu vào nút cần xóa, rồi đệ quy xóa Successor (Successor chỉ có 0 hoặc 1 con phải).

### ❓ Câu 4: Bảng Băm `HashTable` của em triển khai như thế nào?

**Trả lời tốt:**
- **Open Addressing + Linear Probing**: Khi xảy ra đụng độ (hai key hash về cùng slot), thuật toán kiểm tra slot tiếp theo `(index + 1) % capacity` tuần tự cho đến khi tìm được slot trống.
- **Ưu điểm**: Không cần cấp phát thêm bộ nhớ cho linked list trong slot — toàn bộ dữ liệu nằm trong một mảng liên tục, cache-friendly.
- **Độ phức tạp**: $O(1)$ trung bình, $O(N)$ xấu nhất (khi bảng gần đầy).

### ❓ Câu 5: Danh sách sinh viên được chèn theo thứ tự gì? Tại sao?

**Trả lời tốt:**
- Chèn giữ thứ tự **TÊN rồi đến HỌ** (ví dụ: "An Le" trước "Binh Nguyen").
- Lý do: Khi xuất bảng điểm hay danh sách lớp, dữ liệu đã sắp xếp sẵn — không cần sort thêm khi query → tiết kiệm thời gian xử lý.

---

## 📌 Chủ đề 2: Lưu trữ TXT + IDX (Storage Engine)

### ❓ Câu 6: Định dạng Fixed-Length Records là gì? Lợi ích chính?

**Trả lời tốt:**
- Mỗi dòng/bản ghi có số bytes **chính xác bằng nhau** (`RECORD_SIZE`): 69 bytes (Lớp), 134 bytes (SV), 71 bytes (Môn), 736 bytes (Câu hỏi).
- Các trường được đệm khoảng trắng (`' '`) đến đúng độ dài.
- **Lợi ích**: Vị trí bất kỳ bản ghi thứ $K$ = `HEADER_LEN + (K-1) × RECORD_SIZE` → Nhảy thẳng không cần đọc tuần tự → $O(1)$.

### ❓ Câu 7: Tại sao cờ status lại đặt ở cuối bản ghi?

**Trả lời tốt:**
- Cờ `STATUS` ở byte áp chót (vị trí `STATUS_OFFSET`): `132` với SV, `734` với câu hỏi.
- Khi thực hiện "xóa mềm", chỉ cần `file.seekp(offset + STATUS_OFFSET)` rồi ghi **đúng 1 byte `'1'`** — không đụng chạm bất kỳ byte nào khác trong bản ghi.
- Nếu đặt ở đầu, việc ghi đè 1 byte có thể ảnh hưởng đến dữ liệu lân cận.

### ❓ Câu 8: Có 3 giá trị cờ status. Phân biệt `'0'`, `'1'`, `'2'`?

**Trả lời tốt:**
- `'0'` = **ACTIVE**: Bản ghi đang hoạt động — được nạp vào RAM, hiển thị trên web.
- `'1'` = **DELETED**: Đã xóa mềm, chờ Compaction Engine dọn sạch khỏi file `.txt`.
- `'2'` = **SOFT_DELETED** (chỉ cho câu hỏi): Câu hỏi đã bị xóa nhưng đã từng được thi — **Compaction giữ lại** để tra cứu lịch sử bài thi, nhưng không bao giờ đưa vào đề thi mới.

### ❓ Câu 9: File `.idx` có vai trò gì? Nếu bị xóa mất thì sao?

**Trả lời tốt:**
- File `.idx` lưu bản đồ `Key → Byte Offset` dưới dạng nhị phân. Khi Server khởi động, toàn bộ `.idx` được nạp vào **HashTable RAM** để tra cứu $O(1)$.
- Nếu file `.idx` bị mất/hỏng: Hàm `IndexManager::auditAndLoadIndexes()` phát hiện thiếu file → Tự động **Rebuild từ đầu bằng cách quét `.txt` gốc** một lần duy nhất → Lưu lại `.idx` mới. Server vẫn hoạt động bình thường.

### ❓ Câu 10: Compaction Engine hoạt động như thế nào? Khi nào kích hoạt?

**Trả lời tốt:**
- **Ngưỡng kích hoạt** (từ `StorageConfig`): 50 SV xóa mềm, 50 câu hỏi, 20 môn, 10 lớp.
- **Hai thời điểm kích hoạt**:
  1. Lúc khởi động Server: `checkAndExecuteStartupCompaction()` đếm bản ghi `'1'` trong `.txt`, nếu đạt ngưỡng → Compact trước khi nạp dữ liệu.
  2. Lúc đang chạy: Mỗi lần `incrementDeletedCount()` tăng bộ đếm vượt ngưỡng → Compact ngay lập tức.
- **Quy trình Compact**: Đọc `.txt` → Tạo nội dung mới (bỏ dòng `'1'`) → `atomicWriteFile()` (ghi tạm → rename) → Rebuild `.idx`.

---

## 📌 Chủ đề 3: Server, Đa luồng & An toàn Bộ nhớ

### ❓ Câu 11: Nhiều sinh viên nộp bài cùng lúc, hệ thống xử lý không bị lộn xộn?

**Trả lời tốt:**
- Dùng **Read-Write Lock** (`std::shared_mutex g_dbMutex`):
  - Đọc (`GET`): Dùng `DB_READ_LOCK` (`shared_lock`) → Nhiều luồng đọc đồng thời, không chặn nhau.
  - Ghi/Nộp bài (`POST /exams/submit`): Dùng `DB_WRITE_LOCK` (`unique_lock`) → Tại 1 thời điểm, chỉ 1 luồng được ghi, tất cả luồng khác phải xếp hàng chờ.
- **RAII pattern**: Lock tự động giải phóng khi ra khỏi scope `{}` → Không bao giờ bị deadlock do quên mở khóa.

### ❓ Câu 12: Làm thế nào đảm bảo không bị Rò rỉ Bộ nhớ (Memory Leak)?

**Trả lời tốt:**
- Tất cả class tuân thủ **Rule of Three**: Triển khai đầy đủ **Constructor**, **Destructor** đệ quy giải phóng tất cả node con trỏ, và **Copy Constructor** (Deep Copy).
- Khi `Server` tắt, Destructor của `dsl` gọi `delete` từng `Lop*`, từng `dsSinhVien*`, từng `dsDiemThi*` đệ quy → Không còn bộ nhớ bị rò rỉ.

### ❓ Câu 13: `atomicWriteFile` hoạt động thế nào? Tại sao cần thiết?

**Trả lời tốt:**
- Ghi ra file tạm `*.txt.tmp` trước → File gốc vẫn còn nguyên.
- Sau khi ghi xong → `fs::rename(tmpFile, targetFile)` — Hệ điều hành thực hiện rename **atomic** (không thể bị cắt nửa chừng).
- Nếu Server crash lúc đang Compact: File gốc `.txt` vẫn nguyên vẹn, chỉ mất file `.tmp` vô hại.

---

## 📌 Chủ đề 4: Quy tắc Nghiệp vụ

### ❓ Câu 14: Ràng buộc xóa của hệ thống là gì?

**Trả lời tốt:**

| Đối tượng | Điều kiện để được xóa |
| :--- | :--- |
| Xóa **Lớp** | `dssinhvien == nullptr` (Lớp không có sinh viên) |
| Xóa **Sinh viên** | `dsdiemthi.empty() == true` (SV chưa thi môn nào) |
| Xóa **Môn học** | `monhoc.used == false` (Môn chưa có bài thi nào diễn ra) |
| Xóa **Câu hỏi** | Không có ràng buộc nhưng phân nhánh Hard/Soft Delete dựa trên `used` |

### ❓ Câu 15: Sinh viên tắt máy giữa chừng đang thi thì xử lý thế nào?

**Trả lời tốt:**
- Mỗi khi chọn đáp án: `PUT /api/exams/answer` → Đáp án được ghi ngay vào `exam_sessions.txt`.
- Khi SV mở lại trình duyệt: `GET /api/exams/resume` → Server đọc session từ đĩa, tính `remainingSeconds = (thoiGianPhut × 60) - (now - batDau)` → Phục hồi chính xác thời gian còn lại và đáp án đã làm.
- Nếu `remainingSeconds <= 0`: Server tự động nộp bài quá giờ.

### ❓ Câu 16: ID câu hỏi được tạo ra như thế nào? Tại sao không trùng?

**Trả lời tốt:**
- `StorageManager::getNextQuestionID()` đọc `cachedLastQuestionId` từ `metadata.txt`, tăng lên 1 rồi ghi lại vào `metadata.txt`.
- `cachedLastQuestionId` luôn tăng đơn điệu — kể cả câu hỏi bị xóa, ID cũ không bao giờ được tái sử dụng.
- Vì được bao bởi `DB_WRITE_LOCK` → Thread-safe, không có race condition khi 2 người thêm câu hỏi cùng lúc.

### ❓ Câu 17: Tại sao File `.idx` lưu dạng nhị phân thay vì text?

**Trả lời tốt:**
- Dạng nhị phân: Một `int64_t` (Byte Offset) chỉ chiếm **8 bytes**.
- Dạng text: `"40480"` chiếm **5 bytes** nhưng cần parse/convert → Tốn CPU.
- Với 1 triệu bản ghi: Binary `.idx` ≈ `8MB`, Text `.idx` ≈ `15-20MB` + chi phí parse.
- Đọc binary trực tiếp vào memory bằng `fread()` → Tối đa hóa tốc độ nạp khởi động.

---

## 📌 Chủ đề 5: Kiểm định & Chuẩn hóa Dữ liệu (Validation & Normalization)

### ❓ Câu 18: Tại sao hệ thống lại dùng cơ chế kiểm định 2 tầng (FE + BE)?

**Trả lời tốt:**
- **Tầng 1 (Frontend)**: Phản hồi tức thì cho người dùng trên giao diện web (nhập sai mã hay tên báo đỏ ngay), giúp trải nghiệm mượt mà không tốn network round-trip.
- **Tầng 2 (Backend)**: Đảm bảo **bảo mật và toàn vẹn dữ liệu đĩa**. Người dùng có thể bypass Frontend (gửi request qua Postman/curl), do đó Backend bắt buộc phải tái kiểm định ký tự cấm, độ dài và tra trùng trước khi ghi file `.txt`.

### ❓ Câu 19: Quy tắc chuẩn hóa giữa Nội dung câu hỏi và Phương án chọn khác nhau như thế nào?

**Trả lời tốt:**
- **Nội dung câu hỏi (`NOIDUNG`)**: Chỉ trim và collapse space thừa, **GIỮ NGUYÊN hoa/thường** nguyên bản của thầy cô nhập (vì câu hỏi trắc nghiệm có thể chứa từ viết hoa chuyên ngành, công thức...).
- **Phương án chọn (`A`, `B`, `C`, `D`)**: Dùng **Sentence Case** (chỉ tự động viết hoa chữ cái đầu tiên của từ đầu tiên trong câu) và tự động kiểm tra trùng lặp bằng cách biến cả 4 options về chữ thường (`std::tolower`) để phát hiện hai phương án giống hệt nhau.


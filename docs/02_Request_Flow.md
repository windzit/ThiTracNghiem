# 02. Luồng Yêu cầu Dữ liệu (Request Flow)

Tài liệu này mô tả chi tiết luồng xử lý dữ liệu truyền từ tầng **Frontend (React)** qua **REST API**, tới **C++ Web Server**, xuống **Cấu trúc dữ liệu Core (Memory)** và đồng bộ lưu đĩa **Storage TXT + IDX**.

---

## 🗺️ 1. Sơ đồ Luồng Yêu cầu Tổng thể (End-to-End)

```
[1. User Interaction]
        │ (Người dùng nhấn nút / Submit form)
        ▼
[2. Frontend Service Layer]
        │ (Gọi axios: api.post('/api/questions', data))
        ▼
[3. HTTP REST Request over TCP/IP]
        │ (Payload JSON qua HTTP/1.1 Header & Body tới cổng 8080)
        ▼
[4. C++ Web Server (httplib::Server)]
        │ (Thread Pool nhận request, match URL trong RouteRegistry)
        ▼
[5. Route Handler (e.g. QuestionHandler)]
        │ (Parse JSON → Kiểm tra Auth → Acquire DB_WRITE_LOCK)
        ▼
[6. C++ Core Data Structure & Domain Operations]
        │ (Thao tác trên RAM: BST NodeMH / dsLop / dsSinhVien / dsCHT)
        ▼
[7. Storage Engine & Index Accelerator]
        │ (StorageManager::append/write/mark + IndexManager::update)
        ▼
[8. JSON Response → Frontend re-render UI]
```

---

## 🔬 2. Giải phẫu Chi tiết Từng Bước

### Bước 1 & 2: Frontend gọi API

**Tệp liên quan**: `frontend/src/shared/api/api.ts`, các `*Service.ts`

- Frontend **KHÔNG BAO GIỜ** gọi API trực tiếp từ UI component — luôn thông qua Service Module.
- `api.ts` khởi tạo Axios Client với:
  - `baseURL: 'http://localhost:8080'`
  - **Response Interceptor**: Tự động bóc tách envelope `{ success: true, data: ... }` → trả thẳng `data` cho caller.
  - **Error Interceptor**: Bắt lỗi `status >= 502` → Kích hoạt màn hình Reconnect.

```typescript
// Ví dụ service gọi tạo câu hỏi
const createQuestion = (data) => api.post('/api/questions', data);
// Interceptor tự unwrap: caller nhận { id: 105, mamh: "CTDL", ... }
// thay vì { success: true, data: { id: 105, ... } }
```

### Bước 3 & 4: Transport & Server Routing

**Tệp liên quan**: `server/RouteRegistry.cpp`, `server/ServerBootstrap.cpp`

```
HTTP POST /api/questions
   Header: Content-Type: application/json
   Body:   { "mamh": "CTDL", "noidung": "BST là gì?", ... }

→ httplib::Server nhận tại cổng 8080
→ RouteRegistry khớp: POST /api/questions → handle_create_question()
→ CORS Headers tự động thêm: Access-Control-Allow-Origin: *
```

### Bước 5 & 6: Handler & Đồng bộ Đa luồng

**Tệp liên quan**: `server/handlers/*.cpp`, `server/ServerContext.h`

1. Trích xuất tham số từ URL hoặc JSON body.
2. Khóa tài nguyên shared state:

```cpp
// Đọc: Cho phép nhiều luồng đọc đồng thời
DB_READ_LOCK;   // = std::shared_lock<std::shared_mutex> _rl(g_dbMutex)

// Ghi: Chỉ 1 luồng được ghi, tất cả phải chờ
DB_WRITE_LOCK;  // = std::unique_lock<std::shared_mutex> _wl(g_dbMutex)
```

3. Gọi các phương thức trên Cấu trúc Dữ liệu C++ RAM (`insert`, `remove`, `find`...).

### Bước 7: Storage Engine (TXT + IDX)

**Tệp liên quan**: `src/StorageManager.cpp`, `src/IndexManager.cpp`

Sau khi RAM đã được cập nhật → Đồng bộ xuống đĩa ngay lập tức:
- Ghi bản ghi Fixed-Length 736 bytes vào `questions.txt`.
- Gọi `IndexManager::updateQuestionOffset(id, offset)` → Cập nhật `question.idx`.

### Bước 8: JSON Response

Tất cả response đều theo cấu trúc chuẩn:
```json
// Thành công
{ "success": true, "data": { ... } }

// Thất bại
{ "success": false, "message": "Mã sinh viên đã tồn tại!" }
```

---

## 💡 3. Ví dụ Minh họa Đầy đủ: Thêm Câu hỏi Thi

```
[1] Client gửi:
    POST http://localhost:8080/api/questions
    Body: { "mamh": "CTDL", "noidung": "BST là gì?", "a": "Cây...",
            "b": "Mảng...", "c": "...", "d": "...", "dapan": "A" }

[2] RouteRegistry → handle_create_question()

[3] QuestionHandler.cpp:
    │  DB_WRITE_LOCK
    │  NodeMH* mhNode = find_subject_smart("CTDL");       ← Tra HashTable O(1)
    │  if (!mhNode) → error_response(res, "Môn không tồn tại", 404)
    │
    │  // Kiểm tra trùng nội dung câu hỏi
    │  if (dsCauHoi.findByContent(noidung)) → error_response(..., 422)
    │
    │  CauHoi ch;
    │  ch.ID = StorageManager::getInstance().getNextQuestionID(); // → 201
    │  ch.NOIDUNG = noidung; ch.A = a; ... ch.DAPAN_DUNG = 'A';
    │  ch.used = false; ch.deleted = false;
    │
    │  mhNode->data.dsCauHoi.insert(ch);                  ← Chèn vào RAM

[4] StorageManager::appendQuestion(ch, "CTDL", outOffset):
    │  Format: "CTDL           |0000000201|BST là gì?..." (736 bytes cố định)
    │  file.seekp(0, ios::end) → vị trí cuối file = 147200
    │  Ghi 736 bytes → outOffset = 147200

[5] IndexManager::updateQuestionOffset(201, 147200):
    │  m_questionIndex.insert(201, 147200)    ← HashTable RAM
    │  saveQuestionIndex() → question.idx (binary)

[6] Response 201:
    { "id": 201, "mamh": "CTDL", "noidung": "BST là gì?",
      "a": "Cây...", "b": "Mảng...", ..., "dapan": "A",
      "used": false, "deleted": false }
```

---

## ⚡ 4. Tốc độ Xử lý: Tại sao lại nhanh?

```
💡 Bí quyết tốc độ = Kết hợp 3 kỹ thuật:

1. RAM-FIRST: Mọi thao tác đọc đều từ RAM (microseconds)
   → GET /api/students: Duyệt dsSinhVien trên RAM, không chạm đĩa

2. DIRECT SEEK: Thao tác ghi nhảy thẳng đến vị trí byte chính xác (O(1))
   → DELETE /api/students/N21001: file.seekp(offset + 132) → ghi 1 byte '1'

3. HASHTABLE LOOKUP: Tra cứu offset từ Key trong O(1) bằng Bảng Băm
   → Từ "N21001" → offset = 26800 trong vài nanosecond
```

---

## 📁 5. Các File Mã Nguồn Liên quan

| File | Nhiệm vụ |
| :--- | :--- |
| [api.ts](file:///frontend/src/shared/api/api.ts) | Axios API Client tích hợp interceptors |
| [ServerContext.h](file:///server/ServerContext.h) | Khai báo `dsl`, `dsmh`, `g_dbMutex`, macros `DB_READ_LOCK`/`DB_WRITE_LOCK` |
| [RouteRegistry.cpp](file:///server/RouteRegistry.cpp) | Đăng ký tất cả REST API endpoints |
| [QuestionHandler.cpp](file:///server/handlers/QuestionHandler.cpp) | Ví dụ handler xử lý câu hỏi |
| [StorageManager.cpp](file:///src/StorageManager.cpp) | Quản lý lưu trữ ghi đĩa Fixed-Length |
| [IndexManager.cpp](file:///src/IndexManager.cpp) | Quản lý HashTable chỉ mục RAM + file `.idx` |

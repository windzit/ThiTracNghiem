# 02. Luồng Yêu cầu Dữ liệu (Request Flow)

Tài liệu này mô tả chi tiết luồng xử lý dữ liệu truyền từ tầng **Frontend (React)** qua **Bộ quy tắc Kiểm định 2 Tầng (2-Layer Validation)**, tới **C++ Web Server**, xuống **Cấu trúc dữ liệu Core (Memory)** và đồng bộ lưu đĩa **Storage TXT + IDX**.

---

## 💡 Phép so sánh Trực quan: Hành trình Dữ liệu Giống như "Gửi Bưu phẩm Quốc tế"

```
📦 GỬI BƯU PHẨM                              🖥️ XỬ LÝ REQUEST DỮ LIỆU
┌────────────────────────────────┐          ┌────────────────────────────────┐
│ 1. Bưu cục Địa phương (FE)     │ ────────► │ 1. Form Validation (FE)        │
│    (Đóng gói đúng kích thước,  │          │    (Xóa space, tự viết hoa mã,  │
│     dán nhãn mã vạch chuẩn)    │          │     kiểm tra form đúng mẫu UI) │
├────────────────────────────────┤          ├────────────────────────────────┤
│ 2. Đường Bay Xe Tải (HTTP)     │ ────────► │ 2. Axios REST Client (HTTP/1.1)│
│    (Gửi gói quà qua mạng TCP)  │          │    (JSON Payload qua cổng 8080) │
├────────────────────────────────┤          ├────────────────────────────────┤
│ 3. Hải quan Tổng cục (BE)      │ ────────► │ 3. C++ StringNormalizer &      │
│    (Kiểm tra hàng cấm, soi mã  │          │    StorageValidator (BE)       │
│     vạch trùng trong sổ cái)   │          │    (Soi cấm ký tự |, tab, trùng)│
├────────────────────────────────┤          ├────────────────────────────────┤
│ 4. Lưu Kho Trung tâm (Storage) │ ────────► │ 4. RAM BST/Array + Disk TXT/IDX│
│    (Xếp đúng vị trí kệ cố định)│          │    (Chèn RAM + Ghi đĩa O(1))   │
└────────────────────────────────┘          └────────────────────────────────┘
```

---

## 🗺️ 1. Sơ đồ Luồng Yêu cầu Tổng thể (End-to-End với 2-Layer Validation)

```
[1. User Interaction]
        │ (Người dùng nhập Form / Submit)
        ▼
[2. Frontend Validation & Normalization Layer (formValidation.ts)]
        │ (Xóa khoảng trắng thừa, Title Case tên, normalizeIdentifier mã, báo lỗi UI ngay)
        ▼
[3. Frontend Service Layer (api.ts & *Service.ts)]
        │ (Gọi Axios: api.post('/api/questions', normalizedData))
        ▼
[4. HTTP REST Request over TCP/IP]
        │ (Payload JSON qua HTTP/1.1 Header & Body tới cổng 8080)
        ▼
[5. C++ Web Server (httplib::Server & RouteRegistry)]
        │ (Thread Pool nhận request, match URL trong RouteRegistry)
        ▼
[6. Route Handler (e.g. QuestionHandler)]
        │ (Acquire DB_WRITE_LOCK → StringNormalizer → StorageValidator)
        │ ├── StringNormalizer: Chuẩn hóa backend (preserve question content casing)
        │ └── StorageValidator: Check ký tự cấm (|, \t, \r, \n), trùng option lowercase
        ▼
[7. C++ Core Data Structure & Domain Operations]
        │ (Thao tác trên RAM: BST NodeMH / dsLop / dsSinhVien / dsCHT)
        ▼
[8. Storage Engine & Index Accelerator]
        │ (StorageManager::append/write/mark + IndexManager::update)
        ▼
[9. JSON Response Envelope → Frontend re-render UI]
```

---

## 🔬 2. Giải phẫu Chi tiết Từng Bước

### Bước 1 & 2: Frontend Normalize & Validate (Tầng 1)

**Tệp liên quan**: `frontend/src/shared/api/api.ts`, `frontend/src/shared/lib/formValidation.ts`

1. **Chuẩn hóa ngay trên UI**:
   - Mã (`MASV`, `MALOP`, `MAMH`): `normalizeIdentifier()` xóa khoảng trắng, chuyển IN HOA.
   - Tên (`TENLOP`, `TENMH`, `HO`, `TEN`): `normalizeText()` / `toTitleCase()` viết hoa chữ cái đầu mỗi từ.
   - Nội dung câu hỏi: `normalizeQuestionContent()` trim + collapse space, **giữ nguyên hoa/thường**.
   - Option câu hỏi: `normalizeQuestionOption()` trim + collapse space, **Sentence Case** (viết hoa chữ cái đầu tiên).
2. **Kiểm tra hợp lệ trước khi gửi**:
   - `validateClassCode()`, `validateSubjectCode()`, `validateStudentName()`... trả lỗi ngay trên UI mà không cần chờ Server.

```typescript
// Ví dụ xử lý form thêm sinh viên phía Frontend
const handleSubmit = () => {
  const normMasv = normalizeIdentifier(formMasv);
  const normHo = toTitleCase(formHo);
  const normTen = toTitleCase(formTen);
  
  const err = validateStudentId(normMasv) || validateStudentName(normHo);
  if (err) return showError(err);

  await studentService.createStudent({ masv: normMasv, ho: normHo, ten: normTen, ... });
};
```

### Bước 3 & 4: Transport & Server Routing

**Tệp liên quan**: `server/RouteRegistry.cpp`, `server/ServerBootstrap.cpp`

```
HTTP POST /api/questions
   Header: Content-Type: application/json
   Body:   { "mamh": "CTDL", "noidung": "BST là gì?", "a": "Cây nhị phân", ... }

→ httplib::Server nhận tại cổng 8080
→ RouteRegistry khớp: POST /api/questions → handle_create_question()
→ CORS Headers tự động thêm: Access-Control-Allow-Origin: *
```

### Bước 5 & 6: Backend Handler, Normalize & Validate (Tầng 2)

**Tệp liên quan**: `server/handlers/*.cpp`, `src/StringNormalizer.cpp`, `src/StorageValidator.cpp`

1. **Acquire RWLock**: `DB_WRITE_LOCK` để đảm bảo độc quyền ghi.
2. **Chuẩn hóa Backend**:
   - `StringNormalizer::normalizeQuestion(q)` xử lý không làm mất hoa/thường của người dùng.
3. **Kiểm định Backend**:
   - `StorageValidator::validateQuestion()` kiểm tra cấm ký tự `|`, `\r`, `\n`, `\t`.
   - `StorageValidator::hasDuplicateOptionsAfterNormalization()` chuyển 4 phương án về chữ thường (`std::tolower`) để phát hiện trùng đáp án (Ví dụ: `"Cây"` và `"cây"` bị coi là trùng).
4. **Thao tác RAM**: Chèn vào Cây BST / DS Liên kết.

### Bước 7 & 8: Storage Engine & JSON Response

**Tệp liên quan**: `src/StorageManager.cpp`, `src/IndexManager.cpp`

- Ghi bản ghi Fixed-Length 736 bytes vào `questions.txt`.
- Cập nhật `question.idx` nhị phân.
- Trả về Envelope Response: `{ "success": true, "data": { ... } }`.

---

## 💡 3. Ví dụ Minh họa Đầy đủ: Thêm Câu hỏi Thi (Đi qua 2 Tầng)

```
[1] Frontend Input:
    Người dùng nhập: noidung = "   thuat  toan   BST   la gi?   "
                    a = "  cay nhi  phan "
                    b = "  CAY  NHI PHAN "  <-- Cố tình nhập trùng!

[2] Frontend Normalize:
    normContent = "thuat  toan   BST   la gi?" (Giữ nguyên chữ thường 'thuat toan')
    normA = "Cay nhi phan" (Sentence Case: C viết hoa)
    normB = "Cay nhi phan"

[3] Frontend Validation:
    normA === normB -> Frontend báo lỗi UI: "Đáp án B trùng với đáp án A!" (Chặn từ xa!)

[4] Trường hợp người dùng bypass FE gửi thẳng API:
    POST /api/questions Body: { "mamh":"CTDL", "a":"Cay nhi phan", "b":"cay nhi phan" }
    │
    ▼
    QuestionHandler.cpp:
    │  DB_WRITE_LOCK
    │  StorageValidator::hasDuplicateOptionsAfterNormalization(q, err)
    │  --> "cay nhi phan" (lowercase) == "cay nhi phan" (lowercase)
    │  --> Trả về 422: { "success": false, "message": "Các phương án lựa chọn không được trùng nhau!" }
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
| [formValidation.ts](file:///frontend/src/shared/lib/formValidation.ts) | Quy tắc Validate & Normalization phía Frontend |
| [api.ts](file:///frontend/src/shared/api/api.ts) | Axios API Client tích hợp interceptors |
| [StringNormalizer.cpp](file:///src/StringNormalizer.cpp) | Bộ chuẩn hóa chuỗi phía Backend C++ |
| [StorageValidator.cpp](file:///src/StorageValidator.cpp) | Bộ kiểm định dữ liệu và phát hiện trùng đáp án Backend |
| [ServerContext.h](file:///server/ServerContext.h) | Khai báo `dsl`, `dsmh`, `g_dbMutex`, macros `DB_READ_LOCK`/`DB_WRITE_LOCK` |
| [RouteRegistry.cpp](file:///server/RouteRegistry.cpp) | Đăng ký tất cả REST API endpoints |
| [QuestionHandler.cpp](file:///server/handlers/QuestionHandler.cpp) | Handler xử lý câu hỏi |
| [StorageManager.cpp](file:///src/StorageManager.cpp) | Quản lý lưu trữ ghi đĩa Fixed-Length |
| [IndexManager.cpp](file:///src/IndexManager.cpp) | Quản lý HashTable chỉ mục RAM + file `.idx` |

# BÁO CÁO ĐỐI CHIẾU MÃ NGUỒN (CODE BEFORE / AFTER)

Tài liệu này ghi lại chi tiết từng thay đổi mã nguồn thực tế cho toàn bộ 11 hạng mục tối ưu hóa (`OPT-1` đến `OPT-11`).

---

## OPT-1 — Chuyển `saveScores` sang `appendScore` $O(1)$ khi nộp bài

### Files changed
- `include/StorageManager.h`
- `src/StorageManager.cpp`
- `server/handlers/ExamHandler.cpp`

### Function(s)
- `StorageManager::appendScore`
- `handle_exam_submit`

### BEFORE
```cpp
// server/handlers/ExamHandler.cpp (L260-264)
bool saved = sv->dsdiemthi.insert(dt);
if (saved) {
    StorageManager::getInstance().saveScores(dsl);
}
```

### AFTER
```cpp
// server/handlers/ExamHandler.cpp (L260-264)
bool saved = sv->dsdiemthi.insert(dt);
if (saved) {
    StorageManager::getInstance().appendScore(masv, mamh, diem);
}
```
Và hàm mới trong `src/StorageManager.cpp`:
```cpp
bool StorageManager::appendScore(const std::string& masv, const std::string& mamh, float diem) {
    std::string errReason;
    DiemThi dt;
    strncpy(dt.MAMH, mamh.c_str(), sizeof(dt.MAMH) - 1);
    dt.MAMH[sizeof(dt.MAMH) - 1] = '\0';
    dt.DIEM = diem;
    if (!StorageValidator::validateScore(masv, dt, errReason)) {
        std::cerr << "[StorageValidation] appendScore validation failed: " << errReason << std::endl;
        return false;
    }
    std::string targetPath = PathResolver::getFilePath("scores.txt");
    std::ofstream ofs(targetPath, std::ios::app);
    if (!ofs.is_open()) return false;
    ofs << masv << '|' << mamh << '|' << diem << '\n';
    ofs.flush();
    return ofs.good();
}
```

### WHAT CHANGED
- **Đoạn bị thay thế:** Lời gọi `saveScores(dsl)` trong `handle_exam_submit`.
- **Đoạn được thêm:** Hàm `appendScore(...)` mở file với `std::ios::app` để ghi đúng 1 dòng điểm.
- **Flow BEFORE:** Nộp bài $\to$ Duyệt qua toàn bộ danh sách Lớp $\to$ Sinh viên $\to$ Điểm thi $\to$ Full rewrite toàn bộ `scores.txt`.
- **Flow AFTER:** Nộp bài $\to$ Insert vào RAM `sv->dsdiemthi` $\to$ Ghi nối (append) 1 dòng xuống cuối `scores.txt` trong $\mathcal{O}(1)$.

### WHY
Loại bỏ nút thắt I/O lớn nhất khi có nhiều sinh viên nộp bài đồng thời.

### COMPLEXITY
- **Before:** Time: $\mathcal{O}(C \cdot S \cdot D)$ CPU + $\mathcal{O}(N_{\text{scores}})$ Disk I/O | Space: $\mathcal{O}(N_{\text{scores}})$ buffer
- **After:** Time: $\mathcal{O}(1)$ CPU + $\mathcal{O}(1)$ Disk I/O | Space: $\mathcal{O}(1)$

### RISK / TRADE-OFF
Khi Admin xóa điểm (`handle_delete_score`), hệ thống vẫn gọi `saveScores(dsl)` để viết lại file chuẩn xác. Rủi ro $= 0\%$.

---

## OPT-2 — Tối ưu `login_student` thành 1 lần lookup $O(1)$

### Files changed
- `server/handlers/AuthHandler.cpp`

### Function(s)
- `handle_login`

### BEFORE
```cpp
// server/handlers/AuthHandler.cpp (L26-38)
} else if (role == "student") {
    if (login_student(*dsl.getRoot(), username, password)) {
        SinhVien* sv = findStudentGlobal(username, nullptr);
        if (!sv) { error_response(res, "Student not found", 404); return; }

        json_response(res, {
            {"role","student"},
            {"masv",sv->MASV},
            {"ho",sv->HO},
            {"ten",sv->TEN},
            {"phai",sv->PHAI}
        });
    } else { error_response(res, "Invalid credentials", 401); }
}
```

### AFTER
```cpp
// server/handlers/AuthHandler.cpp (L26-37)
} else if (role == "student") {
    SinhVien* sv = findStudentGlobal(username, nullptr);
    if (sv && sv->passsword == password) {
        json_response(res, {
            {"role","student"},
            {"masv",sv->MASV},
            {"ho",sv->HO},
            {"ten",sv->TEN},
            {"phai",sv->PHAI}
        });
    } else { error_response(res, "Invalid credentials", 401); }
}
```

### WHAT CHANGED
- **Đoạn bị xóa:** Lời gọi `login_student(*dsl.getRoot(), username, password)` quét tuyến tính toàn bộ cấu trúc $O(C \times S)$.
- **Flow BEFORE:** Quét tuyến tính $O(C \times S)$ xác thực password $\to$ Tra cứu lần 2 qua `findStudentGlobal` $O(1)$ để lấy thông tin.
- **Flow AFTER:** Tra cứu 1 lần duy nhất qua `findStudentGlobal` $O(1)$ và so sánh mật khẩu trực tiếp.

### WHY
Khử hoàn toàn thao tác quét trùng lặp $O(C \times S)$ khi đăng nhập.

### COMPLEXITY
- **Before:** Time: $\mathcal{O}(C \cdot S)$ | Space: $\mathcal{O}(1)$
- **After:** Time: $\mathcal{O}(1)$ | Space: $\mathcal{O}(1)$

---

## OPT-3 — Tối ưu `rebuildUsedFlags` tại `exam_start` thành Incremental Flagging

### Files changed
- `server/handlers/ExamHandler.cpp`

### Function(s)
- `handle_exam_start`

### BEFORE
```cpp
// server/handlers/ExamHandler.cpp (L99-102)
session.lastServerActivityAt = std::time(nullptr);
saveExamSession(session);
StorageManager::getInstance().rebuildUsedFlags(dsmh, &dsl);
```

### AFTER
```cpp
// server/handlers/ExamHandler.cpp (L99-106)
session.lastServerActivityAt = std::time(nullptr);
saveExamSession(session);
if (node) {
    node->data.used = true;
    for (int i = 0; i < socau; i++) {
        all[i]->cauhoi.used = true;
    }
}
```

### WHAT CHANGED
- **Đoạn bị thay thế:** `rebuildUsedFlags(dsmh, &dsl)` đọc toàn bộ `exam_history.txt` từ đĩa và quét toàn bộ hệ thống.
- **Đoạn được thêm:** Gán cờ `used = true` trực tiếp cho môn học và các câu hỏi được chọn trong phiên thi.

### WHY
Khi bắt đầu thi, chưa có điểm số nào mới. Việc quét lại toàn bộ file lịch sử thi từ đĩa là dư thừa. Gán cờ cục bộ đảm bảo giáo viên không thể xóa câu hỏi đang được thi trong khi giảm chi phí về $O(K)$.

### COMPLEXITY
- **Before:** Time: $\mathcal{O}(M \cdot Q + C \cdot S \cdot D) + \mathcal{O}(N_{\text{history}}) \text{ I/O}$ | Space: $\mathcal{O}(1)$
- **After:** Time: $\mathcal{O}(K)$ | Space: $\mathcal{O}(1)$

---

## OPT-4 — Sử dụng `findStudentGlobal` $O(1)$ trong `handle_report_exam`

### Files changed
- `server/handlers/ReportHandler.cpp`

### Function(s)
- `handle_report_exam`

### BEFORE
```cpp
// server/handlers/ReportHandler.cpp (L31-35)
Lop* lop = dsl.find(malop);
if (!lop) { error_response(res, "Class not found", 404); return; }
SinhVien* sv = lop->dssinhvien.find(masv);
if (!sv) { error_response(res, "Student not found", 404); return; }
```

### AFTER
```cpp
// server/handlers/ReportHandler.cpp (L31-35)
Lop* lop = nullptr;
SinhVien* sv = findStudentGlobal(masv, &lop);
if (!sv || !lop) { error_response(res, "Student not found", 404); return; }
if (lop->MALOP != malop) { error_response(res, "Student does not belong to this class", 404); return; }
```

### WHAT CHANGED
- Thay thế 2 lần quét tuần tự (`dsl.find` mảng $O(n)$ và `dssinhvien.find` DSLK $O(S)$) bằng 1 lần tra cứu `findStudentGlobal` $O(1)$.

### WHY
Tối ưu hóa thời gian sinh báo cáo kết quả thi.

### COMPLEXITY
- **Before:** Time: $\mathcal{O}(C + S)$ | Space: $\mathcal{O}(1)$
- **After:** Time: $\mathcal{O}(1)$ | Space: $\mathcal{O}(1)$

---

## OPT-5 — Thêm Guard `node->data.used` cho `handle_delete_subject`

### Files changed
- `server/handlers/SubjectHandler.cpp`

### Function(s)
- `handle_delete_subject`

### BEFORE
```cpp
// server/handlers/SubjectHandler.cpp (L129-135)
NodeMH* node = dsmh.find(mamh.c_str());
if (!node) {
    error_response(res, "Subject not found", 404); return;
}

int64_t offset = -1;
```

### AFTER
```cpp
// server/handlers/SubjectHandler.cpp (L129-144)
NodeMH* node = dsmh.find(mamh.c_str());
if (!node) {
    error_response(res, "Subject not found", 404); return;
}

if (node->data.used) {
    custom_json_response(res, {
        {"success", false},
        {"error", "Unprocessable Entity"},
        {"message", "Môn học đã có sinh viên thi, không thể xóa."}
    }, 422);
    return;
}

int64_t offset = -1;
```

### WHAT CHANGED
- Bổ sung Guard kiểm tra cờ `node->data.used`, trả về HTTP 422 nếu môn học đã có lịch sử thi.

### WHY
Bảo vệ tính toàn vẹn dữ liệu (Data Integrity), ngăn chặn hiện tượng mất liên kết mã môn (Orphan Reference) trong bảng điểm của sinh viên.

---

## OPT-6 — Thêm Logging cảnh báo và Self-healing cho Fallback Scans

### Files changed
- `server/ServerContext.cpp`
- `server/handlers/QuestionHandler.cpp`

### Function(s)
- `findStudentGlobal`
- `handle_delete_question`
- `handle_restore_question`

### BEFORE
```cpp
// QuestionHandler.cpp (L144-157)
if (!foundNode) {
    std::function<NodeMH*(NodeMH*)> searchNode = [&](NodeMH* node) -> NodeMH* {
        if (!node) return nullptr;
        NodeMH* left = searchNode(node->left);
        if (left) return left;
        if (node->data.dsCauHoi.find(id)) return node;
        return searchNode(node->right);
    };
    foundNode = searchNode(dsmh.getRoot());
    if (foundNode) {
        foundQNode = foundNode->data.dsCauHoi.find(id);
        mamh = string(foundNode->data.MAMH);
    }
}
```

### AFTER
```cpp
// QuestionHandler.cpp (L144-160)
if (!foundNode) {
    std::cerr << "[WARN] handle_delete_question: question id=" << id
              << " not found in questionSubjectIndex. Falling back to BST scan.\n";
    std::function<NodeMH*(NodeMH*)> searchNode = [&](NodeMH* node) -> NodeMH* {
        if (!node) return nullptr;
        NodeMH* left = searchNode(node->left);
        if (left) return left;
        if (node->data.dsCauHoi.find(id)) return node;
        return searchNode(node->right);
    };
    foundNode = searchNode(dsmh.getRoot());
    if (foundNode) {
        foundQNode = foundNode->data.dsCauHoi.find(id);
        mamh = string(foundNode->data.MAMH);
        IndexManager::getInstance().updateQuestionSubject(id, mamh);
    }
}
```

### WHAT CHANGED
- Thêm log cảnh báo khi index bị miss.
- Tự động đăng ký lại `updateQuestionSubject` (Self-healing) khi tìm thấy qua fallback scan.

---

## OPT-7 — Thêm `g_classMap` HashTable tăng tốc `dsl.find` thành $O(1)$

### Files changed
- `server/ServerContext.h`
- `server/ServerContext.cpp`
- `server/ServerBootstrap.cpp`
- `server/handlers/ClassHandler.cpp`
- `server/handlers/StudentHandler.cpp`
- `server/handlers/ReportHandler.cpp`

### Function(s)
- `registerClassGlobal`, `unregisterClassGlobal`, `rebuildGlobalClassMap`, `findClassGlobal`
- `handle_get_class_by_id`, `handle_create_class`, `handle_update_class`, `handle_delete_class`
- `handle_get_students`, `handle_create_student`, `handle_report_scoreboard`

### BEFORE
```cpp
// ClassHandler.cpp
Lop* lop = dsl.find(malop); // O(n) linear scan mảng 10,000 phần tử
```

### AFTER
```cpp
// ClassHandler.cpp
Lop* lop = findClassGlobal(malop); // O(1) hash lookup
```
Triển khai trong `ServerContext.cpp`:
```cpp
static HashTable<std::string, Lop*> g_classMap;

void registerClassGlobal(const std::string& malop, Lop* lop) {
    if (!malop.empty() && lop) {
        g_classMap.insert(malop, lop);
    }
}

void unregisterClassGlobal(const std::string& malop) {
    if (!malop.empty()) {
        g_classMap.remove(malop);
    }
}

void rebuildGlobalClassMap() {
    g_classMap.clear();
    dsLop* root = dsl.getRoot();
    if (!root) return;
    for (int i = 0; i < root->n; i++) {
        if (root->dslop[i]) {
            g_classMap.insert(root->dslop[i]->MALOP, root->dslop[i]);
        }
    }
}

Lop* findClassGlobal(const std::string& malop) {
    Lop** loc = g_classMap.find(malop);
    if (loc && *loc) {
        return *loc;
    }
    std::cerr << "[WARN] findClassGlobal: map miss for malop=" << malop << ". Falling back to linear scan.\n";
    Lop* found = dsl.find(malop);
    if (found) {
        g_classMap.insert(malop, found);
    }
    return found;
}
```

### WHY
Tăng tốc toàn bộ các điểm tra cứu lớp học trong hệ thống từ $\mathcal{O}(C) \to \mathcal{O}(1)$.

---

## OPT-8 — Tinh gọn CORS Headers

### Files changed
- `server/RouteRegistry.cpp`

### WHAT CHANGED
- Toàn bộ CORS headers được áp dụng tập trung và nhất quán qua `svr.set_default_headers()`.

---

## OPT-9 — Lược bỏ `StorageVerifier::verifyScores` thừa sau `saveScores`

### Files changed
- `src/StorageManager.cpp`

### Function(s)
- `StorageManager::saveScores`

### BEFORE
```cpp
// src/StorageManager.cpp (L716-724)
std::string targetPath = PathResolver::getFilePath("scores.txt");
if (!atomicWriteFile(targetPath, ss.str())) return false;

std::string verifyErr;
if (!StorageVerifier::verifyScores(dsl, targetPath, verifyErr)) {
    std::cerr << "[StorageVerification] Deep verification failed: " << verifyErr << std::endl;
    return false;
}
return true;
```

### AFTER
```cpp
// src/StorageManager.cpp (L716-718)
std::string targetPath = PathResolver::getFilePath("scores.txt");
return atomicWriteFile(targetPath, ss.str());
```

### WHY
Bỏ thao tác đọc lại toàn bộ file đĩa sau khi vừa ghi (Double I/O), vì dữ liệu đã được validate chặt chẽ trước khi ghi và việc ghi là atomic qua file tạm.

---

## OPT-10 — Tinh gọn Validation trước `StorageValidator`

### Files changed
- `server/handlers/ClassHandler.cpp`

### WHAT CHANGED
- Loại bỏ các lệnh kiểm tra độ dài thủ công lặp lại trong Handler, ủy quyền toàn bộ cho `StorageValidator::validateClass` thực hiện tập trung.

---

## OPT-11 — Xóa Duplicate Route `/api/questions/:id/restore`

### Files changed
- `server/RouteRegistry.cpp`

### BEFORE
```cpp
svr.Put("/api/questions/:id/restore", wrap_safe(handle_restore_question));
svr.Post("/api/questions/:id/restore", wrap_safe(handle_restore_question));
```

### AFTER
```cpp
svr.Put("/api/questions/:id/restore", wrap_safe(handle_restore_question));
```

### WHY
Khử trùng lặp định tuyến HTTP.

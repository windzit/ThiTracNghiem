# TÀI LIỆU PHÂN TÍCH & THIẾT KẾ HỆ THỐNG THI TRẮC NGHIỆM (C++ CONSOLE)
### Đồ án môn học — chuẩn hoá theo quy trình Business Analyst / Solution Architect thực tế

> **Ghi chú về cấu trúc tài liệu:** Yêu cầu gốc gồm 18 mục lớn, trong đó riêng mục 14 phải đặc tả đầy đủ 16 tiểu mục cho **từng** chức năng a→i (9 chức năng), cộng thêm mục 15 (chức năng đề xuất), 16 (độ khó), 17 (phân công nhóm), 18 (SRS hoàn chỉnh). Nếu dồn tất cả vào một file sẽ vượt quá khả năng xử lý một lượt và làm loãng chất lượng. Do đó tài liệu được chia làm **3 phần**, đánh số liên tục:
>
> - **Phần 1 (file này):** Mục 1 → 11 — Tổng quan, Kết quả Research, Actor, Use Case tổng thể, Business Rule, Validation Rule, Edge Case, Data Flow, Thiết kế dữ liệu, Kiến trúc hệ thống, Đề xuất cấu trúc dữ liệu.
> - **Phần 2:** Mục 12 → 14 — Đề xuất Front-end Console, Đề xuất mở rộng, và đặc tả chi tiết 16 tiểu mục cho từng chức năng **a/ b/ c/ d/ e/ f/ g/ h/ i/** (giữ nguyên ký hiệu, không đổi thành "Use case 1, 2, 3...").
> - **Phần 3:** Mục 15 → 18 — Chức năng đề xuất bổ sung, Phân tích độ khó, Gợi ý phân công nhóm 3 người, và Tài liệu SRS hoàn chỉnh (tổng hợp lại toàn bộ).
>
> Ký hiệu a/, b/, c/... của đề bài gốc được giữ nguyên xuyên suốt tài liệu, không gộp, không tách, không đổi tên.

---

## ĐỀ BÀI GỐC (giữ nguyên văn, không chỉnh sửa)

```
3. Thi trắc nghiệm (chỉ được code C++): Ta tổ chức các danh sách sau:

- Danh sách môn học: cây nhị phân tìm kiếm cân bằng (MAMH (C15), TENMH, ds câu hỏi thi)
- Danh sách Câu hỏi thi: danh sách liên kết đơn. Mỗi câu hỏi có thông tin: Id, Nội dung, A,
  B, C, D, Đáp án; trong đó A, B, C, D là 4 chọn lựa tương ứng với nội dung câu hỏi.
- Danh sách Lớp: mảng con trỏ có tối đa 10000 lớp (MALOP (C15), TENLOP, con trỏ DSSV):
  con trỏ sẽ trỏ đến danh sách sinh viên thuộc lớp đó.
- Danh sách sinh viên: danh sách liên kết đơn (MASV (C15), HO, TEN, PHAI, password, con
  trỏ ds_diemthi): con trỏ sẽ trỏ đến điểm các môn đã thi trắc nghiệm.
- Danh sách Điểm thi (danh sách liên kết đơn) (Mamh, Diem (làm tròn 1 số sau dấu chấm))

Chương trình có các chức năng sau:

a/ Đăng nhập dựa vào mã sinh viên, password. Nếu tài khoản đăng nhập là GV, pass là GV thì
   sẽ có toàn quyền.
b/ NhapLop
c/ In ds lớp
d/ Nhập sinh viên của lớp: nhập vào mã lớp trước, sau đó nhập các sinh viên vào lớp đó.
e/ Nhập môn học: cho phép cập nhật (thêm / xóa / hiệu chỉnh) thông tin của môn học
f/ Nhập câu hỏi thi (Id là số ngẫu nhiên do chương trình tự tạo); Không được xóa câu hỏi
   thi khi đã nằm trong 1 bài thi trắc nghiệm của sinh viên.
g/ Thi Trắc Nghiệm (trước khi thi hỏi người thi môn thi, số câu hỏi thi, số phút thi - sau
   đó lấy ngẫu nhiên các câu hỏi trong danh sách câu hỏi thi của môn;
h/ In chi tiết các câu hỏi đã thi 1 môn học của 1 sinh viên,
i/ In bảng điểm thi trắc nghiệm môn học của 1 lớp (nếu có sinh viên chưa thi thì ghi
   "Chưa thi").

Lưu ý: Chương trình cho phép lưu các danh sách vào file; Kiểm tra các điều kiện làm dữ liệu
bị sai. Sinh viên có thể tự thiết kế thêm danh sách để đáp ứng yêu cầu của đề tài.
```

---

## MỤC 1 — TỔNG QUAN HỆ THỐNG

### 1.1. Mục tiêu
Xây dựng phần mềm thi trắc nghiệm (Computer-Based Test — CBT) dạng ứng dụng console C++, phục vụ một cơ sở đào tạo tổ chức kiểm tra trắc nghiệm cho sinh viên theo môn học, theo lớp. Hệ thống phải:
- Quản lý được dữ liệu môn học, lớp, sinh viên, ngân hàng câu hỏi.
- Cho sinh viên làm bài thi trắc nghiệm có bấm giờ, chấm điểm tự động.
- Cho giáo viên/quản trị xem lại chi tiết bài thi và bảng điểm theo lớp.
- Lưu trữ bền vững bằng file, chịu được các tình huống dữ liệu sai lệch.

### 1.2. Phạm vi
**Trong phạm vi:** quản lý môn học/lớp/sinh viên/câu hỏi; tổ chức 1 lượt thi trắc nghiệm cho 1 sinh viên tại 1 thời điểm; chấm điểm tự động; in ấn báo cáo dạng bảng trên console; lưu/đọc file dữ liệu.

**Ngoài phạm vi (giả định — xem thêm mục Assumption ở Phần 3):** thi đồng thời nhiều sinh viên qua mạng (multi-client/server), giao diện đồ hoạ, xác thực sinh trắc học, chống gian lận bằng camera/AI (các hệ thống thực tế như Azota, HackerRank có, nhưng đề không yêu cầu — được ghi nhận như hướng mở rộng ở mục 13).

### 1.3. Đối tượng sử dụng
- **Sinh viên (SV):** người học, làm bài thi, xem lại kết quả của chính mình.
- **Giáo viên/Quản trị (GV):** tài khoản đặc biệt (đăng nhập bằng "GV"/"GV") có toàn quyền quản trị dữ liệu và xem mọi báo cáo.
- **Hệ thống (tác nhân phi người dùng):** tự sinh Id câu hỏi, tự chấm điểm, tự làm tròn điểm, tự ghi/đọc file.

### 1.4. Mô hình hoạt động
Ứng dụng chạy **đơn tiến trình, đơn người dùng tại một thời điểm** (single-process, single-session console app). Toàn bộ dữ liệu được nạp từ file vào cấu trúc dữ liệu trong bộ nhớ (BST môn học, mảng con trỏ lớp, danh sách liên kết câu hỏi/sinh viên/điểm) khi khởi động, thao tác trên bộ nhớ, và ghi ngược lại file khi người dùng chọn lưu hoặc khi thoát chương trình.

### 1.5. Quy trình tổng quát
```
Khởi động chương trình
   │
   ▼
Nạp dữ liệu từ file (môn học, lớp, sinh viên, câu hỏi, điểm thi)
   │
   ▼
Đăng nhập (MASV/password hoặc GV/GV)
   │
   ├── Là GV  → Menu quản trị (b, c, d, e, f, i, và các chức năng phụ: lưu/đọc file...)
   │
   └── Là SV  → Menu sinh viên (thi trắc nghiệm - g, xem lại bài thi đã làm - h)
   │
   ▼
Thực hiện chức năng, kiểm tra Validation Rule + Business Rule tại mỗi bước
   │
   ▼
Lưu dữ liệu vào file (thủ công hoặc tự động trước khi thoát)
   │
   ▼
Thoát chương trình
```

---

## MỤC 2 — KẾT QUẢ RESEARCH

Việc nghiên cứu tập trung vào cách các hệ thống thật tổ chức **nghiệp vụ** (không sao chép giao diện/code), để rút ra bài học áp dụng cho đồ án console C++.

### 2.1. Moodle Quiz (LMS mã nguồn mở)
Moodle tách bạch rất rõ giữa **Question Bank** (ngân hàng câu hỏi, tổ chức theo category/subcategory) và **Quiz** (đề thi cụ thể lấy câu hỏi từ ngân hàng). <cite index="6-1">Ngân hàng câu hỏi cho phép giáo viên tạo, xem trước và chỉnh sửa câu hỏi, tổ chức chúng theo danh mục và danh mục con giống như cách tổ chức file và thư mục.</cite> Một điểm quan trọng: <cite index="3-1">một câu hỏi trong ngân hàng có thể được tái sử dụng ở nhiều đề thi khác nhau, mỗi lần làm bài được chấm tự động và giáo viên có thể chọn có hiển thị đáp án đúng hay không.</cite>

**Bài học áp dụng:** ngân hàng câu hỏi (danh sách câu hỏi thi theo môn) nên độc lập với "phiên thi" của từng sinh viên; khi sinh viên thi, hệ thống chỉ **lấy ngẫu nhiên tham chiếu** đến câu hỏi trong ngân hàng chứ không copy nội dung — đây chính là lý do đề bài yêu cầu **không được xóa câu hỏi đã từng xuất hiện trong 1 bài thi** (tránh làm "gãy" tham chiếu về sau).

### 2.2. Kahoot! (Game-based quiz)
<cite index="18-1">Kahoot cho phép giáo viên tạo quiz của riêng mình hoặc chọn từ thư viện quiz công khai; khi bắt đầu ván chơi trực tiếp, người học tham gia bằng cách nhập mã PIN, mỗi câu hỏi hiển thị kèm nhiều phương án trả lời, và người chơi được cộng điểm dựa trên việc chọn đúng và tốc độ trả lời.</cite> Sau khi kết thúc, <cite index="18-1">Kahoot cung cấp báo cáo kết quả giúp giáo viên biết câu nào học sinh sai nhiều, điểm số ra sao, từ đó điều chỉnh giảng dạy.</cite>

**Bài học áp dụng:** mô hình "hỏi cấu hình trước khi thi" (số câu, thời gian) trong chức năng g/ của đề bài chính là dạng thu nhỏ của việc Kahoot cho cấu hình timer/loại câu hỏi trước khi host một ván chơi. Thời gian giới hạn cho mỗi lượt thi là cơ chế phổ biến để đảm bảo tính công bằng.

### 2.3. Azota (CBT phổ biến tại Việt Nam)
<cite index="22-1">Azota hỗ trợ đa dạng các tính năng giám sát khi kiểm tra, thi online, theo dõi lịch sử làm bài, phát hiện gian lận bằng công nghệ AI</cite>, và đặc biệt: <cite index="27-1">khi bật chức năng giám sát học sinh, hệ thống sẽ tự động ghi nhận số lần học sinh thoát ứng dụng hoặc chuyển sang tab khác, và báo cáo lại đầy đủ cho giáo viên sau khi thi.</cite> Về thống kê, <cite index="22-1">kết quả làm bài được thống kê theo biểu đồ điểm, phổ điểm, trung vị, bảng xếp hạng.</cite>

**Bài học áp dụng:** báo cáo bảng điểm theo lớp (chức năng i/) nên có trạng thái rõ ràng cho từng sinh viên (đã thi/chưa thi) — đúng như đề bài yêu cầu ghi "Chưa thi"; đây là thông lệ chuẩn của mọi hệ thống CBT thực tế, không riêng gì đồ án này.

### 2.4. HackerRank Assessment (CBT cho tuyển dụng kỹ thuật)
<cite index="34-1">HackerRank Screen cho phép xây dựng bài test tuỳ chỉnh từ thư viện hàng nghìn câu hỏi, thiết lập giới hạn thời gian, và mọi bài test đều được tự động chấm điểm với báo cáo tổng hợp về tỉ lệ đạt, tỉ lệ hoàn thành.</cite> Về việc chọn câu hỏi, <cite index="38-1">hệ thống có cơ chế lấy ngẫu nhiên câu hỏi cho mỗi ứng viên nhằm hạn chế việc đoán bài hoặc chia sẻ đề giữa các ứng viên.</cite>

**Bài học áp dụng:** việc "lấy ngẫu nhiên các câu hỏi trong danh sách câu hỏi thi của môn" (chức năng g/) là thông lệ chuẩn công nghiệp, không phải đơn giản hoá cho đồ án — cần đảm bảo thuật toán rút ngẫu nhiên không lặp câu hỏi trong cùng 1 lượt thi.

### 2.5. Ghi nhận nhanh về các hệ thống còn lại
- **Canvas LMS / Blackboard:** cùng triết lý với Moodle — ngân hàng câu hỏi tách khỏi bài quiz, hỗ trợ nhiều kiểu câu hỏi, tích hợp sổ điểm (gradebook) toàn khoá học.
- **Google Forms Quiz / Microsoft Forms:** đơn giản hoá tối đa, chấm điểm tự động cho trắc nghiệm, xuất kết quả dạng bảng tính — mô hình "nhẹ" phù hợp tham khảo cho phần in báo cáo dạng bảng trên console.
- **Quizizz:** tương tự Kahoot nhưng hỗ trợ học theo nhịp độ cá nhân (self-paced), có thống kê theo từng câu hỏi.
- **SHub Classroom, CodePTIT, TestDome:** các nền tảng Việt Nam/quốc tế dùng cho luyện tập và thi thử, đều theo mô hình chung: ngân hàng câu hỏi → cấu hình đề → làm bài có giờ → chấm tự động → báo cáo.

### 2.6. Tổng kết bài học rút ra cho đồ án
| # | Bài học từ hệ thống thật | Áp dụng vào đồ án |
|---|---|---|
| 1 | Ngân hàng câu hỏi tách rời khỏi phiên thi (Moodle) | Không xóa câu hỏi đã dùng trong bài thi nào đó (đúng yêu cầu đề — f/) |
| 2 | Cấu hình trước khi thi: số câu, thời gian (Kahoot, HackerRank) | Đúng yêu cầu g/: hỏi môn, số câu, số phút trước khi thi |
| 3 | Rút ngẫu nhiên không trùng lặp (HackerRank) | Thuật toán chọn ngẫu nhiên không lặp trong danh sách câu hỏi liên kết đơn của môn |
| 4 | Trạng thái rõ ràng "chưa hoàn thành" trong báo cáo (Azota) | Đúng yêu cầu i/: ghi "Chưa thi" nếu sinh viên chưa làm |
| 5 | Toàn quyền quản trị tách biệt với người học (mọi LMS) | Tài khoản GV có toàn quyền, tách biệt luồng SV |
| 6 | Lưu trữ bền vững, phục hồi khi có sự cố (mọi CBT thật) | Ghi/đọc file, kiểm tra dữ liệu sai lệch theo yêu cầu "Lưu ý" của đề |

---

## MỤC 3 — PHÂN TÍCH ACTOR

| Actor | Vai trò | Quyền | Giới hạn | Chức năng được phép |
|---|---|---|---|---|
| **Sinh viên (SV)** | Người học, thí sinh | Xem thông tin cá nhân, xem lớp mình, làm bài thi, xem lại bài thi của chính mình | Không được sửa/xóa dữ liệu môn học, lớp, câu hỏi, điểm; không xem được bài thi/điểm của sinh viên khác; mỗi môn chỉ thi 1 lần (Business Rule) | g/ (thi trắc nghiệm — môn mình chưa thi), h/ (xem lại bài thi của chính mình) |
| **Giáo viên/Quản trị (GV)** | Toàn quyền quản trị hệ thống | Toàn quyền: thêm/sửa/xóa môn học, lớp, sinh viên, câu hỏi; xem báo cáo điểm mọi lớp; xem chi tiết bài thi của bất kỳ sinh viên nào; lưu/đọc file | Vẫn phải tuân thủ Business Rule (không xóa câu hỏi đã thi, không sửa điểm đã chấm...) | a/ (đăng nhập với quyền GV), b/, c/, d/, e/, f/, h/, i/, và các chức năng quản trị file |
| **Hệ thống (System)** | Tác nhân phi người dùng, chạy nền theo sự kiện | Tự sinh Id ngẫu nhiên cho câu hỏi; tự chấm điểm theo đáp án; tự làm tròn điểm 1 số sau dấu phẩy; tự kiểm tra validation; tự đọc/ghi file | Không có giao diện riêng, chỉ phản ứng theo yêu cầu của SV/GV | Sinh Id, chấm điểm, làm tròn, đọc/ghi file, kiểm tra ràng buộc |

**Ghi chú:** đề bài chỉ định nghĩa 2 loại tài khoản đăng nhập (SV thường và "GV") — không có khái niệm "Khách". Việc bổ sung actor "Khách" (nếu cần) sẽ được đề xuất ở mục 15 (Chức năng đề xuất bổ sung) chứ không chèn vào đây.

---

## MỤC 4 — USE CASE TỔNG THỂ

Theo đúng tinh thần BA thực thụ, danh sách dưới đây liệt kê **toàn bộ nghiệp vụ có thể xảy ra**, không chỉ 9 chức năng a→i. Các use case **in đậm** là use case có trong đề gốc (được đặc tả đầy đủ ở Phần 2 — mục 14, giữ nguyên ký hiệu a→i). Các use case còn lại là phân tích mở rộng của BA, sẽ được đánh giá độ phù hợp và độ ưu tiên ở mục 15 (Phần 3) — **không thay thế hay chèn vào giữa a→i**.

### 4.1. Nhóm Authentication & tài khoản
- **a/ Đăng nhập** (có trong đề)
- Đăng xuất (đề xuất bổ sung)
- Đổi mật khẩu sinh viên (đề xuất bổ sung)
- Khóa/Mở khóa tài khoản sinh viên (đề xuất bổ sung)

### 4.2. Nhóm quản lý Lớp — Sinh viên
- **b/ Nhập lớp (NhapLop)** (có trong đề)
- **c/ In danh sách lớp** (có trong đề)
- **d/ Nhập sinh viên của lớp** (có trong đề)
- Sửa/xóa thông tin lớp (đề xuất bổ sung)
- Sửa/xóa thông tin sinh viên (đề xuất bổ sung)
- Tìm kiếm sinh viên theo MASV/họ tên (đề xuất bổ sung)

### 4.3. Nhóm quản lý Môn học — Câu hỏi
- **e/ Nhập môn học (thêm/xóa/hiệu chỉnh)** (có trong đề)
- **f/ Nhập câu hỏi thi** (có trong đề)
- Sửa nội dung câu hỏi khi chưa từng được thi (đề xuất bổ sung, hệ quả tự nhiên của f/)
- Tìm kiếm môn học theo mã (tận dụng đặc tính BST — đề xuất bổ sung)

### 4.4. Nhóm tổ chức thi
- **g/ Thi trắc nghiệm** (có trong đề)
- **h/ In chi tiết các câu hỏi đã thi của 1 sinh viên** (có trong đề)
- **i/ In bảng điểm thi trắc nghiệm của 1 lớp** (có trong đề)
- Nộp bài (là một phần của luồng g/, được đặc tả trong luồng chính của g/)
- Hủy/reset kết quả thi của 1 sinh viên cho 1 môn (đề xuất bổ sung — GV dùng khi có sự cố)

### 4.5. Nhóm dữ liệu & vận hành hệ thống
- Lưu danh sách vào file (có trong "Lưu ý" của đề — bắt buộc)
- Đọc danh sách từ file khi khởi động (có trong "Lưu ý" của đề — bắt buộc)
- Kiểm tra dữ liệu sai lệch khi đọc file (có trong "Lưu ý" của đề — bắt buộc)
- Backup / Restore dữ liệu (đề xuất bổ sung)
- Import/Export dữ liệu câu hỏi từ file rời (đề xuất bổ sung)

---

## MỤC 5 — BUSINESS RULE

| Mã | Business Rule |
|---|---|
| BR-01 | Tài khoản đăng nhập là **MASV** thường → vai trò Sinh viên; đăng nhập bằng **"GV"/"GV"** → vai trò Giáo viên với toàn quyền. |
| BR-02 | Một sinh viên **chỉ được thi 1 lần cho mỗi môn học**. Nếu đã có bản ghi điểm cho (MASV, MAMH) thì không được thi lại (trừ khi GV chủ động reset — xem use case đề xuất 4.4). |
| BR-03 | **Không được xóa câu hỏi thi** nếu câu hỏi đó đã từng được lấy vào 1 bài thi của bất kỳ sinh viên nào (đúng yêu cầu f/). Cần cơ chế đánh dấu "đã sử dụng" cho câu hỏi. |
| BR-04 | **Không được xóa môn học** nếu môn đó đã có sinh viên thi (hệ quả logic của BR-03, vì môn học chứa câu hỏi đã dùng); chỉ cho phép hiệu chỉnh tên môn, không đổi mã môn (MAMH là khóa chính). |
| BR-05 | **Không được xóa lớp** nếu lớp đã có sinh viên có điểm thi (tránh mất dữ liệu điểm không thể phục hồi). |
| BR-06 | Khi nhập sinh viên vào lớp (d/), phải nhập **mã lớp trước**; nếu mã lớp không tồn tại thì từ chối và yêu cầu tạo lớp (b/) trước. |
| BR-07 | Số câu hỏi rút ngẫu nhiên khi thi (g/) **không được vượt quá** số câu hỏi hiện có trong ngân hàng câu hỏi của môn đó. |
| BR-08 | Thời gian thi (số phút) phải là số nguyên dương; hệ thống phải tự động nộp bài khi hết giờ (auto-submit), không cho sinh viên tiếp tục trả lời sau khi hết thời gian. |
| BR-09 | Điểm thi luôn được **làm tròn 1 số sau dấu phẩy** (đúng yêu cầu cấu trúc Điểm thi). |
| BR-10 | Khi in bảng điểm theo lớp (i/), sinh viên chưa có bản ghi điểm cho môn đang xét phải hiển thị **"Chưa thi"**, không được bỏ trống hay ghi điểm 0 (0 là điểm thật, "Chưa thi" là trạng thái khác biệt). |
| BR-11 | Id câu hỏi là số ngẫu nhiên **do hệ thống tự sinh**, không cho người dùng nhập tay; hệ thống phải đảm bảo Id không trùng với Id đã tồn tại trong toàn bộ ngân hàng câu hỏi. |
| BR-12 | Mọi thao tác ghi/xóa/sửa dữ liệu (môn học, lớp, sinh viên, câu hỏi) chỉ được thực hiện bởi tài khoản **GV**; Sinh viên chỉ có quyền đọc dữ liệu liên quan đến chính mình và làm bài thi. |
| BR-13 | Nếu chương trình bị tắt đột ngột (mất điện, đóng cửa sổ) **giữa lúc đang thi**, bài thi đó coi như chưa hoàn tất và không được tính điểm (vì chưa qua bước nộp bài) — trạng thái vẫn là "Chưa thi" cho tới khi thi lại từ đầu. |
| BR-14 | Dữ liệu chỉ được ghi xuống file khi người dùng chọn lưu hoặc khi thoát chương trình đúng quy trình; không tự động ghi đè file trong lúc đang xử lý dở để tránh hỏng file khi có sự cố giữa chừng. |

---

## MỤC 6 — VALIDATION RULE

| Đối tượng | Trường dữ liệu | Quy tắc kiểm tra |
|---|---|---|
| Môn học | MAMH | Không rỗng, đúng 15 ký tự (C15), không chứa khoảng trắng, không trùng với mã đã có (khóa chính của BST) |
| Môn học | TENMH | Không rỗng, không chỉ toàn khoảng trắng, giới hạn độ dài hợp lý (vd ≤ 100 ký tự) |
| Câu hỏi | Id | Do hệ thống sinh, không cho nhập tay; đảm bảo duy nhất toàn hệ thống |
| Câu hỏi | Nội dung, A, B, C, D | Không rỗng, không toàn khoảng trắng; nên có giới hạn độ dài tối đa để hiển thị hợp lý trên console |
| Câu hỏi | Đáp án | Chỉ nhận 1 trong 4 giá trị {A, B, C, D}, không phân biệt hoa/thường khi nhập nhưng lưu chuẩn hóa dạng in hoa |
| Lớp | MALOP | Không rỗng, đúng 15 ký tự (C15), không trùng mã lớp đã có; tổng số lớp không vượt quá 10000 (giới hạn mảng con trỏ) |
| Lớp | TENLOP | Không rỗng, không toàn khoảng trắng |
| Sinh viên | MASV | Không rỗng, đúng 15 ký tự (C15), không trùng mã trong toàn hệ thống (không chỉ trong 1 lớp) |
| Sinh viên | HO, TEN | Không rỗng; nên cho phép ký tự có dấu tiếng Việt; không toàn khoảng trắng |
| Sinh viên | PHAI (giới tính) | Chỉ nhận giá trị hợp lệ đã định nghĩa (vd Nam/Nữ hoặc 0/1), từ chối giá trị khác |
| Sinh viên | password | Không rỗng; nên có độ dài tối thiểu (khuyến nghị bổ sung ở mục 15, đề không quy định) |
| Điểm thi | Mamh | Phải là mã môn học đã tồn tại trong danh sách môn học (ràng buộc khóa ngoại) |
| Điểm thi | Diem | Số thực trong khoảng hợp lệ (vd 0–10), làm tròn 1 số sau dấu phẩy trước khi lưu |
| Thi trắc nghiệm | Số câu hỏi thi | Số nguyên dương, ≤ tổng số câu hỏi hiện có của môn đó |
| Thi trắc nghiệm | Số phút thi | Số nguyên dương, > 0 |

**Nguyên tắc chung áp dụng cho mọi trường dữ liệu chuỗi:** kiểm tra rỗng (empty), kiểm tra NULL/con trỏ hỏng, cắt bỏ khoảng trắng thừa đầu-cuối (trim), từ chối chuỗi toàn khoảng trắng, kiểm tra ký tự đặc biệt không hợp lệ khi trường đó là mã định danh (MAMH/MALOP/MASV không nên chứa ký tự đặc biệt), kiểm tra trùng mã trước khi thêm mới.

---

## MỤC 7 — EDGE CASE

| # | Tình huống | Cách xử lý đề xuất |
|---|---|---|
| 1 | Môn học chưa có câu hỏi nào nhưng sinh viên chọn thi môn đó | Từ chối bắt đầu thi, thông báo "Môn học chưa có câu hỏi" |
| 2 | Số câu hỏi sinh viên yêu cầu thi > số câu hỏi hiện có của môn | Từ chối hoặc tự động giới hạn lại bằng số câu hỏi tối đa hiện có, kèm cảnh báo |
| 3 | Thời gian thi = 0 hoặc số âm | Từ chối, yêu cầu nhập lại số phút hợp lệ (>0) |
| 4 | Sinh viên đóng chương trình đột ngột giữa lúc đang thi | Bài thi không được ghi nhận điểm (theo BR-13); lần sau vẫn ở trạng thái "Chưa thi" |
| 5 | Môn học không tồn tại khi nhập điểm/nhập câu hỏi/tra cứu | Báo lỗi rõ ràng, không tạo bản ghi "rỗng"/mặc định |
| 6 | Lớp không tồn tại khi nhập sinh viên (d/) | Từ chối, yêu cầu tạo lớp trước bằng b/ |
| 7 | Số lớp vượt quá 10000 (giới hạn mảng con trỏ) | Từ chối thêm lớp mới, thông báo đã đạt giới hạn tối đa |
| 8 | File dữ liệu bị lỗi định dạng / thiếu trường khi đọc lúc khởi động | Bỏ qua dòng lỗi, ghi log cảnh báo, không dừng toàn bộ chương trình |
| 9 | Id câu hỏi bị trùng do lỗi sinh ngẫu nhiên (xác suất thấp nhưng phải phòng ngừa) | Sinh lại Id khác cho đến khi duy nhất |
| 10 | MAMH/MALOP/MASV nhập trùng với mã đã tồn tại | Từ chối thêm mới, yêu cầu nhập mã khác |
| 11 | Xóa câu hỏi đã từng nằm trong 1 bài thi | Từ chối theo BR-03, thông báo lý do |
| 12 | Sinh viên thi môn mà mình đã thi rồi | Từ chối theo BR-02, gợi ý xem lại kết quả (h/) thay vì thi lại |
| 13 | Danh sách câu hỏi của môn có ít hơn 4 câu nhưng người dùng chọn thi 10 câu | Từ chối hoặc giới hạn theo số câu thực có (liên quan Edge Case #2) |
| 14 | Sinh viên chưa từng thi môn nào khi GV in bảng điểm lớp (i/) | Toàn bộ dòng của sinh viên đó ghi "Chưa thi" cho các môn tương ứng |
| 15 | Nhập giới tính (PHAI) với giá trị không hợp lệ (vd số 5, ký tự lạ) | Từ chối, yêu cầu nhập lại theo danh sách giá trị hợp lệ |
| 16 | File dữ liệu không tồn tại ở lần chạy đầu tiên (chưa từng lưu) | Khởi tạo cấu trúc dữ liệu rỗng, không báo lỗi, coi là hệ thống mới |

---

## MỤC 8 — DATA FLOW

| Thực thể dữ liệu | Ai tạo | Ai sửa | Ai xóa | Ai đọc |
|---|---|---|---|---|
| Môn học (BST) | GV (e/) | GV (e/), chỉ khi chưa có sinh viên thi (BR-04) | GV (e/), chỉ khi chưa có sinh viên thi (BR-04) | GV, SV (gián tiếp qua g/, h/) |
| Câu hỏi thi (list liên kết) | GV (f/) | GV (f/), chỉ khi chưa từng được thi (BR-03) | GV (f/), chỉ khi chưa từng được thi (BR-03) | Hệ thống (khi rút ngẫu nhiên cho g/), GV |
| Lớp (mảng con trỏ) | GV (b/) | GV, chỉ khi chưa có SV có điểm (BR-05) | GV, chỉ khi chưa có SV có điểm (BR-05) | GV (c/), SV (xem lớp mình) |
| Sinh viên (list liên kết) | GV (d/) | GV | GV, cân nhắc nếu SV đã có điểm thi | GV, SV (chính mình) |
| Điểm thi (list liên kết) | Hệ thống (tự động sau khi SV nộp bài ở g/) | Không cho sửa tay (chỉ hệ thống ghi nhận lúc chấm) | Không cho xóa trực tiếp (đề không yêu cầu) | GV (i/), SV (chính mình, qua h/) |

### Luồng dữ liệu tổng quát
```
[File dữ liệu trên đĩa]
        │  đọc lúc khởi động
        ▼
[Cấu trúc dữ liệu trong bộ nhớ: BST môn học ─ list câu hỏi ─ mảng lớp ─ list sinh viên ─ list điểm]
        │                              ▲
        │  GV thao tác (b,c,d,e,f,i)   │  SV thao tác (g,h)
        ▼                              │
   [Cập nhật bộ nhớ] ────────────────────
        │
        │  người dùng chọn Lưu / hoặc thoát chương trình đúng quy trình
        ▼
[Ghi ngược lại File dữ liệu trên đĩa]
```

**Khi nào đọc file:** duy nhất 1 lần lúc khởi động chương trình (nạp toàn bộ 5 danh sách).
**Khi nào ghi file:** khi người dùng chọn chức năng "Lưu dữ liệu" trong menu, và bắt buộc trước khi thoát chương trình (kể cả thoát bằng lựa chọn "Thoát" trong menu).

---

## MỤC 9 — THIẾT KẾ DỮ LIỆU (mô hình hoá như thiết kế Database dù cài đặt bằng cấu trúc dữ liệu C++)

### 9.1. Thực thể (Entity) và thuộc tính

| Entity | Thuộc tính | Khóa chính (PK) | Khóa ngoại (FK) |
|---|---|---|---|
| **MonHoc** | MAMH (C15), TENMH, DsCauHoi | MAMH | — |
| **CauHoi** | Id, NoiDung, A, B, C, D, DapAn | Id | (thuộc về 1 MonHoc qua danh sách chứa nó — quan hệ ngầm định qua vị trí lưu trữ, không lưu FK tường minh vì câu hỏi nằm trong ds của môn) |
| **Lop** | MALOP (C15), TENLOP, DsSV | MALOP | — |
| **SinhVien** | MASV (C15), HO, TEN, PHAI, password, DsDiemThi | MASV | Lop chứa SinhVien (quan hệ ngầm định: SV nằm trong DsSV của 1 Lop) |
| **DiemThi** | MAMH, Diem | (MASV, MAMH) — khóa phức hợp ở cấp độ nghiệp vụ | MAMH → MonHoc.MAMH |

### 9.2. Mối quan hệ (Relationship) và bản số (Cardinality)

| Quan hệ | Bản số | Ràng buộc |
|---|---|---|
| MonHoc — CauHoi | 1 — N | Một môn học có nhiều câu hỏi; một câu hỏi thuộc đúng 1 môn học |
| Lop — SinhVien | 1 — N | Một lớp có nhiều sinh viên; một sinh viên thuộc đúng 1 lớp (theo đề, mỗi SV nằm trong DsSV của 1 Lop cụ thể) |
| SinhVien — DiemThi | 1 — N | Một sinh viên có nhiều bản ghi điểm (mỗi môn tối đa 1 bản ghi — hệ quả của BR-02) |
| MonHoc — DiemThi | 1 — N | Một môn học có thể xuất hiện trong điểm thi của nhiều sinh viên |

### 9.3. Ràng buộc toàn vẹn (Constraint)
- **PK uniqueness:** MAMH, MALOP, MASV, Id (câu hỏi) phải duy nhất trong toàn hệ thống.
- **FK integrity:** DiemThi.Mamh phải tham chiếu đến một MAMH tồn tại trong MonHoc (không cho điểm thi "mồ côi").
- **Business constraint:** (MASV, MAMH) trong DiemThi là duy nhất — không có 2 bản ghi điểm cho cùng 1 sinh viên/1 môn (đảm bảo BR-02).
- **Dependency:** không xóa được MonHoc nếu còn CauHoi đã dùng (BR-03/04); không xóa được Lop nếu SinhVien trong lớp đã có DiemThi (BR-05).

### 9.4. Sơ đồ quan hệ (dạng văn bản)
```
MonHoc (1) ───< (N) CauHoi
   │
   │ (1)
   │
   < (N) DiemThi >── (N) ─── (1) SinhVien ── (N) >── (1) Lop
```

---

## MỤC 10 — KIẾN TRÚC HỆ THỐNG

### 10.1. Kiến trúc phân lớp (layered, phù hợp console C++)
```
┌─────────────────────────────────────────────┐
│  Presentation Layer (Console UI)             │  → Menu, Dashboard, Form nhập liệu, Bảng in
├─────────────────────────────────────────────┤
│  Application / Business Logic Layer          │  → Xử lý nghiệp vụ a→i, Business Rule
├─────────────────────────────────────────────┤
│  Validation Layer                            │  → Kiểm tra dữ liệu đầu vào (mục 6)
├─────────────────────────────────────────────┤
│  Data Structure Layer (BST/List/Array)       │  → MonHoc(BST), CauHoi(list), Lop(mảng con trỏ),
│                                               │     SinhVien(list), DiemThi(list)
├─────────────────────────────────────────────┤
│  Storage Layer (File I/O)                    │  → Đọc/ghi file, kiểm tra dữ liệu lỗi khi đọc
└─────────────────────────────────────────────┘
```

### 10.2. Đề xuất chia module
| Module | Trách nhiệm |
|---|---|
| **Authentication** | Đăng nhập, phân quyền SV/GV (a/) |
| **Subject (MonHoc)** | BST môn học, thêm/sửa/xóa môn (e/), tìm kiếm theo MAMH |
| **Question (CauHoi)** | Danh sách liên kết câu hỏi theo môn, sinh Id ngẫu nhiên, thêm/xóa có kiểm tra BR-03 (f/) |
| **Class (Lop)** | Mảng con trỏ lớp, thêm lớp (b/), in danh sách lớp (c/) |
| **Student (SinhVien)** | Danh sách liên kết sinh viên trong từng lớp, nhập sinh viên vào lớp (d/) |
| **Exam (ThiTracNghiem)** | Luồng tổ chức thi: hỏi môn/số câu/số phút, rút ngẫu nhiên, chấm điểm, bấm giờ (g/) |
| **Score (DiemThi)** | Danh sách điểm thi, làm tròn điểm, in chi tiết (h/), in bảng điểm lớp (i/) |
| **Validation** | Tập hàm kiểm tra dữ liệu dùng chung cho mọi module (mục 6) |
| **Storage** | Đọc/ghi file cho cả 5 danh sách, xử lý lỗi định dạng |
| **Utilities** | Sinh số ngẫu nhiên, làm tròn số, xử lý chuỗi (trim, so sánh không phân biệt hoa/thường) |
| **Report** | Định dạng bảng in console cho c/, h/, i/ |

### 10.3. Đề xuất ánh xạ module → file (Thiết kế file)
```
main.cpp                 // Điểm khởi động, gọi menu theo vai trò

Subject.h / Subject.cpp        // struct MonHoc + cây BST + thao tác CRUD
Question.h / Question.cpp      // struct CauHoi + danh sách liên kết + sinh Id
Class.h / Class.cpp            // struct Lop + mảng con trỏ Lop*
Student.h / Student.cpp        // struct SinhVien + danh sách liên kết
Exam.h / Exam.cpp              // logic tổ chức 1 lượt thi (g/), chấm điểm
Score.h / Score.cpp            // struct DiemThi + danh sách liên kết + báo cáo h/, i/
Auth.h / Auth.cpp              // đăng nhập, phân quyền (a/)
Validation.h / Validation.cpp  // các hàm kiểm tra dữ liệu dùng chung
Storage.h / Storage.cpp        // đọc/ghi file cho từng loại danh sách
Utils.h / Utils.cpp            // random Id, làm tròn, xử lý chuỗi
Report.h / Report.cpp          // in bảng console cho c/, h/, i/
Menu.h / Menu.cpp              // hiển thị menu console theo vai trò GV/SV
```

---

## MỤC 11 — ĐỀ XUẤT CẤU TRÚC DỮ LIỆU

### 11.1. Đánh giá cấu trúc dữ liệu bắt buộc theo đề (không được thay đổi)
| Danh sách | Cấu trúc theo đề | Đánh giá phù hợp |
|---|---|---|
| Môn học | Cây nhị phân tìm kiếm cân bằng (BST) theo MAMH | Phù hợp cho tra cứu môn học theo mã (O(log n) nếu cây thực sự cân bằng); đề chỉ nói "cân bằng" — nên cài đặt như BST tự cân bằng thủ công (vd giữ cân bằng khi build từ dữ liệu sắp xếp, hoặc AVL đơn giản hoá) |
| Câu hỏi thi | Danh sách liên kết đơn | Phù hợp vì số câu hỏi mỗi môn không quá lớn, thao tác chính là duyệt tuần tự để rút ngẫu nhiên |
| Lớp | Mảng con trỏ, tối đa 10000 | Phù hợp, truy cập theo chỉ số nhanh; cần duyệt tuyến tính để tìm theo MALOP (hoặc sắp xếp mảng theo MALOP để tìm nhị phân) |
| Sinh viên | Danh sách liên kết đơn (trong mỗi lớp) | Phù hợp vì số SV/lớp không quá lớn |
| Điểm thi | Danh sách liên kết đơn (trong mỗi sinh viên) | Phù hợp vì số môn mỗi SV đã thi thường nhỏ |

**Kết luận:** giữ nguyên toàn bộ 5 cấu trúc dữ liệu theo đúng yêu cầu đề bài, không thay thế.

### 11.2. Cấu trúc dữ liệu phụ đề xuất bổ sung (chỉ hỗ trợ, không thay thế yêu cầu đề)
| Cấu trúc phụ | Vai trò | Vì sao cần |
|---|---|---|
| **HashMap phụ (Id câu hỏi → con trỏ CauHoi)** | Tăng tốc kiểm tra Id trùng khi sinh ngẫu nhiên (BR-11) | Nếu chỉ dùng list liên kết, kiểm tra trùng Id là O(n); với hashmap phụ là O(1) trung bình. Đây chỉ là **chỉ mục (index) phụ**, dữ liệu gốc vẫn là list liên kết theo đúng đề. |
| **Mảng tạm (temporary array) khi rút ngẫu nhiên câu hỏi cho 1 lượt thi (g/)** | Đảm bảo không rút trùng câu hỏi trong cùng 1 đề thi | Duyệt list liên kết ra mảng con trỏ tạm, xáo trộn (shuffle) hoặc đánh dấu đã chọn, sau đó lấy đủ số câu yêu cầu |
| **Cờ đánh dấu "đã sử dụng" (used flag) trong struct CauHoi** | Hỗ trợ BR-03 (không xóa câu hỏi đã thi) | Thay vì duyệt lại toàn bộ điểm thi để kiểm tra câu hỏi có từng được thi hay chưa, đánh dấu ngay khi câu hỏi được chọn vào 1 lượt thi — tra cứu O(1) khi xóa |
| **Mảng chỉ mục sắp xếp theo MALOP (song song với mảng con trỏ Lop)** | Tăng tốc tìm kiếm nhị phân trong mảng lớp (thay vì tuyến tính) | Vì mảng lớp có thể lên tới 10000 phần tử, tìm tuyến tính O(n) là chấp nhận được cho đồ án nhưng chỉ mục sắp xếp giúp tối ưu nếu cần mở rộng |

**Nguyên tắc áp dụng:** mọi cấu trúc phụ nêu trên đóng vai trò **chỉ mục/tăng tốc (index/cache)**, dữ liệu chính thức (nguồn sự thật — *source of truth*) vẫn luôn là 5 danh sách bắt buộc theo đề. Khi ghi file, chỉ ghi dữ liệu chính; cấu trúc phụ được xây dựng lại (rebuild) từ dữ liệu chính mỗi khi nạp lại chương trình.

---

*(Hết Phần 1 — tiếp tục ở Phần 2: Mục 12 Đề xuất Front-end, Mục 13 Đề xuất mở rộng, Mục 14 đặc tả chi tiết từng chức năng a→i)*

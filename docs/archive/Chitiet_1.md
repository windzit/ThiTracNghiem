# TÀI LIỆU PHÂN TÍCH & THIẾT KẾ HỆ THỐNG THI TRẮC NGHIỆM (C++ CONSOLE)
### Phần 2b — Mục 14: Đặc tả chi tiết từng chức năng (a/ → e/)

*(Giữ nguyên ký hiệu a/, b/, c/, d/, e/ của đề gốc — không gộp, không tách, không đổi thành "Use case 1, 2, 3...". Phần f/ g/ h/ i/ tiếp tục ở Phần 2c.)*

---

## a/ ĐĂNG NHẬP

**1. Mục đích**
Xác thực người dùng và phân quyền truy cập (Sinh viên thường / Giáo viên toàn quyền) trước khi cho phép sử dụng bất kỳ chức năng nào khác của hệ thống.

**2. Mô tả chức năng**
Người dùng nhập mã đăng nhập (MASV hoặc chuỗi đặc biệt "GV") và mật khẩu. Nếu mã đăng nhập là "GV" và mật khẩu là "GV" → cấp quyền Giáo viên (toàn quyền). Nếu mã đăng nhập trùng khớp với một MASV tồn tại và mật khẩu khớp với password đã lưu của sinh viên đó → cấp quyền Sinh viên, giới hạn theo chính sinh viên đó.

**3. Đối tượng sử dụng**
Sinh viên, Giáo viên (mọi người dùng đều phải qua bước này trước tiên).

**4. Điều kiện trước**
- Chương trình đã khởi động và nạp xong dữ liệu từ file (danh sách sinh viên đã có trong bộ nhớ để đối chiếu).

**5. Điều kiện sau**
- Nếu thành công: hệ thống xác định được vai trò (SV/GV) và (nếu là SV) xác định được đối tượng SinhVien tương ứng để dùng cho các thao tác sau (g/, h/); menu tương ứng vai trò được hiển thị.
- Nếu thất bại: người dùng không được vào hệ thống, có thể thử lại hoặc thoát.

**6. Luồng chính**
1. Hệ thống hiển thị màn hình yêu cầu nhập mã đăng nhập.
2. Người dùng nhập mã đăng nhập.
3. Hệ thống yêu cầu nhập mật khẩu.
4. Người dùng nhập mật khẩu.
5. Hệ thống kiểm tra: nếu mã = "GV" và mật khẩu = "GV" → gán vai trò Giáo viên, chuyển bước 7.
6. Ngược lại, hệ thống tìm MASV trong danh sách sinh viên; nếu tìm thấy và password khớp → gán vai trò Sinh viên (lưu con trỏ/tham chiếu đến SinhVien này), chuyển bước 7.
7. Hệ thống hiển thị menu tương ứng vai trò.

**7. Luồng thay thế**
- 7a. Nếu mã đăng nhập không tồn tại hoặc mật khẩu sai → hệ thống thông báo "Sai mã đăng nhập hoặc mật khẩu", cho phép nhập lại (giới hạn số lần thử — xem mục Ghi chú triển khai).

**8. Luồng ngoại lệ**
- Danh sách sinh viên rỗng (chưa từng nhập dữ liệu nào) và người dùng không đăng nhập bằng GV → luôn báo sai, hướng dẫn liên hệ GV để được tạo tài khoản (tức là được thêm vào lớp qua d/).
- Người dùng nhập chuỗi rỗng cho mã đăng nhập hoặc mật khẩu → từ chối ngay, không cần tra cứu danh sách.

**9. Business Rule**
BR-01 (mã "GV"/"GV" → toàn quyền; MASV/password khớp → quyền Sinh viên); BR-12 (chỉ GV được thao tác ghi/sửa/xóa dữ liệu).

**10. Validation Rule**
Mã đăng nhập và mật khẩu không được rỗng/toàn khoảng trắng trước khi tra cứu.

**11. Edge Case**
- Mật khẩu phân biệt hoa/thường hay không cần được quy định rõ (khuyến nghị: **có** phân biệt hoa/thường cho password để tránh nhầm giữa "GV" và "gv" — vì "GV" là từ khóa đặc biệt của hệ thống, không phải MASV thật).
- Trường hợp một MASV thật trùng ngẫu nhiên với chuỗi "GV" (xác suất gần như 0 vì MASV có C15, nhưng cần validation ở e/ ngăn nhập trùng).

**12. Dữ liệu liên quan**
SinhVien (MASV, password) — chỉ đọc, không ghi trong chức năng này.

**13. File liên quan**
`Auth.cpp/h` (logic xác thực); dữ liệu đọc từ file sinh viên đã nạp sẵn trong bộ nhớ lúc khởi động (`Storage.cpp` phụ trách đọc file, không đọc lại file ở bước đăng nhập).

**14. Giao diện Console đề xuất**
```
╔══════════════════════════════╗
║   ĐĂNG NHẬP HỆ THỐNG          ║
╠══════════════════════════════╣
║ Mã đăng nhập : ______         ║
║ Mật khẩu     : ______         ║
╚══════════════════════════════╝
```

**15. Khả năng mở rộng**
Khi mở rộng Web/REST API: chức năng này trở thành endpoint `POST /auth/login`, trả về token thay vì lưu vai trò trong biến toàn cục của tiến trình console; nên tách sẵn hàm `xacThuc(maDangNhap, matKhau) -> KetQuaXacThuc` độc lập khỏi phần in màn hình để tái sử dụng.

**16. Ghi chú triển khai**
Khuyến nghị giới hạn số lần nhập sai liên tiếp (vd 5 lần) trước khi tạm khóa/thoát chương trình — đề gốc không yêu cầu nhưng là thông lệ bảo mật tối thiểu (đề xuất thêm ở Mục 15, không bắt buộc).

---

## b/ NHẬP LỚP (NhapLop)

**1. Mục đích**
Cho phép Giáo viên tạo mới một lớp học trong hệ thống, làm tiền đề để sau đó nhập sinh viên vào lớp (d/).

**2. Mô tả chức năng**
Giáo viên nhập thông tin MALOP và TENLOP; hệ thống kiểm tra hợp lệ và không trùng, sau đó thêm lớp mới vào mảng con trỏ Lớp (con trỏ danh sách sinh viên của lớp được khởi tạo rỗng — NULL).

**3. Đối tượng sử dụng**
Giáo viên (duy nhất — theo BR-12).

**4. Điều kiện trước**
Đã đăng nhập với vai trò Giáo viên; số lớp hiện có chưa đạt giới hạn tối đa 10000.

**5. Điều kiện sau**
Một phần tử Lop mới được thêm vào mảng con trỏ, với DsSV = NULL (chưa có sinh viên nào); sẵn sàng để dùng ở d/.

**6. Luồng chính**
1. GV chọn chức năng "Nhập lớp mới".
2. Hệ thống yêu cầu nhập MALOP.
3. Hệ thống kiểm tra: không rỗng, đúng 15 ký tự, không trùng mã lớp đã có, chưa vượt quá 10000 lớp.
4. Hệ thống yêu cầu nhập TENLOP.
5. Hệ thống kiểm tra TENLOP không rỗng.
6. Hệ thống tạo phần tử Lop mới (MALOP, TENLOP, DsSV = NULL), thêm vào mảng con trỏ tại vị trí trống kế tiếp.
7. Hệ thống thông báo thêm lớp thành công.

**7. Luồng thay thế**
- 3a. Nếu MALOP đã tồn tại → thông báo lỗi, quay lại bước 2 để nhập mã khác.
- 3b. Nếu đã đạt 10000 lớp → thông báo "Đã đạt giới hạn tối đa số lớp", không cho tiếp tục.

**8. Luồng ngoại lệ**
Người dùng nhập MALOP có khoảng trắng ở đầu/cuối → hệ thống tự động cắt bỏ (trim) trước khi kiểm tra trùng, tránh trường hợp "LOP01 " và "LOP01" bị coi là 2 mã khác nhau.

**9. Business Rule**
Không có Business Rule nghiệp vụ riêng ngoài các Validation Rule; đây là chức năng tạo mới thuần túy, không có ràng buộc về sửa/xóa (khác với e/, f/).

**10. Validation Rule**
MALOP: không rỗng, đúng 15 ký tự (C15), không trùng. TENLOP: không rỗng, không toàn khoảng trắng.

**11. Edge Case**
Số lớp đã đạt 10000 (giới hạn cứng của mảng con trỏ theo đề bài).

**12. Dữ liệu liên quan**
Lop (MALOP, TENLOP, DsSV).

**13. File liên quan**
`Class.cpp/h`; dữ liệu được ghi xuống file lớp khi GV chọn "Lưu dữ liệu" (không tự ghi file ngay sau mỗi lần thêm 1 lớp, theo BR-14).

**14. Giao diện Console đề xuất**
```
┌─ NHẬP LỚP MỚI ────────────────────┐
│ Mã lớp (15 ký tự)  : ___________  │
│ Tên lớp            : ___________  │
└────────────────────────────────────┘
✔ Đã thêm lớp thành công. (325/10000 lớp)
```

**15. Khả năng mở rộng**
Trở thành endpoint `POST /classes` khi mở rộng Web/REST API; mảng con trỏ có thể thay bằng `std::vector<Lop*>` hoặc bảng SQL `Lop` khi mở rộng sang Database.

**16. Ghi chú triển khai**
Vì giới hạn 10000 phần tử cố định theo đề, có thể cấp phát mảng con trỏ tĩnh `Lop* dsLop[10000]` và biến đếm `soLop` riêng để theo dõi số lớp hiện có, tránh phải duyệt toàn mảng tìm ô trống mỗi lần thêm.

---

## c/ IN DANH SÁCH LỚP

**1. Mục đích**
Cho phép Giáo viên xem toàn bộ danh sách các lớp hiện có trong hệ thống dưới dạng bảng.

**2. Mô tả chức năng**
Duyệt qua mảng con trỏ Lớp (từ vị trí 0 đến soLop-1), in ra MALOP, TENLOP và sĩ số (đếm số phần tử trong danh sách liên kết DsSV của từng lớp).

**3. Đối tượng sử dụng**
Giáo viên. (Có thể mở rộng cho Sinh viên xem danh sách lớp công khai — đề xuất ở Mục 15, không bắt buộc theo đề gốc vì đề chỉ ghi chức năng này chung, không giới hạn actor rõ ràng, nhưng theo BR-12 các thao tác quản trị mặc định thuộc về GV).

**4. Điều kiện trước**
Đã đăng nhập với vai trò Giáo viên.

**5. Điều kiện sau**
Danh sách lớp được hiển thị ra màn hình console, không làm thay đổi dữ liệu.

**6. Luồng chính**
1. GV chọn chức năng "In danh sách lớp".
2. Hệ thống duyệt mảng con trỏ Lớp từ đầu đến hết.
3. Với mỗi lớp, hệ thống đếm số sinh viên (duyệt danh sách liên kết DsSV).
4. Hệ thống in bảng gồm cột: Mã lớp, Tên lớp, Sĩ số.

**7. Luồng thay thế**
Không có luồng thay thế đáng kể — đây là chức năng chỉ đọc (read-only), không rẽ nhánh nghiệp vụ.

**8. Luồng ngoại lệ**
Chưa có lớp nào trong hệ thống → hiển thị thông báo "Chưa có lớp nào" thay vì bảng rỗng gây khó hiểu.

**9. Business Rule**
Không có (chức năng chỉ đọc).

**10. Validation Rule**
Không áp dụng (không có dữ liệu đầu vào từ người dùng).

**11. Edge Case**
Số lượng lớp lớn (gần 10000) → cân nhắc phân trang khi in trên console (vd in 20 lớp/trang, có phím xem tiếp) để tránh tràn màn hình.

**12. Dữ liệu liên quan**
Lop (đọc toàn bộ mảng con trỏ).

**13. File liên quan**
`Class.cpp/h`, `Report.cpp/h` (định dạng bảng in).

**14. Giao diện Console đề xuất**
(xem lại mẫu bảng ở Mục 12.4 — Đề xuất Table)

**15. Khả năng mở rộng**
Trở thành endpoint `GET /classes` khi mở rộng REST API; phân trang (pagination) là khái niệm chuẩn cần có sẵn khi chuyển qua API thật.

**16. Ghi chú triển khai**
Tách riêng hàm `demSiSo(Lop*) -> int` để tái sử dụng ở cả c/ và i/ (bảng điểm theo lớp cũng cần sĩ số).

---

## d/ NHẬP SINH VIÊN CỦA LỚP

**1. Mục đích**
Cho phép Giáo viên thêm sinh viên vào một lớp đã tồn tại, tạo tài khoản (MASV/password) để sinh viên đó có thể đăng nhập và thi sau này.

**2. Mô tả chức năng**
Giáo viên nhập mã lớp trước (để xác định lớp đích), sau đó nhập lần lượt thông tin từng sinh viên (MASV, HO, TEN, PHAI, password) cho đến khi dừng nhập. Mỗi sinh viên được thêm vào danh sách liên kết DsSV của đúng lớp đã chọn.

**3. Đối tượng sử dụng**
Giáo viên.

**4. Điều kiện trước**
Đã đăng nhập vai trò Giáo viên; lớp đích đã tồn tại (được tạo trước đó bằng b/).

**5. Điều kiện sau**
Một hoặc nhiều SinhVien mới được thêm vào cuối (hoặc đầu, tùy cài đặt) danh sách liên kết DsSV của lớp; mỗi SinhVien có DsDiemThi = NULL (chưa thi môn nào).

**6. Luồng chính**
1. GV chọn chức năng "Nhập sinh viên của lớp".
2. Hệ thống yêu cầu nhập MALOP.
3. Hệ thống tìm lớp theo MALOP trong mảng con trỏ; nếu tìm thấy, chuyển bước 4.
4. Hệ thống yêu cầu nhập lần lượt: MASV, HO, TEN, PHAI, password cho 1 sinh viên.
5. Hệ thống kiểm tra hợp lệ và không trùng MASV trong toàn hệ thống (không chỉ trong lớp này).
6. Hệ thống thêm SinhVien mới vào danh sách liên kết DsSV của lớp.
7. Hệ thống hỏi "Nhập tiếp sinh viên khác? (Y/N)"; nếu Y quay lại bước 4, nếu N kết thúc.

**7. Luồng thay thế**
- 3a. Nếu MALOP không tồn tại → thông báo lỗi "Lớp không tồn tại, vui lòng tạo lớp trước (b/)", kết thúc chức năng (theo BR-06).
- 5a. Nếu MASV đã tồn tại (ở bất kỳ lớp nào) → thông báo lỗi, yêu cầu nhập lại MASV khác cho sinh viên này (không hủy toàn bộ, chỉ nhập lại sinh viên hiện tại).

**8. Luồng ngoại lệ**
Người dùng nhấn dừng ngay từ sinh viên đầu tiên (không nhập ai) → không lỗi, chỉ đơn giản là lớp vẫn chưa có sinh viên.

**9. Business Rule**
BR-06 (phải nhập mã lớp trước, lớp phải tồn tại).

**10. Validation Rule**
MASV: không rỗng, đúng 15 ký tự (C15), không trùng toàn hệ thống. HO, TEN: không rỗng. PHAI: chỉ nhận giá trị hợp lệ đã định nghĩa. password: không rỗng.

**11. Edge Case**
Edge Case #6 (lớp không tồn tại), Edge Case #10 (MASV trùng).

**12. Dữ liệu liên quan**
Lop (đọc, để tìm lớp đích), SinhVien (tạo mới, ghi vào DsSV của Lop).

**13. File liên quan**
`Student.cpp/h`, `Class.cpp/h` (để tìm lớp theo mã).

**14. Giao diện Console đề xuất**
```
┌─ NHẬP SINH VIÊN VÀO LỚP ─────────────┐
│ Mã lớp: DH22TIN01                     │
│ ────────────────────────────────────  │
│ Mã SV      : ______________           │
│ Họ         : ______________           │
│ Tên        : ______________           │
│ Giới tính  : (Nam/Nữ)                 │
│ Mật khẩu   : ______________           │
│                                        │
│ Thêm sinh viên khác? (Y/N)            │
└────────────────────────────────────────┘
```

**15. Khả năng mở rộng**
Trở thành endpoint `POST /classes/{malop}/students` khi mở rộng REST API; có thể bổ sung import hàng loạt từ file Excel/CSV (đề xuất ở Mục 15).

**16. Ghi chú triển khai**
Kiểm tra trùng MASV cần duyệt qua **toàn bộ** các lớp (không chỉ lớp hiện tại) — nên cân nhắc dùng cấu trúc chỉ mục phụ (Mục 11.2) nếu số lượng sinh viên lớn, để tránh duyệt lồng 2 vòng (mảng lớp × danh sách sinh viên mỗi lớp) với độ phức tạp cao.

---

## e/ NHẬP MÔN HỌC (thêm / xóa / hiệu chỉnh)

**1. Mục đích**
Cho phép Giáo viên quản lý danh mục môn học — nền tảng để sau đó nhập câu hỏi thi (f/) và tổ chức thi (g/) theo từng môn.

**2. Mô tả chức năng**
Chức năng gồm 3 thao tác con: **Thêm** môn học mới vào cây BST; **Xóa** môn học (chỉ khi được phép — BR-04); **Hiệu chỉnh** thông tin môn học (chỉ cho sửa TENMH, không cho đổi MAMH vì là khóa chính).

**3. Đối tượng sử dụng**
Giáo viên.

**4. Điều kiện trước**
Đã đăng nhập vai trò Giáo viên.

**5. Điều kiện sau**
- Thêm: 1 nút mới được chèn vào cây BST môn học, giữ cây cân bằng.
- Xóa: nút môn học và toàn bộ danh sách câu hỏi thuộc môn đó bị loại khỏi hệ thống (chỉ khi hợp lệ).
- Hiệu chỉnh: TENMH của môn học được cập nhật.

**6. Luồng chính (Thêm)**
1. GV chọn "Quản lý môn học" → "Thêm môn học".
2. Hệ thống yêu cầu nhập MAMH.
3. Hệ thống kiểm tra không rỗng, đúng 15 ký tự, chưa tồn tại trong cây BST.
4. Hệ thống yêu cầu nhập TENMH, kiểm tra không rỗng.
5. Hệ thống chèn nút mới vào cây BST theo đúng vị trí (so sánh MAMH), thực hiện cân bằng lại cây nếu cần.
6. Thông báo thêm thành công.

**Luồng chính (Xóa)**
1. GV chọn "Quản lý môn học" → "Xóa môn học", nhập MAMH cần xóa.
2. Hệ thống tìm môn học trong cây BST.
3. Hệ thống kiểm tra: môn học có câu hỏi nào đã từng được thi hay chưa (theo cờ "đã sử dụng" — Mục 11.2).
4. Nếu **chưa có câu hỏi nào từng được thi** → cho phép xóa nút khỏi cây BST (kèm giải phóng toàn bộ danh sách câu hỏi của môn).
5. Nếu **đã có câu hỏi từng được thi** → từ chối xóa, thông báo lý do (BR-04).

**Luồng chính (Hiệu chỉnh)**
1. GV chọn "Quản lý môn học" → "Hiệu chỉnh", nhập MAMH cần sửa.
2. Hệ thống tìm môn học; nếu tìm thấy, hiển thị TENMH hiện tại.
3. GV nhập TENMH mới; hệ thống kiểm tra không rỗng.
4. Hệ thống cập nhật TENMH, giữ nguyên MAMH và toàn bộ danh sách câu hỏi.

**7. Luồng thay thế**
- 3a (Thêm). Nếu MAMH đã tồn tại → báo lỗi, yêu cầu nhập mã khác.
- (Xóa) Nếu MAMH không tồn tại → báo lỗi "Không tìm thấy môn học".
- (Hiệu chỉnh) Nếu người dùng cố tình nhập lại MAMH mới ở bước sửa → hệ thống từ chối vì MAMH là khóa chính bất biến, chỉ cho sửa TENMH.

**8. Luồng ngoại lệ**
Xóa môn học khi đang có sinh viên thi dở môn đó (tình huống hiếm nhưng cần phòng ngừa trong thiết kế đơn luồng — vì ứng dụng single-session nên về lý thuyết không xảy ra đồng thời, nhưng nếu mở rộng đa người dùng cần khóa (lock) môn học trong lúc đang có phiên thi).

**9. Business Rule**
BR-04 (không xóa môn học đã có sinh viên thi — hệ quả từ BR-03 vì môn chứa câu hỏi đã dùng).

**10. Validation Rule**
MAMH: không rỗng, đúng 15 ký tự, không trùng khi thêm, bất biến khi sửa. TENMH: không rỗng.

**11. Edge Case**
Môn học không tồn tại khi sửa/xóa (Edge Case #5).

**12. Dữ liệu liên quan**
MonHoc (cây BST), gián tiếp ảnh hưởng đến CauHoi (khi xóa môn kéo theo xóa toàn bộ câu hỏi của môn, nếu được phép).

**13. File liên quan**
`Subject.cpp/h`.

**14. Giao diện Console đề xuất**
```
┌─ QUẢN LÝ MÔN HỌC ─────────────────┐
│ 1. Thêm môn học                   │
│ 2. Xóa môn học                    │
│ 3. Hiệu chỉnh tên môn học         │
│ 0. Quay lại                       │
└────────────────────────────────────┘
```

**15. Khả năng mở rộng**
Trở thành nhóm endpoint `POST /subjects`, `DELETE /subjects/{mamh}`, `PUT /subjects/{mamh}` khi mở rộng REST API; cây BST vẫn giữ nguyên vai trò cấu trúc tra cứu trong bộ nhớ, độc lập với việc dữ liệu được lưu ở SQL hay file.

**16. Ghi chú triển khai**
Thao tác xóa nút trong BST cần xử lý đủ 3 trường hợp kinh điển (nút lá, nút có 1 con, nút có 2 con) và **giữ tính cân bằng** của cây sau khi xóa — nên viết hàm xóa BST tổng quát dùng chung, tách biệt khỏi kiểm tra Business Rule (kiểm tra BR-04 thực hiện **trước khi** gọi hàm xóa cây, không trộn logic).

---

*(Hết Phần 2b — tiếp theo Phần 2c: đặc tả chi tiết f/ Nhập câu hỏi thi, g/ Thi trắc nghiệm, h/ In chi tiết bài thi, i/ In bảng điểm lớp)*

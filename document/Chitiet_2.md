# TÀI LIỆU PHÂN TÍCH & THIẾT KẾ HỆ THỐNG THI TRẮC NGHIỆM (C++ CONSOLE)
### Phần 2c — Mục 14: Đặc tả chi tiết từng chức năng (f/ → i/)

*(Tiếp theo Phần 2b — giữ nguyên ký hiệu f/, g/, h/, i/ của đề gốc.)*

---

## f/ NHẬP CÂU HỎI THI

**1. Mục đích**
Xây dựng và duy trì ngân hàng câu hỏi cho từng môn học, làm dữ liệu nguồn để hệ thống rút ngẫu nhiên khi tổ chức thi (g/).

**2. Mô tả chức năng**
Giáo viên chọn môn học, sau đó nhập nội dung câu hỏi cùng 4 phương án A/B/C/D và đáp án đúng. Hệ thống tự sinh Id ngẫu nhiên cho câu hỏi (không cho nhập tay). Chức năng cũng cho phép xóa câu hỏi, nhưng **bị chặn nếu câu hỏi đã từng nằm trong 1 bài thi của sinh viên** (BR-03) — đây là ràng buộc quan trọng nhất của chức năng này theo đề gốc.

**3. Đối tượng sử dụng**
Giáo viên.

**4. Điều kiện trước**
Đã đăng nhập vai trò Giáo viên; môn học đích đã tồn tại (tạo trước bằng e/).

**5. Điều kiện sau**
- Thêm: 1 nút mới được thêm vào danh sách liên kết câu hỏi của đúng môn học, với Id duy nhất do hệ thống sinh.
- Xóa: nút câu hỏi bị loại khỏi danh sách liên kết (chỉ khi hợp lệ).

**6. Luồng chính (Thêm câu hỏi)**
1. GV chọn "Quản lý câu hỏi thi" → "Thêm câu hỏi", nhập MAMH.
2. Hệ thống tìm môn học trong cây BST; nếu tìm thấy, chuyển bước 3.
3. Hệ thống sinh 1 số ngẫu nhiên làm Id, kiểm tra Id này chưa tồn tại trong toàn bộ ngân hàng câu hỏi (dùng chỉ mục phụ — Mục 11.2); nếu trùng, sinh lại cho đến khi duy nhất.
4. GV nhập Nội dung, A, B, C, D.
5. GV nhập Đáp án đúng (A/B/C/D).
6. Hệ thống kiểm tra tất cả các trường không rỗng và Đáp án thuộc {A,B,C,D}.
7. Hệ thống thêm nút CauHoi mới (Id, Nội dung, A, B, C, D, Đáp án, cờ "đã sử dụng" = false) vào đầu/cuối danh sách liên kết câu hỏi của môn.
8. Thông báo thêm thành công, hiển thị Id vừa sinh.

**Luồng chính (Xóa câu hỏi)**
1. GV chọn "Quản lý câu hỏi thi" → "Xóa câu hỏi", nhập MAMH và Id câu hỏi cần xóa.
2. Hệ thống tìm câu hỏi theo Id trong danh sách liên kết của môn.
3. Hệ thống kiểm tra cờ "đã sử dụng" của câu hỏi.
4. Nếu cờ = false (chưa từng được thi) → cho phép xóa nút khỏi danh sách liên kết.
5. Nếu cờ = true (đã từng được thi) → từ chối xóa, thông báo lý do (BR-03).

**7. Luồng thay thế**
- 2a (Thêm). Nếu MAMH không tồn tại → thông báo lỗi, yêu cầu tạo môn học trước (e/).
- 2a (Xóa). Nếu Id không tồn tại trong môn đó → thông báo "Không tìm thấy câu hỏi".

**8. Luồng ngoại lệ**
Trường hợp cực hiếm: bộ sinh số ngẫu nhiên sinh liên tục nhiều lần đều trùng Id đã có (xác suất rất thấp nếu miền giá trị đủ lớn) → cần vòng lặp sinh lại có giới hạn số lần thử hợp lý, tránh vòng lặp vô hạn về mặt lý thuyết.

**9. Business Rule**
BR-03 (không xóa câu hỏi đã từng nằm trong 1 bài thi), BR-11 (Id do hệ thống sinh, đảm bảo duy nhất).

**10. Validation Rule**
Nội dung, A, B, C, D: không rỗng, không toàn khoảng trắng. Đáp án: chỉ nhận 1 trong {A, B, C, D} (chuẩn hóa in hoa).

**11. Edge Case**
Edge Case #9 (Id trùng do sinh ngẫu nhiên), Edge Case #11 (xóa câu hỏi đã từng thi).

**12. Dữ liệu liên quan**
CauHoi (thuộc danh sách liên kết của 1 MonHoc cụ thể).

**13. File liên quan**
`Question.cpp/h`, `Utils.cpp/h` (hàm sinh số ngẫu nhiên dùng chung).

**14. Giao diện Console đề xuất**
(xem mẫu Form ở Mục 12.5 — Đề xuất Form nhập liệu, phần "Thêm câu hỏi mới")

**15. Khả năng mở rộng**
Trở thành `POST /subjects/{mamh}/questions`, `DELETE /questions/{id}` khi mở rộng REST API; cờ "đã sử dụng" có thể thay bằng cột `da_su_dung BOOLEAN` khi mở rộng sang SQL.

**16. Ghi chú triển khai**
Cờ "đã sử dụng" nên được **đặt bằng true ngay tại thời điểm câu hỏi được chọn vào 1 lượt thi ở g/** (không phải chờ đến lúc thử xóa mới đi kiểm tra ngược lại toàn bộ điểm thi) — điều này giúp thao tác xóa ở f/ kiểm tra với độ phức tạp O(1) thay vì phải duyệt lại toàn bộ lịch sử thi.

---

## g/ THI TRẮC NGHIỆM

**1. Mục đích**
Cho phép Sinh viên thực hiện một lượt thi trắc nghiệm có bấm giờ đối với 1 môn học, với bộ câu hỏi được rút ngẫu nhiên từ ngân hàng câu hỏi của môn, và được hệ thống tự động chấm điểm sau khi nộp bài.

**2. Mô tả chức năng**
Trước khi bắt đầu, hệ thống hỏi Sinh viên: môn học muốn thi, số câu hỏi muốn thi, số phút làm bài. Sau đó hệ thống rút ngẫu nhiên đúng số câu yêu cầu (không trùng lặp) từ danh sách câu hỏi của môn, lần lượt hiển thị từng câu kèm đồng hồ đếm ngược, ghi nhận lựa chọn của sinh viên, và khi hết giờ hoặc sinh viên chủ động nộp bài, hệ thống chấm điểm, làm tròn và lưu vào danh sách Điểm thi của sinh viên.

**3. Đối tượng sử dụng**
Sinh viên (chỉ được thi cho chính tài khoản đang đăng nhập).

**4. Điều kiện trước**
- Đã đăng nhập vai trò Sinh viên.
- Môn học được chọn phải tồn tại và có ít nhất 1 câu hỏi (nên có tối thiểu bằng số câu SV yêu cầu — BR-07).
- Sinh viên **chưa từng thi môn này** (BR-02) — kiểm tra bằng cách duyệt DsDiemThi của sinh viên xem đã có bản ghi Mamh này chưa.

**5. Điều kiện sau**
- Thành công: 1 bản ghi DiemThi mới (Mamh, Diem đã làm tròn) được thêm vào DsDiemThi của sinh viên; các câu hỏi đã rút được đánh dấu cờ "đã sử dụng" = true (để phục vụ BR-03 ở f/); chi tiết bài làm (câu nào chọn gì) được lưu lại để phục vụ h/.
- Không hoàn tất (thoát giữa chừng — BR-13): không có bản ghi điểm nào được tạo, trạng thái vẫn là "Chưa thi".

**6. Luồng chính**
1. SV chọn "Vào thi trắc nghiệm".
2. Hệ thống hỏi MAMH muốn thi.
3. Hệ thống kiểm tra môn tồn tại và SV chưa thi môn này (BR-02); nếu hợp lệ, chuyển bước 4.
4. Hệ thống hỏi số câu hỏi muốn thi.
5. Hệ thống kiểm tra số câu ≤ tổng số câu hỏi hiện có của môn (BR-07); nếu hợp lệ, chuyển bước 6.
6. Hệ thống hỏi số phút làm bài.
7. Hệ thống kiểm tra số phút > 0 (BR-08); nếu hợp lệ, chuyển bước 8.
8. Hệ thống rút ngẫu nhiên đúng số câu yêu cầu từ danh sách câu hỏi của môn (không trùng lặp), đưa vào 1 danh sách/mảng tạm cho lượt thi này, đồng thời đánh dấu cờ "đã sử dụng" cho từng câu được chọn.
9. Hệ thống bắt đầu đếm ngược thời gian; lần lượt hiển thị từng câu hỏi kèm 4 phương án.
10. Với mỗi câu, sinh viên chọn 1 đáp án (A/B/C/D); hệ thống ghi nhận lựa chọn (không chấm ngay, để tránh lộ đáp án đúng/sai giữa chừng — trừ khi thiết kế cho phép xem ngay, tùy chính sách, mặc định đề không yêu cầu feedback tức thời).
11. Sau khi trả lời hết các câu (hoặc hết giờ), hệ thống tự động chuyển sang bước nộp bài.
12. Hệ thống so sánh từng lựa chọn của sinh viên với đáp án đúng, tính số câu đúng, quy đổi ra điểm theo thang điểm đã định nghĩa (vd thang 10), làm tròn 1 số sau dấu phẩy (BR-09).
13. Hệ thống tạo bản ghi DiemThi (Mamh, Diem) và thêm vào đầu/cuối DsDiemThi của sinh viên.
14. Hệ thống hiển thị kết quả (điểm số) cho sinh viên.

**7. Luồng thay thế**
- 10a. Sinh viên chủ động chọn "Nộp bài" trước khi trả lời hết hoặc trước khi hết giờ → chuyển thẳng đến bước 11 (các câu chưa trả lời được tính là sai/bỏ trống).
- 9a. Hết thời gian trong khi đang làm câu bất kỳ → hệ thống tự động khóa không cho chọn đáp án nữa, chuyển sang bước 11 (auto-submit theo BR-08).

**8. Luồng ngoại lệ**
- Sinh viên thoát chương trình đột ngột (đóng cửa sổ, mất điện) trước khi đến bước 12 → không có bản ghi DiemThi nào được tạo (BR-13); lần đăng nhập sau, môn này vẫn ở trạng thái "Chưa thi", sinh viên có thể thi lại từ đầu (không vi phạm BR-02 vì chưa từng có bản ghi điểm thành công).
- Số câu hỏi còn lại trong ngân hàng không đủ so với yêu cầu (Edge Case #2/#13) → hệ thống từ chối ở bước 5, không cho bắt đầu thi.

**9. Business Rule**
BR-02 (mỗi môn chỉ thi 1 lần), BR-03 (đánh dấu câu hỏi đã dùng), BR-07 (số câu ≤ số câu hiện có), BR-08 (auto-submit khi hết giờ), BR-09 (làm tròn điểm), BR-13 (thoát giữa chừng không tính điểm).

**10. Validation Rule**
Số câu hỏi thi: số nguyên dương, ≤ tổng số câu hiện có của môn. Số phút thi: số nguyên dương > 0. Lựa chọn đáp án của SV: chỉ nhận A/B/C/D (hoặc "không chọn"/bỏ trống nếu cho phép).

**11. Edge Case**
Edge Case #1 (môn chưa có câu hỏi), #2 (số câu yêu cầu > số câu hiện có), #3 (thời gian = 0), #4 (thoát giữa chừng), #12 (thi môn đã thi rồi), #13 (số câu hỏi ngân hàng ít hơn yêu cầu).

**12. Dữ liệu liên quan**
MonHoc (đọc), CauHoi (đọc để rút ngẫu nhiên, ghi cờ "đã sử dụng"), SinhVien (đọc), DiemThi (tạo mới).

**13. File liên quan**
`Exam.cpp/h` (logic chính), `Question.cpp/h`, `Score.cpp/h`, `Utils.cpp/h` (random, làm tròn), `Report.cpp/h` (hiển thị đề thi/timer).

**14. Giao diện Console đề xuất**
(xem mẫu Timer & Progress Bar ở Mục 12.7)
```
┌─ THIẾT LẬP LƯỢT THI ─────────────┐
│ Mã môn học     : ______           │
│ Số câu hỏi thi : ______           │
│ Số phút thi    : ______           │
└────────────────────────────────────┘
```

**15. Khả năng mở rộng**
Trở thành nhóm endpoint `POST /exam/start` (khởi tạo phiên thi, trả về danh sách câu hỏi + token phiên), `POST /exam/submit` (nộp bài) khi mở rộng REST API/Web/Mobile — đúng theo khuyến nghị tách Business Logic ở Mục 13.3 (không để hàm chấm điểm tự `cout` ra kết quả mà trả về struct kết quả).

**16. Ghi chú triển khai**
- Thuật toán rút ngẫu nhiên không trùng: duyệt danh sách liên kết câu hỏi ra 1 mảng con trỏ tạm, dùng thuật toán xáo trộn (Fisher–Yates shuffle) trên mảng tạm, sau đó lấy N phần tử đầu (N = số câu yêu cầu).
- Đồng hồ đếm ngược nên dùng `<chrono>` để đo thời gian thực tế đã trôi qua, không dựa vào đếm vòng lặp (vì tốc độ vòng lặp phụ thuộc CPU, không chính xác).
- Cân nhắc lưu "chi tiết bài làm" (câu hỏi nào, SV chọn gì, đúng/sai) ở một cấu trúc phụ liên kết với bản ghi DiemThi, để phục vụ chức năng h/ (in chi tiết) — đây là dữ liệu **được phép/khuyến khích** sinh viên tự thiết kế thêm theo đúng "Lưu ý" của đề gốc.

---

## h/ IN CHI TIẾT CÁC CÂU HỎI ĐÃ THI CỦA 1 MÔN HỌC CỦA 1 SINH VIÊN

**1. Mục đích**
Cho phép xem lại toàn bộ nội dung câu hỏi, phương án, đáp án đúng và lựa chọn của sinh viên trong một lượt thi đã hoàn tất, phục vụ việc phúc tra, đối chiếu, hoặc để sinh viên tự xem lại kết quả học tập.

**2. Mô tả chức năng**
Người dùng chỉ định 1 sinh viên (MASV) và 1 môn học (MAMH) mà sinh viên đó đã thi; hệ thống tra cứu bản ghi điểm tương ứng và in ra toàn bộ danh sách câu hỏi đã dùng trong lượt thi đó, kèm đáp án đúng và lựa chọn thực tế của sinh viên.

**3. Đối tượng sử dụng**
Giáo viên (xem của bất kỳ sinh viên nào); Sinh viên (chỉ xem của chính mình).

**4. Điều kiện trước**
Sinh viên đã có bản ghi DiemThi cho môn học được chỉ định (tức là đã thi xong môn đó theo g/).

**5. Điều kiện sau**
Danh sách chi tiết câu hỏi/lựa chọn được hiển thị; không làm thay đổi dữ liệu (chức năng chỉ đọc).

**6. Luồng chính**
1. Người dùng chọn "Xem chi tiết bài thi" (SV: tự động dùng MASV của chính mình; GV: được hỏi nhập MASV).
2. Hệ thống hỏi MAMH muốn xem.
3. Hệ thống tìm bản ghi DiemThi (MASV, MAMH); nếu tìm thấy, chuyển bước 4.
4. Hệ thống tra cứu chi tiết bài làm đã lưu (Mục 14-g, Ghi chú triển khai) ứng với lượt thi đó.
5. Hệ thống in từng câu hỏi: nội dung, 4 phương án, đáp án đúng, đáp án sinh viên đã chọn, đánh dấu Đúng/Sai.
6. Hệ thống in tổng kết: số câu đúng/tổng số câu, điểm số.

**7. Luồng thay thế**
- 3a. Nếu chưa có bản ghi điểm cho (MASV, MAMH) → thông báo "Sinh viên chưa thi môn này", không có gì để hiển thị.

**8. Luồng ngoại lệ**
Sinh viên cố gắng xem chi tiết bài thi của **sinh viên khác** → hệ thống từ chối do vi phạm phân quyền (BR-12 mở rộng: SV chỉ thao tác trên dữ liệu của chính mình).

**9. Business Rule**
Chỉ áp dụng cho lượt thi đã hoàn tất thành công (có bản ghi DiemThi); phân quyền xem theo actor (GV xem mọi SV, SV chỉ xem chính mình).

**10. Validation Rule**
MASV, MAMH nhập vào phải tồn tại trong hệ thống trước khi tra cứu tiếp.

**11. Edge Case**
Sinh viên chưa thi môn được hỏi (khác với Edge Case #14 vốn nói về bảng điểm lớp, đây là tra cứu đơn lẻ) → thông báo rõ ràng thay vì bảng trống.

**12. Dữ liệu liên quan**
DiemThi (tra cứu bản ghi), CauHoi (nội dung câu hỏi đã dùng — cần liên kết ngược từ chi tiết bài làm), SinhVien.

**13. File liên quan**
`Score.cpp/h`, `Question.cpp/h`, `Report.cpp/h`.

**14. Giao diện Console đề xuất**
```
┌─ CHI TIẾT BÀI THI — SV: 22110001 — Môn: THL301 ─────┐
│ Câu 1: Ngôn ngữ C++ hỗ trợ đa hình bằng cách nào?    │
│   A. Overload toán tử   B. Hàm ảo (virtual)          │
│   C. Template           D. Macro                     │
│   Đáp án đúng: B   |   SV chọn: B   → ĐÚNG            │
│ ...                                                   │
│ Tổng kết: 8/10 câu đúng — Điểm: 8.0                   │
└────────────────────────────────────────────────────────┘
```

**15. Khả năng mở rộng**
Trở thành `GET /students/{masv}/exams/{mamh}/detail` khi mở rộng REST API.

**16. Ghi chú triển khai**
Đòi hỏi hệ thống phải lưu **chi tiết bài làm** (không chỉ điểm số cuối cùng) tại thời điểm nộp bài ở g/ — đây chính là lý do khuyến nghị "sinh viên có thể tự thiết kế thêm danh sách" (theo đúng ghi chú của đề gốc) để lưu cặp (Id câu hỏi, đáp án SV chọn) cho mỗi lượt thi.

---

## i/ IN BẢNG ĐIỂM THI TRẮC NGHIỆM MÔN HỌC CỦA 1 LỚP

**1. Mục đích**
Cho Giáo viên cái nhìn tổng hợp về kết quả thi của toàn bộ sinh viên trong 1 lớp đối với 1 môn học cụ thể, phục vụ việc tổng kết, báo cáo.

**2. Mô tả chức năng**
Giáo viên chỉ định MALOP và MAMH; hệ thống duyệt qua toàn bộ sinh viên trong lớp, với mỗi sinh viên tra cứu xem đã có điểm cho môn đó chưa — nếu có thì in điểm, nếu chưa thì in "Chưa thi" (đúng yêu cầu đề gốc).

**3. Đối tượng sử dụng**
Giáo viên.

**4. Điều kiện trước**
Lớp (MALOP) và môn học (MAMH) đã tồn tại trong hệ thống.

**5. Điều kiện sau**
Bảng điểm được hiển thị đầy đủ cho **toàn bộ** sinh viên trong lớp (không bỏ sót sinh viên chưa thi); không làm thay đổi dữ liệu.

**6. Luồng chính**
1. GV chọn "In bảng điểm theo lớp", nhập MALOP.
2. Hệ thống tìm lớp trong mảng con trỏ; nếu tìm thấy, chuyển bước 3.
3. GV nhập MAMH muốn xem điểm.
4. Hệ thống kiểm tra môn học tồn tại; nếu hợp lệ, chuyển bước 5.
5. Hệ thống duyệt danh sách liên kết DsSV của lớp, với mỗi sinh viên:
   - Duyệt DsDiemThi của sinh viên đó tìm bản ghi có Mamh trùng MAMH đang xét.
   - Nếu tìm thấy → lấy Diem để in.
   - Nếu không tìm thấy → in "Chưa thi".
6. Hệ thống in bảng gồm cột: MASV, Họ tên, Điểm/Trạng thái.
7. Hệ thống có thể in thêm dòng tổng kết (số SV đã thi / chưa thi, điểm trung bình của số đã thi).

**7. Luồng thay thế**
Không có nhánh nghiệp vụ đặc biệt — đây là chức năng tổng hợp/báo cáo thuần túy (read-only).

**8. Luồng ngoại lệ**
- 2a. MALOP không tồn tại → thông báo lỗi "Lớp không tồn tại".
- 4a. MAMH không tồn tại → thông báo lỗi "Môn học không tồn tại".
- Lớp không có sinh viên nào (lớp vừa tạo bằng b/ nhưng chưa nhập SV bằng d/) → in thông báo "Lớp chưa có sinh viên" thay vì bảng rỗng.

**9. Business Rule**
BR-10 (sinh viên chưa thi phải ghi rõ "Chưa thi", không bỏ trống, không ghi điểm 0).

**10. Validation Rule**
MALOP, MAMH phải tồn tại trước khi thực hiện tra cứu.

**11. Edge Case**
Edge Case #14 (có sinh viên chưa từng thi khi in bảng điểm lớp) — đây chính là trường hợp trọng tâm của chức năng này.

**12. Dữ liệu liên quan**
Lop (đọc DsSV), SinhVien (đọc DsDiemThi của từng SV), MonHoc (kiểm tra tồn tại), DiemThi (đọc, không ghi).

**13. File liên quan**
`Class.cpp/h`, `Student.cpp/h`, `Score.cpp/h`, `Report.cpp/h`.

**14. Giao diện Console đề xuất**
```
┌─ BẢNG ĐIỂM — Lớp: DH22TIN01 — Môn: THL301 ──────────┐
│ Mã SV      │ Họ tên             │ Điểm / Trạng thái  │
├────────────┼────────────────────┼─────────────────────┤
│ 22110001   │ Nguyễn Văn A       │ 8.0                 │
│ 22110002   │ Trần Thị B         │ Chưa thi            │
│ 22110003   │ Lê Văn C           │ 6.5                 │
├────────────┴────────────────────┴─────────────────────┤
│ Đã thi: 2/3 — Điểm trung bình (đã thi): 7.3            │
└──────────────────────────────────────────────────────┘
```
(Dòng "Chưa thi" nên in màu vàng/nhạt — xem Mục 12.8)

**15. Khả năng mở rộng**
Trở thành `GET /classes/{malop}/subjects/{mamh}/scores` khi mở rộng REST API; ở quy mô SQL có thể dùng câu lệnh `LEFT JOIN` giữa bảng SinhVien và DiemThi để tự động lấy ra cả sinh viên chưa có điểm (thay vì phải tự viết vòng lặp kiểm tra như ở bản C++ file-based).

**16. Ghi chú triển khai**
Nên tách hàm `timDiemTheoMon(SinhVien*, mamh) -> DiemThi*` (trả về NULL nếu không tìm thấy) dùng chung cho cả h/ và i/, tránh viết trùng logic duyệt DsDiemThi ở 2 nơi khác nhau.

---

*(Hết Mục 14 — đã đặc tả đầy đủ 16 tiểu mục cho toàn bộ 9 chức năng a→i theo đúng ký hiệu gốc. Tiếp theo ở Phần 3: Mục 15 Chức năng đề xuất bổ sung, Mục 16 Phân tích độ khó, Mục 17 Gợi ý phân công nhóm, Mục 18 Tài liệu SRS hoàn chỉnh.)*

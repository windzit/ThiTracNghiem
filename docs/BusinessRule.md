# Quy Định & Quy Tắc Nghiệp Vụ Hệ Thống (ThiTracNghiem Business Rules)

Tài liệu quy định chi tiết toàn bộ các ràng buộc dữ liệu, quy tắc nghiệp vụ quản lý lớp học, sinh viên, môn học, ngân hàng câu hỏi, và quy trình tổ chức thi trắc nghiệm.

---

## 1. Quy Tắc Quản Lý Lớp Học (Class Rules)

1. **Mã Lớp (`MALOP`)**:
   - Tối đa 15 ký tự, viết hoa, không khoảng trắng, không chứa ký tự gạch đứng `|`.
   - **Tính Duy Nhất**: Mã lớp phải là duy nhất trên toàn hệ thống.
2. **Tên Lớp (`TENLOP`)**: Không được rỗng, tự động chuẩn hóa khoảng trắng thừa.
3. **Ràng Buộc Xóa Lớp**: Chỉ được phép xóa Lớp học khi **Sĩ số sinh viên của lớp bằng 0**.

---

## 2. Quy Tắc Quản Lý Sinh Viên (Student Rules)

1. **Mã Sinh Viên (`MASV`)**:
   - Tối đa 15 ký tự, không chứa khoảng trắng hay ký tự điều khiển.
   - **Tính Duy Nhất Toàn Cục (Global Uniqueness)**: Mã sinh viên phải duy nhất trên toàn bộ các lớp học (không được có 2 sinh viên trùng mã ở 2 lớp khác nhau).
2. **Mật Khẩu (`password`)**: Bảo toàn 100% nguyên vẹn (preserves leading/trailing spaces, không bị trim hay normalize).
3. **Giới Tính (`PHAI`)**: Chỉ chấp nhận giá trị `"Nam"` hoặc `"Nu"`.
4. **Ràng Buộc Xóa Sinh Viên**:
   - Không được phép xóa sinh viên nếu sinh viên đó **đã có ít nhất 1 bài thi/điểm thi** trong hệ thống.
   - Xóa hàng loạt (`bulk-delete`): Chỉ thành công khi tất cả sinh viên được chọn đều chưa có điểm thi.

---

## 3. Quy Tắc Quản Lý Môn Học (Subject Rules)

1. **Mã Môn Học (`MAMH`)**:
   - Tối đa 15 ký tự, viết hoa, không chứa khoảng trắng.
   - Được quản lý trong Cây Tìm Kiếm Nhị Phân (BST) sắp xếp theo `MAMH`.
2. **Cờ Đã Sử Dụng (`used`)**: Khi một môn học có câu hỏi được đưa vào bài thi của sinh viên, cờ `used` của môn học chuyển thành `true`.
3. **Ràng Buộc Xóa Môn Học**: Không được phép xóa môn học nếu `used == true` hoặc môn học đang chứa các câu hỏi đã được dùng trong bài thi.

---

## 4. Quy Tắc Ngân Hàng Câu Hỏi (Question Rules)

1. **Nội Dung & Các Phương Án**:
   - Nội dung câu hỏi và 4 phương án A, B, C, D không được để rỗng.
   - **Tính Phân Biệt**: Các phương án A, B, C, D của 1 câu hỏi phải khác nhau sau khi chuẩn hóa.
   - **Đáp Án Đúng (`DAPAN_DUNG`)**: Chỉ nhận một trong 4 ký tự `'A'`, `'B'`, `'C'`, `'D'`.
2. **Tự Động Sinh Mã ID**: Mã ID câu hỏi là số nguyên dương tự động tăng (`Auto-Increment`).
3. **Cơ Chế Xóa Lai (Hybrid Deletion)**:
   - **Xóa Cứng (`removeNode`)**: Nếu câu hỏi chưa từng xuất hiện trong bài thi nào (`used == false`), cho phép xóa vĩnh viễn khỏi danh sách liên kết.
   - **Xóa Mềm (`setDeleted`)**: Nếu câu hỏi đã từng được thi (`used == true`), không xóa khỏi bộ nhớ mà đánh dấu `deleted = true`. Câu hỏi bị xóa mềm sẽ không bao giờ xuất hiện trong các bài thi mới, nhưng vẫn hiển thị chính xác trong lịch sử xem lại bài thi cũ của sinh viên.
4. **Xóa Hàng Loạt**: API xóa hàng loạt (`bulk-delete`) kiểm tra nếu có câu hỏi đã `used`, hệ thống tự động chuyển sang xóa mềm cho câu hỏi đó.

---

## 5. Quy Tắc Tổ Chức Thi Trắc Nghiệm (Exam Execution Rules)

1. **Đăng Ký & Điều Kiện Thi**:
   - Sinh viên chỉ được thi môn học khi môn học đó có đủ số lượng câu hỏi khả dụng (`deleted == false`).
   - Mỗi sinh viên **chỉ được thi tối đa 1 lần** cho mỗi môn học (trừ khi Giảng viên xóa điểm thi đó).
2. **Thuật Toán Trộn Câu Hỏi (Fisher-Yates Shuffle)**:
   - Khi sinh viên bắt đầu thi (`POST /api/exams/start`), hệ thống rút ngẫu nhiên `socauthi` câu hỏi từ ngân hàng câu hỏi môn học bằng giải thuật Fisher-Yates shuffle.
3. **Tự Động Luân Chuyển & Phục Hồi Phiên Thi (Session Resume)**:
   - Mỗi khi chọn câu trả lời, frontend tự động đồng bộ về backend (`PUT /api/exams/answer`).
   - backend lưu phiên thi vào `exam_sessions.txt`. Nếu mất kết nối/sập nguồn, khi đăng nhập lại, phiên thi tự động được khôi phục kèm thời gian làm bài chính xác.
4. **Chấm Điểm Tự Động & Làm Tròn**:
   - Điểm số = `(Số câu trả lời đúng / Tổng số câu thi) * 10.0`.
   - Điểm thi được làm tròn chính xác **2 chữ số thập phân** (ví dụ: `8.33`, `9.50`).
5. **Chế Độ Giám Sát Fullscreen**:
   - Khi cài đặt `fullscreenRequired == true`, sinh viên bắt buộc làm bài ở chế độ toàn màn hình. Rời khỏi toàn màn hình quá số lần cho phép sẽ bị hủy/nộp bài tự động.

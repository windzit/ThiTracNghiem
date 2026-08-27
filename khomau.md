# ⚔️ BỘ 65 CÂU HỎI VẤN ĐÁP "KHÔ MÁU" - TRUY SÁT TẬN GỐC CODEBASE
**Mục tiêu:** Bóc tách từng dòng code em đã viết. Yêu cầu giải thích bản chất, viết lại mã giả (pseudo-code) và bảo vệ cách tối ưu của mình.

---

## 🛑 PHẦN 1: THUẬT TOÁN MÔN HỌC (CÂY BST) & CÂU HỎI (DSLK) (20 Câu)

### 📌 Cấu trúc Môn Học (`src/Subject.cpp`)
**1. [Code Cốt lõi - Đệ quy vs Vòng lặp]**  
Trong `_insert`, em đang dùng Đệ quy. Hãy viết lại thuật toán này bằng **Vòng lặp `while`**. Tại sao vòng lặp `while` lại an toàn tuyệt đối so với đệ quy khi cây BST bị lệch hẳn về 1 bên 1000 node?

**2. [Xoáy sâu Code - Xóa Node 2 con]**  
Trong hàm `_remove`, khi gán đè `node->data = temp->data;`, biến `data` chứa con trỏ `dsCHT`. Việc gán đè `=` này có gọi Copy Constructor không? Nó có làm rò rỉ (leak) toàn bộ danh sách câu hỏi gốc của `node` bị xóa không?

**3. [Bóc phốt - Clear Cây]**  
Hàm `Subject::clear(NodeMH*& node)` dùng duyệt **Post-order** (Trái $\rightarrow$ Phải $\rightarrow$ Node). Hãy thử đổi thành **Pre-order** (xóa cha trước rồi đệ quy xóa 2 con). Chương trình báo lỗi bộ nhớ gì? Tại sao?

**4. [Tối ưu cực hạn - Biến cờ `used`]**  
Nếu Thầy cấm dùng biến cờ `used` để tiết kiệm RAM, hãy viết thuật toán duyệt $O(N)$ kiểm tra xem môn học đó đã được phép xóa hay chưa (tức là không có câu hỏi nào từng ra thi).

**5. [Trường hợp - Deep Copy Cây]**  
Em có hàm `clone` để copy cây. Nếu không có hàm này (Shallow Copy), khi truyền cây vào một tham số Pass-by-Value `void printTree(Subject s)`, lúc hàm kết thúc, chuyện kinh hoàng gì sẽ xảy ra với cây gốc?

**6. [Lỗi C++ Căn bản - So sánh chuỗi]**  
Khi tìm vị trí chèn node BST, em dùng `strcmp(node->data.MAMH, monhoc.MAMH)`. Tại sao không dùng toán tử `==` hay `<`? Nếu mảng char `MAMH` quên không có ký tự kết thúc chuỗi `\0`, hàm `strcmp` sẽ gây ra lỗi gì?

**7. [Trường hợp - Xóa Node Gốc]**  
Khi môn học cần xóa chính là Node Gốc (Root) và nó chỉ có 1 cây con trái. Code của em cập nhật lại con trỏ `root` của toàn bộ cây như thế nào? Viết dòng code đó ra.

**8. [Hỏi vặn - Toán tử gán `operator=`]**  
Trong `Subject::operator=`, em gọi `clear(root)` rồi `clone(other.root)`. Nếu em viết mã ngu ngốc gán chính nó cho nó (`monHocA = monHocA;`), hàm này sẽ tự tay xóa sạch dữ liệu trước khi kịp copy không? Em đã chặn lỗi self-assignment này thế nào?

**9. [Độ cao của Cây]**  
Nếu Thầy yêu cầu In ra độ sâu (chiều cao) lớn nhất của cây BST Môn học, hãy viết nhanh mã giả đệ quy để tính toán.

**10. [Tìm kiếm Lặp vs Đệ quy]**  
Hàm `Subject::find(MAMH)` của em dùng đệ quy hay vòng lặp? Nếu dùng vòng lặp, hãy phân tích tại sao nó tốn $O(1)$ Space Complexity trong khi đệ quy tốn $O(\log N)$ Space Complexity?

### 📌 Danh sách Câu Hỏi Thi (`src/Question.cpp` & `dsCauHoi`)
**11. [Code Cốt lõi - Chèn cuối O(1)]**  
Trong `dsCauHoi`, em thêm phần tử vào cuối với thời gian $O(1)$. Khi DSLK đang rỗng (`head == nullptr`), thuật toán cập nhật `tail` xử lý khác với khi đã có phần tử như thế nào?

**12. [Thuật toán sinh đề - Random không trùng]**  
DSLK đơn không dùng được index `arr[i]`. Em bốc ngẫu nhiên $N$ câu hỏi từ danh sách $M$ câu bằng cách nào để không bị trùng lặp? (Gợi ý: Mảng con trỏ + Fisher-Yates Shuffle).

**13. [Hỏi vặn - Hủy Node giữa DSLK]**  
Xóa câu hỏi ID `55`, code cập nhật `prev->next = cur->next; delete cur;`. Nếu câu hỏi `55` vô tình là node **Cuối cùng** (`tail`), em cập nhật `tail` ra sao? Nếu quên cập nhật `tail`, lần chèn tiếp theo sẽ bị lỗi gì?

**14. [Quản lý Memory - DSLK Điểm Thi]**  
Khi một sinh viên bị xóa, vòng lặp `while` trong Destructor của `dsDiemThi` hoạt động thế nào để `delete` từng node? Nếu quên viết Destructor, RAM có tự dọn không?

**15. [So sánh Mảng vs DSLK]**  
Tại sao Đề bài quy định Lớp học là **Mảng con trỏ** nhưng Sinh viên và Câu hỏi lại là **DSLK Đơn**? Sự tối ưu bộ nhớ khi `swap` sắp xếp lớp học bằng con trỏ so với mảng thực thể là gì?

**16. [Singly vs Doubly Linked List]**  
Nếu Thầy bắt em đổi danh sách câu hỏi thành Danh sách liên kết Kép (Doubly LL), thao tác xóa một node (khi đã biết con trỏ trỏ tới node đó) sẽ giảm từ $O(N)$ xuống $O(1)$ như thế nào? Tại sao em vẫn chọn Singly LL?

**17. [Gài bẫy Copy DSLK]**  
Khi một struct `MonHoc` bị gán `=` cho một struct khác. Biến `dsCauHoi` bên trong (là DSLK đơn) sẽ được copy nông (con trỏ `head` chép qua `head`) hay copy sâu (tạo Node mới)? Nếu copy nông, chuyện gì xảy ra khi xóa 1 môn?

**18. [Chuyển đổi Mảng Động]**  
Nếu Thầy thay `dsCauHoi` bằng mảng động `DArray<Question>`. Khi mảng đầy và phải nới rộng (Resize $\times 2$), thao tác copy các phần tử cũ sang mảng mới tốn Big-O bao nhiêu? 

**19. [Tính toàn vẹn của Tail]**  
Nếu Thầy dùng code ngoài luồng can thiệp trực tiếp vào con trỏ `next` của một node giữa danh sách để cắt đứt danh sách, con trỏ `tail` của em có tự động nhận biết được đuôi mới không?

**20. [Duyệt Tìm Min/Max]**  
ID câu hỏi là tự tăng toàn cục. Vậy câu hỏi nằm ở `tail` luôn có ID lớn nhất đúng không? Có trường hợp nào câu hỏi ở `head` lại có ID lớn hơn câu hỏi ở `tail` không?

---

## 💾 PHẦN 2: BẢO TOÀN DỮ LIỆU ĐĨA, INDEX & VALIDATION (15 Câu)

**21. [Mổ xẻ Code - Thuật toán `atomicWriteFile`]**  
Lệnh `fs::rename(tempPath, targetPath);` an toàn thế nào nếu cúp điện ngay lúc đang ghi file `tempPath`? File gốc `targetPath` có bị lỗi hỏng không?

**22. [Hỏi xoáy - Offset File Text]**  
Bản ghi cố định 736 byte. Thay vì dùng `file.seekg(offset)`, nếu Thầy dùng vòng lặp `getline` đọc $10,000$ dòng để tìm câu hỏi cuối, hệ thống chậm đi bao nhiêu lần?

**23. [Thuật toán Indexing - HashTable]**  
Bảng băm `HashTable<K, V>`. Hàm Băm biến `MASV` thành Index mảng như thế nào? Kỹ thuật giải quyết đụng độ (Collision) em xài là Chaining hay Open Addressing? 

**24. [Trường hợp HĐH - Chuẩn hóa CRLF]**  
Windows ngắt dòng bằng `\r\n` (2 bytes), Linux bằng `\n` (1 byte). Cấu trúc Fixed-Length Record của em có rủi ro bị lệch cột khi đem sang chấm trên macOS của Thầy không? Khắc phục thế nào?

**25. [Kiểm toán Code - StorageValidator]**  
Bài test `TC_S04` bẫy lỗi đáp án trùng (`"Dap an A"` và `"dap an a"`). Trình bày thuật toán so sánh In-case-sensitive của em. Tốn bao nhiêu vòng lặp?

**26. [Xử lý đường dẫn - PathResolver]**  
Hàm `PathResolver` của em dùng API nào của Windows (VD: `GetModuleFileNameW`) để lấy thư mục gốc của file `.exe`? Tại sao không dùng đường dẫn tương đối `./storage/`?

**27. [Struct Padding C++]**  
Kích thước thực tế trên file của Môn học là 71 Bytes. Nếu em dùng lệnh `sizeof(MonHoc)` trong C++, kết quả có ra đúng 71 Bytes không hay bị to hơn do Struct Padding? Tại sao không được dùng lệnh `fwrite(&monhoc, sizeof(MonHoc))` lưu nhị phân?

**28. [Tối ưu Map Index]**  
Khi gọi `IndexManager::removeQuestionOffset`, nó gọi hàm `erase()` của HashTable. Độ phức tạp xóa trong Bảng băm là bao nhiêu? Nếu truyền vào ID không tồn tại thì sao?

**29. [Concurrency Ghi đĩa]**  
Hàm `atomicWriteFile` có Thread-safe (an toàn đa luồng) không nếu 2 luồng cùng lúc gọi ghi vào file `questions.txt`? 

**30. [Chuẩn hóa Ký tự rác]**  
Thuật toán `StringNormalizer::trim` xử lý thế nào nếu truyền vào chuỗi chỉ chứa toàn `\r\n\t` (không có chữ nào)? Có bị lỗi Out-of-bounds không?

**31. [Validation Float Exception]**  
Điểm thi là số thực float `0.0 - 10.0`. Khi dùng `std::stof` đọc từ file, nếu file bị lỗi chứa chữ `"ABC"`, hàm `stof` văng exception `std::invalid_argument`. Em đã bao bọc `try-catch` như thế nào để web server không bị crash?

**32. [Singleton Pattern]**  
`IndexManager` và `StorageManager` thiết kế theo mẫu Singleton. Singleton là gì? Hàm `getInstance()` trả về Reference `&` hay Pointer `*`?

**33. [Chế độ đọc file ios]**  
Khi mở file txt, em dùng `std::ios::in`. Tại sao không dùng `std::ios::binary`? Đặc tính tự động chuyển `\r\n` thành `\n` của Text Mode giúp ích gì cho thuật toán tính Offset của em?

**34. [Lỗi Khởi tạo lần đầu]**  
Khi khởi chạy Server lần đầu tiên trên máy trống (không có thư mục `/storage/`), các lệnh `std::ifstream` sẽ bị `fail()`. Server của em có tự động `mkdir` và tạo file rỗng không hay văng lỗi sập chương trình?

**35. [Vấn đề Xóa mềm & Nén đĩa]**  
Trạng thái `STATUS_DELETED` = `'1'`. Nếu trong file có quá nhiều bản ghi bị xóa mềm, dung lượng ổ đĩa phình to. Em dùng chiến thuật "Nén đĩa" (Compaction) như thế nào? (Chép các bản ghi active sang file mới hay ghi đè In-place?)

---

## ⚙️ PHẦN 3: LUỒNG TÍNH TOÁN CHỨC NĂNG (A $\rightarrow$ I) & ĐA LUỒNG (15 Câu)

**36. [Phân quyền - Chức năng a]**  
Kiểm tra `username == "GV"` thì có toàn quyền. Nếu là Sinh viên, tra cứu `MASV` trên Bảng băm tốn $O(1)$. Token JWT tạo ra chứa thông tin gì để chặn sinh viên lấy Token này gọi API của Giáo viên?

**37. [Bảo vệ đa luồng - Read/Write Lock trên BST]**  
Cơ chế khóa `DB_READ_LOCK`, `DB_WRITE_LOCK` chống Race Condition như thế nào? Nếu 100 SV đang thi (Read), 1 GV thêm Câu hỏi (Write), luồng Write có bị Blocki chờ 100 luồng Read xong không?

**38. [Không cho đổi MAMH trong chức năng e]**  
Vì sao chức năng Hiệu chỉnh cấm sửa `MAMH`? Nếu sửa trực tiếp giá trị Key của 1 node đang nằm lưng chừng cây BST, trật tự $Left < Node < Right$ sẽ nát bét thế nào?

**39. [Chuyển đổi dữ liệu - BST sang Mảng]**  
Để trả về danh sách môn học qua API, em biến Cây phi tuyến thành Mảng JSON. Thuật toán gom node tốn Big-O thời gian và không gian bao nhiêu? 

**40. [Ràng buộc xóa môn học]**  
Môn học chỉ được xóa khi: `used == false` hoặc danh sách câu hỏi rỗng. Giải thích phép kiểm tra $O(1)$ này trong logic `Subject::removeSubject`.

**41. [Tạo ID Tự tăng không trùng (Chức năng f)]**  
Lấy giá trị Max ID lúc khởi động bằng cách duyệt cây tìm kiếm $O(N)$ hay lưu vào biến Global tĩnh? Làm sao `std::atomic<int>` chống được 2 GV cấp trùng ID cùng 1 miligiây?

**42. [In Bảng điểm Lớp (Chức năng i)]**  
Thuật toán In bảng điểm 1 lớp: Duyệt mảng Lớp $\rightarrow$ Duyệt DSLK SV $\rightarrow$ Tra cứu Điểm `MAMH`. Vòng lặp lồng nhau này tốn $O(S \times D)$. Em dùng cách nào để tối ưu tốc độ tra cứu?

**43. [Đột phá tối ưu $O(1)$ xem điểm]**  
Khi SV tự xem điểm, dùng `findStudentGlobal(masv)` qua Bảng băm để trỏ thẳng tới `sv->dsdiemthi`. Đánh giá băng thông CPU tiết kiệm được so với việc duyệt toàn bộ lớp (Cách ở chức năng i).

**44. [Tính toán dấu phẩy động - Chấm điểm]**  
Hàm tính: `(soCauDung / tongSoCau) * 10.0`. Nếu `soCauDung` và `tongSoCau` đều là số nguyên (int), phép chia có bị làm tròn về `0` trước khi nhân 10 không? Em ép kiểu (Type Casting) thế nào?

**45. [Bù giờ Cúp điện - Khôi phục Session]**  
Downtime Gap = 300s. Hàm `adjustDowntime` cộng thẳng vào `thoiGianBatDau`. Phép tính này khiến Frontend trình duyệt xử lý tiếp bộ đếm ngược ra sao mà không làm sai lệch số phút còn lại?

**46. [Con trỏ Toàn cục ServerContext]**  
`ServerContext::getGlobalSubjects()` trả về Reference tới cây BST. Ai là người sở hữu vùng nhớ này? Việc Share chung vùng nhớ BST trên toàn bộ các Request Thread tiềm ẩn rủi ro Crash nào nếu không có Mutex?

**47. [Thuật toán Phân trang DSLK]**  
Khi trả về danh sách câu hỏi, làm sao em phân trang (Pagination) nhảy tới trang thứ 10 trong Singly Linked List? Việc duyệt `next` 100 lần so với Mảng `arr[100]` chênh lệch tốc độ ra sao?

**48. [Routing C++]**  
Thư viện `httplib` định tuyến `/api/questions/:id` bằng Regex hay Match chuỗi cơ bản? Làm sao em trích xuất ra được biến `id` kiểu số nguyên từ chuỗi URL này?

**49. [Sinh đề Hết Câu Hỏi]**  
Thuật toán Random chọn 50 câu. Nếu môn đó chỉ có đúng 50 câu, thuật toán random của em có bị kẹt vào vòng lặp vô tận (Infinite Loop) do cố tìm câu không trùng không? Thuật toán Fisher-Yates giải quyết việc này thế nào?

**50. [Chống Duplicate Submit]**  
Làm sao API nộp bài thi chặn được việc 1 sinh viên gửi Request "Nộp bài" 2 lần liên tiếp (do lỗi mạng hoặc cố tình double-click)? 

---

## 🌐 PHẦN 4: HỎI ĐÁP TOÀN DIỆN, BÓC PHỐT KIẾN TRÚC & BẢO MẬT (15 Câu)

**51. [Mối liên kết 5 CTDL Core]**  
Mô tả liên kết: `Mảng con trỏ Lop*` $\rightarrow$ `DSLK SinhVien` $\rightarrow$ `DSLK DiemThi`. Dữ liệu Điểm thi (chỉ chứa `MAMH`) ánh xạ với Cây Môn học bằng cách Lookup Bảng băm hay duyệt Cây?

**52. [Memory Leak - Giải phóng đệ quy]**  
Khi hủy chương trình, logic giải phóng đệ quy chạy từ Node SV $\rightarrow$ Node Điểm thi diễn ra ra sao? Từ khóa `delete` trong vòng lặp `while` gọi Destructor như thế nào?

**53. [Mảng con trỏ 10,000 Lớp]**  
Khai báo `Lop* dslop[10000]`. Tại sao lại tốn thêm RAM lưu con trỏ thay vì lưu `Lop` thẳng? Phép gán `dslop[i] = dslop[j]` đổi chỗ lớp nhanh hơn phép gán thực thể bao nhiêu lần?

**54. [Deep Copy vs Shallow Copy Cốt Lõi]**  
Bản chất lỗi Double Free / Dangling Pointer: Xóa 1 đối tượng đã copy nông khiến con trỏ bên trong bị `delete` theo. Đối tượng còn lại trỏ vào vùng nhớ rác. Khắc phục bằng Code C++ nào?

**55. [Tự code CTDL không dùng STL]**  
Không dùng `std::vector` hay `std::map`. Hãy nêu 1 nhược điểm tồi tệ nhất của mảng động tự build `DArray` so với `std::vector` chuẩn (Hint: Move Semantics, std::allocator).

**56. [Kích thước bộ nhớ tối đa - Tính toán]**  
10.000 lớp + 10.000 sinh viên + 10.000 câu hỏi. Mỗi câu 736 byte $\approx$ 7.3MB. Tổng RAM In-Memory Database em ước tính tối đa bao nhiêu MB? Nếu vượt quá RAM, máy tính sẽ văng lỗi C++ `std::bad_alloc` đúng không?

**57. [Destructor Cây BST - Tại sao Post-order?]**  
Hủy cây bắt buộc phải duyệt Post-order (Con Trái $\rightarrow$ Con Phải $\rightarrow$ Xóa Cha). Nếu xóa Cha trước, làm sao truy cập 2 con để xóa tiếp?

**58. [Bóc phốt Kiến trúc]**  
Trong 5 CTDL gốc, cấu trúc nào khó code nhất, nhiều rủi ro sập Server nhất vì thao tác con trỏ chằng chịt? (Gợi ý: Thao tác bứt node có 2 con của Cây BST).

**59. [Hack SQL Injection & XSS]**  
Hệ thống C++ dùng File Text (Flat-file DB). Mã `MASV` = `1' OR '1'='1` có phá sập hệ thống không? (Không, vì đâu có SQL). Vậy làm sao ngăn chặn sinh viên nhập thẻ `<script>alert(1)</script>` vào Tên lớp gây XSS trên trình duyệt?

**60. [Shared_Mutex RAII]**  
Trong C++, `std::shared_lock<std::shared_mutex> lock(mutex);`. Việc dùng cấp phát đối tượng `lock` để giữ khóa (Khái niệm RAII) giúp chống Deadlock thế nào nếu bên trong code văng Exception?

**61. [Resizing DArray Memory]**  
Mảng động `DArray`. Khi full capacity, lệnh `new T[capacity * 2]` chạy xong. Em chép dữ liệu cũ qua mảng mới bằng vòng lặp gán `=` hay lệnh `memcpy`? Tại sao dùng `memcpy` đối với C++ Object (chứa String/Con trỏ) là thảm họa?

**62. [Dangling Pointer ở Mảng Lớp]**  
Lớp thứ 10 bị xóa: `delete dslop[10];`. Nhưng nếu em quên gán `dslop[10] = nullptr;`. Nó trở thành Dangling Pointer (Con trỏ hoang/lơ lửng). Vòng lặp in danh sách lớp chạy qua sẽ văng lỗi gì?

**63. [Toán tử tăng con trỏ C++]**  
Mảng con trỏ `Lop** ptr = dslop; ptr++;`. Lệnh `ptr++` dịch chuyển vùng nhớ đi bao nhiêu Byte trên hệ điều hành 64-bit? (8 Bytes). 

**64. [Cross-Platform]**  
Web Server này code bằng Visual Studio trên Windows. Nếu mang source code qua hệ điều hành Ubuntu (Linux) dùng bộ biên dịch GCC để build, file nào sẽ báo lỗi đầu tiên do gọi API đặc thù của Microsoft?

**65. [HỎI CHỐT HẠ KẾT LÚC]**  
Nếu bây giờ có 1 nhà đầu tư rót 1 Tỷ VNĐ yêu cầu em scale hệ thống thi trắc nghiệm này lên cho 1 Triệu Sinh Viên thi cùng lúc. Điểm "Thắt cổ chai" (Bottleneck) lớn nhất của kiến trúc C++ File-Text hiện tại nằm ở đâu? Đập đi xây lại cái gì đầu tiên?

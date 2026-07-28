@echo off
echo === Seeding PTIT Sample Data ===
echo.
echo [0] Stopping any running server instance...
taskkill /f /im server.exe > nul 2>&1
ping 127.0.0.1 -n 2 > nul

set SERVER_EXE=build\Release\server.exe
if not exist %SERVER_EXE% set SERVER_EXE=build\Debug\server.exe
if not exist %SERVER_EXE% set SERVER_EXE=server.exe

echo [1] Resetting storage directory to clean default state...
%SERVER_EXE% --reset-storage

echo [2] Starting fresh server for seeding...
start /b %SERVER_EXE% > seed_server.log 2>&1
ping 127.0.0.1 -n 3 > nul

echo [2] Creating 4 classes...
curl -s -X POST http://localhost:8080/api/classes -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"tenlop\":\"Lop CNTT 1 Khoa 2022\"}" > nul
curl -s -X POST http://localhost:8080/api/classes -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"tenlop\":\"Lop CNTT 2 Khoa 2022\"}" > nul
curl -s -X POST http://localhost:8080/api/classes -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"tenlop\":\"Lop Vien thong 1 Khoa 2022\"}" > nul
curl -s -X POST http://localhost:8080/api/classes -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"tenlop\":\"Lop An toan thong tin 1 Khoa 2022\"}" > nul
echo   Done: 4 classes

echo [3] Creating 4 subjects...
curl -s -X POST http://localhost:8080/api/subjects -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"tenmh\":\"Lap trinh C++\"}" > nul
curl -s -X POST http://localhost:8080/api/subjects -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"tenmh\":\"Cau truc du lieu va giai thuat\"}" > nul
curl -s -X POST http://localhost:8080/api/subjects -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"tenmh\":\"Co so du lieu\"}" > nul
curl -s -X POST http://localhost:8080/api/subjects -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"tenmh\":\"Mang may tinh\"}" > nul
echo   Done: 4 subjects

echo [4] Creating 50 students (N22DCCN001-N22DCCN050)...
REM Students 1-13 -> D22CQCN01-N
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN001\",\"ho\":\"Nguyen Van\",\"ten\":\"An\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN002\",\"ho\":\"Le Thi\",\"ten\":\"Binh\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN003\",\"ho\":\"Tran Van\",\"ten\":\"Cuong\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN004\",\"ho\":\"Pham Thi\",\"ten\":\"Dung\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN005\",\"ho\":\"Hoang Van\",\"ten\":\"Em\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN006\",\"ho\":\"Vu Thi\",\"ten\":\"Phuong\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN007\",\"ho\":\"Dang Van\",\"ten\":\"Giang\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN008\",\"ho\":\"Bui Thi\",\"ten\":\"Hoa\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN009\",\"ho\":\"Do Van\",\"ten\":\"Khanh\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN010\",\"ho\":\"Ngo Thi\",\"ten\":\"Lan\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN011\",\"ho\":\"Nguyen Van\",\"ten\":\"Minh\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN012\",\"ho\":\"Le Thi\",\"ten\":\"Ngoc\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN01-N\",\"masv\":\"N22DCCN013\",\"ho\":\"Tran Van\",\"ten\":\"Phat\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul

REM Students 14-25 -> D22CQCN02-N
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN014\",\"ho\":\"Pham Thi\",\"ten\":\"Quynh\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN015\",\"ho\":\"Hoang Van\",\"ten\":\"Son\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN016\",\"ho\":\"Vu Thi\",\"ten\":\"Tam\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN017\",\"ho\":\"Dang Van\",\"ten\":\"Thanh\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN018\",\"ho\":\"Bui Thi\",\"ten\":\"Uyen\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN019\",\"ho\":\"Do Van\",\"ten\":\"Vu\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN020\",\"ho\":\"Ngo Thi\",\"ten\":\"Xuan\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN021\",\"ho\":\"Nguyen Van\",\"ten\":\"Y\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN022\",\"ho\":\"Le Thi\",\"ten\":\"Zung\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN023\",\"ho\":\"Tran Van\",\"ten\":\"Anh\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN024\",\"ho\":\"Pham Thi\",\"ten\":\"Bich\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQCN02-N\",\"masv\":\"N22DCCN025\",\"ho\":\"Hoang Van\",\"ten\":\"Cuong\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul

REM Students 26-38 -> D22CQVT01-N
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN026\",\"ho\":\"Vu Thi\",\"ten\":\"Dao\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN027\",\"ho\":\"Dang Van\",\"ten\":\"Gia\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN028\",\"ho\":\"Bui Thi\",\"ten\":\"Hanh\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN029\",\"ho\":\"Do Van\",\"ten\":\"Khoa\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN030\",\"ho\":\"Ngo Thi\",\"ten\":\"Linh\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN031\",\"ho\":\"Nguyen Van\",\"ten\":\"Long\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN032\",\"ho\":\"Le Thi\",\"ten\":\"Mai\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN033\",\"ho\":\"Tran Van\",\"ten\":\"Nam\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN034\",\"ho\":\"Pham Thi\",\"ten\":\"Oanh\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN035\",\"ho\":\"Hoang Van\",\"ten\":\"Phuc\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN036\",\"ho\":\"Vu Thi\",\"ten\":\"Quyen\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN037\",\"ho\":\"Dang Van\",\"ten\":\"Sang\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQVT01-N\",\"masv\":\"N22DCCN038\",\"ho\":\"Bui Thi\",\"ten\":\"Trang\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul

REM Students 39-50 -> D22CQAT01-N
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN039\",\"ho\":\"Do Van\",\"ten\":\"Ung\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN040\",\"ho\":\"Ngo Thi\",\"ten\":\"Van\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN041\",\"ho\":\"Nguyen Van\",\"ten\":\"Xuan\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN042\",\"ho\":\"Le Thi\",\"ten\":\"Yen\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN043\",\"ho\":\"Tran Van\",\"ten\":\"Zung\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN044\",\"ho\":\"Pham Thi\",\"ten\":\"Anh\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN045\",\"ho\":\"Hoang Van\",\"ten\":\"Bao\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN046\",\"ho\":\"Vu Thi\",\"ten\":\"Cat\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN047\",\"ho\":\"Dang Van\",\"ten\":\"Duc\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN048\",\"ho\":\"Bui Thi\",\"ten\":\"E\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN049\",\"ho\":\"Do Van\",\"ten\":\"Giang\",\"phai\":\"Nam\",\"password\":\"123\"}" > nul
curl -s -X POST http://localhost:8080/api/students -H "Content-Type: application/json" -d "{\"malop\":\"D22CQAT01-N\",\"masv\":\"N22DCCN050\",\"ho\":\"Ngo Thi\",\"ten\":\"Hoa\",\"phai\":\"Nu\",\"password\":\"123\"}" > nul
echo   Done: 50 students

echo [5] Creating questions (20 per subject, 80 total)...
REM INT1339 - Lap trinh C++ (20 questions)
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Polymorphism la gi?\",\"a\":\"Da hinh\",\"b\":\"Dong goi\",\"c\":\"Ke thua\",\"d\":\"Truu tuong\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Tu khoa 'virtual' dung de?\",\"a\":\"Khai bao bien\",\"b\":\"Khai bao ham ao\",\"c\":\"Khai bao lop\",\"d\":\"Khai bao hang\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Constructor la gi?\",\"a\":\"Ham huy\",\"b\":\"Ham tao\",\"c\":\"Ham ban\",\"d\":\"Ham tinh\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Destructor lam gi?\",\"a\":\"Tao doi tuong\",\"b\":\"Huy doi tuong\",\"c\":\"Sao chep\",\"d\":\"Gan gia tri\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Encapsulation la gi?\",\"a\":\"Da hinh\",\"b\":\"Dong goi\",\"c\":\"Ke thua\",\"d\":\"Truu tuong\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Inheritance la gi?\",\"a\":\"Da hinh\",\"b\":\"Dong goi\",\"c\":\"Ke thua\",\"d\":\"Truu tuong\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Abstraction la gi?\",\"a\":\"Da hinh\",\"b\":\"Dong goi\",\"c\":\"Ke thua\",\"d\":\"Truu tuong\",\"dapan\":\"d\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Tu khoa 'public' la?\",\"a\":\"Rieng tu\",\"b\":\"Bao ve\",\"c\":\"Cong khai\",\"d\":\"Ban be\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Tu khoa 'private' la?\",\"a\":\"Rieng tu\",\"b\":\"Cong khai\",\"c\":\"Ban be\",\"d\":\"Bao ve\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Tu khoa 'protected' la?\",\"a\":\"Rieng tu\",\"b\":\"Cong khai\",\"c\":\"Bao ve\",\"d\":\"Ban be\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"'new' trong C++ dung de?\",\"a\":\"Xoa bo nho\",\"b\":\"Cap phat bo nho\",\"c\":\"Khai bao bien\",\"d\":\"Ket thuc chuong trinh\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"'delete' trong C++ dung de?\",\"a\":\"Cap phat bo nho\",\"b\":\"Xoa bo nho\",\"c\":\"Khai bao bien\",\"d\":\"Ket thuc chuong trinh\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Con tro (pointer) la gi?\",\"a\":\"Bien luu gia tri\",\"b\":\"Bien luu dia chi\",\"c\":\"Ham\",\"d\":\"Lop\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Tham chieu (reference) la?\",\"a\":\"Biet danh cua bien\",\"b\":\"Con tro\",\"c\":\"Hang so\",\"d\":\"Ham\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Template trong C++ la gi?\",\"a\":\"Kieu du lieu\",\"b\":\"Ham\",\"c\":\"Lop\",\"d\":\"Khuon hinh tong quat\",\"dapan\":\"d\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"STL la gi?\",\"a\":\"Standard Template Library\",\"b\":\"Standard Type List\",\"c\":\"Simple Template Library\",\"d\":\"System Type Library\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Vector trong C++ la?\",\"a\":\"Mang tinh\",\"b\":\"Mang dong\",\"c\":\"Danh sach lien ket\",\"d\":\"Cay\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Map trong C++ la?\",\"a\":\"Danh sach\",\"b\":\"Mang\",\"c\":\"Cau truc key-value\",\"d\":\"Hang doi\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"'const' trong C++ co nghia la?\",\"a\":\"Bien thay doi\",\"b\":\"Bien khong thay doi\",\"c\":\"Bien toan cuc\",\"d\":\"Bien cuc bo\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1339\",\"noidung\":\"Operator overloading la gi?\",\"a\":\"Nap chong toan tu\",\"b\":\"Nap chong ham\",\"c\":\"Nap chong lop\",\"d\":\"Nap chong bien\",\"dapan\":\"a\"}" > nul

REM INT13145 - CTDLGT (20 questions)
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Big-O notation mo ta gi?\",\"a\":\"Thoi gian chay\",\"b\":\"Do phuc tap thoi gian\",\"c\":\"Bo nho\",\"d\":\"Kich thuoc\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Danh sach lien ket la gi?\",\"a\":\"Mang tinh\",\"b\":\"Cau truc du lieu tuyen tinh\",\"c\":\"Cay\",\"d\":\"Do thi\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Stack hoat dong theo nguyen tac?\",\"a\":\"FIFO\",\"b\":\"LIFO\",\"c\":\"Random\",\"d\":\"Priority\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Queue hoat dong theo nguyen tac?\",\"a\":\"FIFO\",\"b\":\"LIFO\",\"c\":\"Random\",\"d\":\"Priority\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Cay nhi phan tim kiem co thuoc tinh?\",\"a\":\"Con trai lon hon cha\",\"b\":\"Con trai nho hon cha, con phai lon hon cha\",\"c\":\"Con phai nho hon cha\",\"d\":\"Khong co thu tu\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Quick Sort co do phuc tap trung binh?\",\"a\":\"O(n)\",\"b\":\"O(n log n)\",\"c\":\"O(n^2)\",\"d\":\"O(log n)\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Merge Sort co do phuc tap?\",\"a\":\"O(n)\",\"b\":\"O(n log n)\",\"c\":\"O(n^2)\",\"d\":\"O(1)\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Bubble Sort co do phuc tap xau nhat?\",\"a\":\"O(n)\",\"b\":\"O(n log n)\",\"c\":\"O(n^2)\",\"d\":\"O(1)\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Hash Table tra cuu trung binh?\",\"a\":\"O(n)\",\"b\":\"O(log n)\",\"c\":\"O(1)\",\"d\":\"O(n^2)\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Cay AVL la cay?\",\"a\":\"Nhi phan can bang\",\"b\":\"Nhi phan thuong\",\"c\":\"B-tree\",\"d\":\"Red-Black\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Heap la cau truc?\",\"a\":\"FIFO\",\"b\":\"LIFO\",\"c\":\"Cay nhi phan dac biet\",\"d\":\"Danh sach\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Duyet cay theo thu tu giua (Inorder) co thu tu?\",\"a\":\"Goc-Trai-Phai\",\"b\":\"Trai-Goc-Phai\",\"c\":\"Trai-Phai-Goc\",\"d\":\"Phai-Goc-Trai\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Duyet cay theo thu tu truoc (Preorder) co thu tu?\",\"a\":\"Goc-Trai-Phai\",\"b\":\"Trai-Goc-Phai\",\"c\":\"Trai-Phai-Goc\",\"d\":\"Phai-Trai-Goc\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Duyet cay theo thu tu sau (Postorder) co thu tu?\",\"a\":\"Goc-Trai-Phai\",\"b\":\"Trai-Goc-Phai\",\"c\":\"Trai-Phai-Goc\",\"d\":\"Goc-Phai-Trai\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Do thi (Graph) gom?\",\"a\":\"Dinh va canh\",\"b\":\"Dinh va la\",\"c\":\"Canh va la\",\"d\":\"Node va link\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Dijkstra tim?\",\"a\":\"Duong di ngan nhat\",\"b\":\"Cay khung nho nhat\",\"c\":\"Chu trinh Euler\",\"d\":\"BFS\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"BFS su dung?\",\"a\":\"Stack\",\"b\":\"Queue\",\"c\":\"Heap\",\"d\":\"Tree\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"DFS su dung?\",\"a\":\"Queue\",\"b\":\"Stack\",\"c\":\"Heap\",\"d\":\"Array\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Mang (Array) truy cap phan tu?\",\"a\":\"O(n)\",\"b\":\"O(log n)\",\"c\":\"O(1)\",\"d\":\"O(n^2)\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT13145\",\"noidung\":\"Danh sach lien ket chen phan tu?\",\"a\":\"O(1)\",\"b\":\"O(n)\",\"c\":\"O(log n)\",\"d\":\"O(n^2)\",\"dapan\":\"a\"}" > nul

REM INT1332 - Co so du lieu (20 questions)
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"SQL la gi?\",\"a\":\"Structured Query Language\",\"b\":\"Simple Query Language\",\"c\":\"Standard Query Language\",\"d\":\"System Query Language\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"Khoa chinh (Primary Key) la?\",\"a\":\"Khoa duy nhat\",\"b\":\"Khoa ngoai\",\"c\":\"Khoa phu\",\"d\":\"Khong co y nghia\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"Khoa ngoai (Foreign Key) la?\",\"a\":\"Khoa chinh\",\"b\":\"Khoa tham chieu bang khac\",\"c\":\"Khoa duy nhat\",\"d\":\"Khoa phu\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"NORMALIZATION la gi?\",\"a\":\"Chuan hoa du lieu\",\"b\":\"Sao chep du lieu\",\"c\":\"Xoa du lieu\",\"d\":\"Them du lieu\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"1NF yeu cau?\",\"a\":\"Khong trung lap nhom\",\"b\":\"Khoa chinh\",\"c\":\"Khoa ngoai\",\"d\":\"Tat ca deu dung\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"2NF yeu cau?\",\"a\":\"1NF va khong phu thuoc ham\",\"b\":\"1NF va phu thuoc ham\",\"c\":\"Khong can 1NF\",\"d\":\"3NF\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"3NF yeu cau?\",\"a\":\"2NF va khong bac chuyen\",\"b\":\"2NF va co bac chuyen\",\"c\":\"1NF\",\"d\":\"BCNF\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"JOIN trong SQL la?\",\"a\":\"Ket hop bang\",\"b\":\"Xoa bang\",\"c\":\"Tao bang\",\"d\":\"Sua bang\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"INNER JOIN tra ve?\",\"a\":\"Tat ca dong\",\"b\":\"Dong khop\",\"c\":\"Dong khong khop\",\"d\":\"Bang rong\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"LEFT JOIN tra ve?\",\"a\":\"Tat ca dong bang trai\",\"b\":\"Tat ca dong bang phai\",\"c\":\"Dong khop\",\"d\":\"Bang rong\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"GROUP BY dung de?\",\"a\":\"Sap xep\",\"b\":\"Nhom du lieu\",\"c\":\"Loc du lieu\",\"d\":\"Ket hop\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"ORDER BY dung de?\",\"a\":\"Nhom\",\"b\":\"Sap xep\",\"c\":\"Loc\",\"d\":\"Ket hop\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"WHERE dung de?\",\"a\":\"Sap xep\",\"b\":\"Nhom\",\"c\":\"Loc dieu kien\",\"d\":\"Ket hop\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"INDEX trong DB la?\",\"a\":\"Cau truc tang toc truy van\",\"b\":\"Khoa chinh\",\"c\":\"Khoa ngoai\",\"d\":\"Bang\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"Transaction co tinh chat ACID, chu cai A la?\",\"a\":\"Atomicity\",\"b\":\"Consistency\",\"c\":\"Isolation\",\"d\":\"Durability\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"ACID, chu cai C la?\",\"a\":\"Atomicity\",\"b\":\"Consistency\",\"c\":\"Isolation\",\"d\":\"Durability\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"ACID, chu cai I la?\",\"a\":\"Atomicity\",\"b\":\"Consistency\",\"c\":\"Isolation\",\"d\":\"Durability\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"ACID, chu cai D la?\",\"a\":\"Atomicity\",\"b\":\"Consistency\",\"c\":\"Isolation\",\"d\":\"Durability\",\"dapan\":\"d\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"SELECT * FROM lay gi?\",\"a\":\"1 dong\",\"b\":\"Tat ca dong va cot\",\"c\":\"1 cot\",\"d\":\"Khong gi\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1332\",\"noidung\":\"DROP TABLE lam gi?\",\"a\":\"Xoa bang\",\"b\":\"Tao bang\",\"c\":\"Sua bang\",\"d\":\"Xem bang\",\"dapan\":\"a\"}" > nul

REM INT1415 - Mang may tinh (20 questions)
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"OSI model co bao nhieu tang?\",\"a\":\"5\",\"b\":\"7\",\"c\":\"4\",\"d\":\"3\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"TCP/IP model co bao nhieu tang?\",\"a\":\"7\",\"b\":\"5\",\"c\":\"4\",\"d\":\"3\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"HTTP la giao thuc tang?\",\"a\":\"Transport\",\"b\":\"Application\",\"c\":\"Network\",\"d\":\"Data Link\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"TCP la giao thuc?\",\"a\":\"Khong tin cay\",\"b\":\"Tin cay\",\"c\":\"Khong ket noi\",\"d\":\"Broadcast\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"UDP la giao thuc?\",\"a\":\"Tin cay\",\"b\":\"Khong tin cay\",\"c\":\"Ket noi\",\"d\":\"Multicast\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"IP la giao thuc tang?\",\"a\":\"Application\",\"b\":\"Transport\",\"c\":\"Network\",\"d\":\"Physical\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"DNS phuc vu gi?\",\"a\":\"Gui email\",\"b\":\"Phan giai ten mien\",\"c\":\"Chuyen file\",\"d\":\"Dang nhap\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"DHCP phuc vu gi?\",\"a\":\"Cap IP tu dong\",\"b\":\"Phan giai ten\",\"c\":\"Gui mail\",\"d\":\"Web\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"MAC address co do dai?\",\"a\":\"32 bit\",\"b\":\"48 bit\",\"c\":\"64 bit\",\"d\":\"128 bit\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"IPv4 co do dai?\",\"a\":\"32 bit\",\"b\":\"48 bit\",\"c\":\"64 bit\",\"d\":\"128 bit\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"IPv6 co do dai?\",\"a\":\"32 bit\",\"b\":\"64 bit\",\"c\":\"128 bit\",\"d\":\"256 bit\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"Port HTTP mac dinh?\",\"a\":\"21\",\"b\":\"22\",\"c\":\"80\",\"d\":\"443\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"Port HTTPS mac dinh?\",\"a\":\"80\",\"b\":\"443\",\"c\":\"22\",\"d\":\"21\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"Port SSH mac dinh?\",\"a\":\"21\",\"b\":\"22\",\"c\":\"80\",\"d\":\"443\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"Subnet mask /24 co bao nhieu host?\",\"a\":\"128\",\"b\":\"254\",\"c\":\"256\",\"d\":\"512\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"Switch hoat dong o tang?\",\"a\":\"Physical\",\"b\":\"Data Link\",\"c\":\"Network\",\"d\":\"Transport\",\"dapan\":\"b\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"Router hoat dong o tang?\",\"a\":\"Physical\",\"b\":\"Data Link\",\"c\":\"Network\",\"d\":\"Application\",\"dapan\":\"c\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"Hub hoat dong o tang?\",\"a\":\"Physical\",\"b\":\"Data Link\",\"c\":\"Network\",\"d\":\"Transport\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"ARP la giao thuc?\",\"a\":\"Phan giai dia chi\",\"b\":\"Gui mail\",\"c\":\"Web\",\"d\":\"File\",\"dapan\":\"a\"}" > nul
curl -s -X POST http://localhost:8080/api/questions -H "Content-Type: application/json" -d "{\"mamh\":\"INT1415\",\"noidung\":\"NAT la gi?\",\"a\":\"Chuyen doi dia chi mang\",\"b\":\"Phan giai ten\",\"c\":\"Cap IP\",\"d\":\"Web\",\"dapan\":\"a\"}" > nul
echo   Done: 80 questions

echo [6] Saving to disk...
curl -s -X POST http://localhost:8080/api/admin/save
echo.

echo [7] Stopping server...
taskkill /f /im server.exe > nul 2>&1

echo.
echo === Seed Data Complete ===
echo   - 4 classes (D22CQCN01-N, D22CQCN02-N, D22CQVT01-N, D22CQAT01-N)
echo   - 4 subjects (INT1339, INT13145, INT1332, INT1415)
echo   - 50 students (N22DCCN001 - N22DCCN050)
echo   - 80 questions (20 per subject)
echo   - Teacher account: GV / GV
echo   - Student password: 123
echo.
pause
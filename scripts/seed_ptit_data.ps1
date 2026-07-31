$baseUrl = "http://localhost:8080/api"

Write-Host "Creating 20 classes via HTTP API..."
$majors = @("CNTT", "Vien thong", "An toan thong tin", "Mang may tinh")
$codes  = @("CN", "VT", "AT", "MR")
$classList = @()

for ($m = 0; $m -lt 4; $m++) {
    for ($c = 1; $c -le 5; $c++) {
        $malop = "D22CQ$($codes[$m])0$c-N"
        $tenlop = "Lop $($majors[$m]) $c Khoa 2022"
        $classList += $malop
        $body = @{ malop = $malop; tenlop = $tenlop } | ConvertTo-Json
        Invoke-RestMethod -Uri "$baseUrl/classes" -Method POST -ContentType "application/json" -Body $body | Out-Null
    }
}
Write-Host "  Done: 20 classes."

Write-Host "Creating 20 subjects via HTTP API..."
$subjects = @(
    @{ mamh="INT1339"; tenmh="Lap trinh C++" },
    @{ mamh="INT13145"; tenmh="Cau truc du lieu va giai thuat" },
    @{ mamh="INT1332"; tenmh="Co so du lieu" },
    @{ mamh="INT1415"; tenmh="Mang may tinh" },
    @{ mamh="INT1301"; tenmh="He dieu hanh" },
    @{ mamh="INT1302"; tenmh="Kien truc may tinh" },
    @{ mamh="INT1303"; tenmh="Tri tue nhan tao" },
    @{ mamh="INT1304"; tenmh="Hoc may" },
    @{ mamh="INT1305"; tenmh="Phan tich thiet ke he thong" },
    @{ mamh="INT1306"; tenmh="Cong nghe phan mem" },
    @{ mamh="INT1307"; tenmh="Lap trinh Java" },
    @{ mamh="INT1308"; tenmh="Lap trinh Web" },
    @{ mamh="INT1309"; tenmh="Lap trinh Huong doi tuong" },
    @{ mamh="INT1310"; tenmh="Do hoa may tinh" },
    @{ mamh="INT1311"; tenmh="Xu ly anh" },
    @{ mamh="INT1312"; tenmh="An toan thong tin" },
    @{ mamh="INT1313"; tenmh="Danh gia hieu nang he thong" },
    @{ mamh="INT1314"; tenmh="Dien toan dam may" },
    @{ mamh="INT1315"; tenmh="Phat trien ung dung di dong" },
    @{ mamh="INT1316"; tenmh="Quan tri du an CNTT" }
)

foreach ($s in $subjects) {
    $body = @{ mamh = $s.mamh; tenmh = $s.tenmh } | ConvertTo-Json
    Invoke-RestMethod -Uri "$baseUrl/subjects" -Method POST -ContentType "application/json" -Body $body | Out-Null
}
Write-Host "  Done: 20 subjects."

Write-Host "Creating 200 students (10 students per class across 20 classes)..."
$lastNames = @("Nguyen Van", "Le Thi", "Tran Van", "Pham Thi", "Hoang Van", "Vu Thi", "Dang Van", "Bui Thi", "Do Van", "Ngo Thi")
$firstNames = @("An", "Binh", "Cuong", "Dung", "Em", "Phuong", "Giang", "Hoa", "Khanh", "Lan")

$stCount = 1
foreach ($malop in $classList) {
    for ($i = 0; $i -lt 10; $i++) {
        $idStr = "{0:D3}" -f $stCount
        $masv = "N22DCCN$idStr"
        $phai = if ($i % 2 -eq 0) { "Nam" } else { "Nu" }
        $body = @{ malop = $malop; masv = $masv; ho = $lastNames[$i]; ten = $firstNames[$i]; phai = $phai; password = "123" } | ConvertTo-Json
        Invoke-RestMethod -Uri "$baseUrl/students" -Method POST -ContentType "application/json" -Body $body | Out-Null
        $stCount++
    }
}
Write-Host "  Done: 200 students."

Write-Host "Creating 200 questions (10 questions per subject across 20 subjects)..."
$options = @("a", "b", "c", "d")

foreach ($s in $subjects) {
    $mamh = $s.mamh
    for ($q = 1; $q -le 10; $q++) {
        $ans = $options[($q - 1) % 4]
        $body = @{
            mamh = $mamh
            noidung = "Cau hoi $q cua mon $($s.tenmh) ($mamh)"
            a = "Phuong an A cau $q"
            b = "Phuong an B cau $q"
            c = "Phuong an C cau $q"
            d = "Phuong an D cau $q"
            dapan = $ans
        } | ConvertTo-Json -Depth 5
        Invoke-RestMethod -Uri "$baseUrl/questions" -Method POST -ContentType "application/json" -Body $body | Out-Null
    }
}
Write-Host "  Done: 200 questions."

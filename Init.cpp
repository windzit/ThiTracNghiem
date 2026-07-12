#include <iostream>
using namespace std;
// dsCHT : danh sach cau hoi thi
struct MonHoc{
    string MAMH;
    string TENMH;
    dsCHT* dsCauHoi = NULL;
};

// BST cho mon hoc ( kem cac ham phu tro insert , search , delete )

struct NodeMH{
    MonHoc monhoc;
    NodeMH* left;
    NodeMH* right;
    NodeMH(MonHoc monhoc){
        left = nullptr;
        right = nullptr;
        monhoc = monhoc;
    }
};

struct CauHoi{
    int ID; // ID tu dong tang , khong trung lap
    string NOIDUNG;
    string DAPAN[4];
};

struct dsCHT{
    dsCHT*next;
    CauHoi cauhoi;
    dsCHT(CauHoi cauhoi){
        next = nullptr;
        cauhoi = cauhoi;
    }
    
};

struct DiemThi{
    string MAMH;
    float DIEM;
};

struct dsDiemThi{
    dsDiemThi* next;
    DiemThi diemthi;
    dsDiemThi(DiemThi diemthi){
        next = nullptr;
        diemthi = diemthi;
    }
};

struct SinhVien{
    string MASV;
    string HO;
    string TEN;
    string PHAI;
    string passsword;
    dsDiemThi* dsdiemthi = NULL;
};

struct dsSinhVien{
    dsSinhVien* next;
    SinhVien sinhvien;
    dsSinhVien(SinhVien sinhvien){
        next = nullptr;
        sinhvien = sinhvien;
    }
};


const int MAXLOP =10000;
struct Lop{
    string MALOP;
    string TENLOP;
    dsSinhVien* dssinhvien = NULL;
};

struct dsLop{
    int n;
    Lop* lop[MAXLOP];
};


int main(){
    dsLop* dslop = new dsLop();
    NodeMH* root = nullptr;
    
}
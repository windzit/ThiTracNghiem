#include <iostream>
using namespace std;
const int MAXLOP =10000;

struct dsCHT;

// dsCHT : danh sach cau hoi thi
struct MonHoc{
    char MAMH[15];
    string TENMH;
    dsCHT* dsCauHoi = NULL;
};



struct NodeMH{
    MonHoc monhoc;
    NodeMH* left;
    NodeMH* right;
    NodeMH(MonHoc monhoc){
        left = nullptr;
        right = nullptr;
        this->monhoc = monhoc;
    }
};

struct CauHoi{
    int ID; 
    string NOIDUNG;
    string A;
    string B;
    string C;
    string D;
    char DAPAN_DUNG; // 'A' | 'B' | 'C' | 'D'
};

struct dsCHT{
    dsCHT*next;
    CauHoi cauhoi;
    dsCHT(CauHoi cauhoi){
        next = nullptr;
        this->cauhoi = cauhoi;
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
        this->diemthi = diemthi;
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
        this->sinhvien = sinhvien;
    }
};



struct Lop{
    string MALOP;
    string TENLOP;
    dsSinhVien* dssinhvien = NULL;
};

struct dsLop{
    int n;
    dsLop* lop[MAXLOP];
    dsLop(){
        n = 0;
        for(int i = 0; i < MAXLOP; i++){
            lop[i] = nullptr;
        }
    }
};


int main(){
    dsLop* dslop = new dsLop();
    NodeMH* root = nullptr;
    
}
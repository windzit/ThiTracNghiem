#include <iostream>
using namespace std;
// dsCHT : danh sach cau hoi thi
struct MonHoc{
    string MAMH;
    string TENMH;
    dsCHT* dsCauHoi = NULL;
};

// BST cho mon hoc ( kem cac ham phu tro insert , search , delete )


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


#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "../include/Report.h"
#include "../include/Subject.h"

using namespace std;

void PrintClassListReport(dsLop* root) {
    if(root == nullptr || root->n == 0) {
        cout << "Khong co lop nao.\n";
        return;
    }
    cout << "\n=== DANH SACH LOP ===\n";
    cout << left << setw(15) << "MALOP"
         << setw(30) << "TENLOP"
         << setw(10) << "SI SO" << "\n";
    cout << string(55, '-') << "\n";
    for(int i = 0; i < root->n; i++){
        if(root->dslop[i] == nullptr) continue;
        cout << left << setw(15) << root->dslop[i]->MALOP
             << setw(30) << root->dslop[i]->TENLOP
             << setw(10) << root->dslop[i]->dssinhvien.size() << "\n";
    }
}

static void collectMAMHRecursive(NodeMH* node, vector<string>& result) {
    if (!node) return;
    collectMAMHRecursive(node->left, result);
    result.push_back(node->data.MAMH);
    collectMAMHRecursive(node->right, result);
}

static vector<string> getAllMAMHFromRAM(Subject& dsmh) {
    vector<string> result;
    collectMAMHRecursive(dsmh.getRoot(), result);
    return result;
}

void PrintExamDetailReport(Class& dsl, Subject& dsmh) {
    // 1. Input MALOP
    string malop;
    cout << "\n=== CHI TIET BAI THI ===\n";
    cout << "Nhap MALOP: "; cin >> malop;
    Lop* lop = dsl.find(malop);
    if (!lop) {
        cout << "Lop khong ton tai.\n";
        return;
    }

    // 2. Input MASV
    string masv;
    cout << "Nhap MASV: "; cin >> masv;
    SinhVien* sv = lop->dssinhvien.find(masv);
    if (!sv) {
        cout << "Sinh vien khong ton tai trong lop.\n";
        return;
    }

    // 3. Input MAMH
    char mamh[15];
    cout << "Nhap MAMH: "; cin >> mamh;
    NodeMH* monhoc = dsmh.find(mamh);
    if (!monhoc) {
        cout << "Mon hoc khong ton tai.\n";
        return;
    }

    // 4. Find score
    dsDiemThi* diem = sv->dsdiemthi.find(mamh);
    if (!diem) {
        cout << "Sinh vien chua thi mon: " << mamh << "\n";
        return;
    }

    // 5. Display info (READ ONLY)
    cout << "\n--- THONG TIN SINH VIEN ---\n";
    cout << "MASV:    " << sv->MASV << "\n";
    cout << "Ho ten:  " << sv->HO << " " << sv->TEN << "\n";
    cout << "Phai:    " << sv->PHAI << "\n";
    cout << "Lop:     " << lop->MALOP << " - " << lop->TENLOP << "\n";

    cout << "\n--- MON HOC ---\n";
    cout << "MAMH:    " << monhoc->data.MAMH << "\n";
    cout << "TENMH:   " << monhoc->data.TENMH << "\n";

    cout << "\n--- DIEM THI ---\n";
    cout << "Diem:    " << diem->diemthi.DIEM << "\n";

    cout << "\n--- DANH SACH CAU HOI (THAM KHAO) ---\n";
    int stt = 1;
    for (int id = 0; id < 10000; id++) {
        dsCHT* q = monhoc->data.dsCauHoi.find(id);
        if (q) {
            cout << stt++ << ". [ID=" << q->cauhoi.ID << "] "
                 << q->cauhoi.NOIDUNG << "\n";
            cout << "   A. " << q->cauhoi.A << "\n";
            cout << "   B. " << q->cauhoi.B << "\n";
            cout << "   C. " << q->cauhoi.C << "\n";
            cout << "   D. " << q->cauhoi.D << "\n";
            cout << "   Dap an dung: " << q->cauhoi.DAPAN_DUNG << "\n\n";
        }
    }
    cout << "--- HET ---\n";
}

void PrintScoreboardReport(Class& dsl, Subject& dsmh) {
    // 1. Input MALOP
    string malop;
    cout << "\n=== BANG DIEM LOP ===\n";
    cout << "Nhap MALOP: "; cin >> malop;
    Lop* lop = dsl.find(malop);
    if (!lop) {
        cout << "Lop khong ton tai.\n";
        return;
    }

    // 2. Get all subjects from RAM
    vector<string> allMAMH = getAllMAMHFromRAM(dsmh);
    if (allMAMH.empty()) {
        cout << "Khong co mon hoc nao.\n";
        return;
    }

    // 3. Print header
    cout << "\n" << left << setw(15) << "MASV"
         << setw(20) << "Ho ten";
    for (const string& mamh : allMAMH) {
        cout << setw(10) << mamh;
    }
    cout << "\n" << string(15 + 20 + (int)allMAMH.size() * 10, '-') << "\n";

    // 4. Iterate students from RAM
    dsSinhVien* curSV = lop->dssinhvien.getRoot();
    if (!curSV) {
        cout << "Khong co sinh vien trong lop.\n";
        return;
    }

    while (curSV) {
        const SinhVien& sv = curSV->sinhvien;
        cout << left << setw(15) << sv.MASV
             << setw(20) << (sv.HO + " " + sv.TEN);

        for (const string& mamh : allMAMH) {
            dsDiemThi* diem = const_cast<SinhVien&>(sv).dsdiemthi.find(mamh.c_str());
            if (diem) {
                cout << setw(10) << diem->diemthi.DIEM;
            } else {
                cout << setw(10) << "-";
            }
        }
        cout << "\n";
        curSV = curSV->next;
    }

    cout << "\n--- HET ---\n";
}
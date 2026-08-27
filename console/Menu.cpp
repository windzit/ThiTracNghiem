#include <iostream>
#include <limits>
#include "Menu.h"
#include "Report.h"
#include "ConsoleExam.h"
#include "../include/Auth.h"
#include "../include/Storage.h"
#include "../include/StringNormalizer.h"
#include "../include/Utils.h"
#include "../include/IndexManager.h"
#include "../include/StorageManager.h"

// -- helpers --
static bool login_student(dsLop &uis, const std::string &username, const std::string &password) {
    for (int i = 0; i < uis.n; i++) {
        if (uis.dslop[i] == nullptr) continue;
        SinhVien* sv = uis.dslop[i]->dssinhvien.find(username);
        if (sv) {
            return (sv->passsword == password);
        }
    }
    int64_t offset = -1;
    if (IndexManager::getInstance().getStudentOffset(username, offset)) {
        SinhVien sv;
        std::string malop;
        if (StorageManager::getInstance().readStudentAt(offset, sv, malop)) {
            return (sv.passsword == password);
        }
    }
    return false;
}

static void pressEnter() {
    std::cout << "\nNhan Enter de tiep tuc...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// -- GV sub-menus --

static void gvClassMenu(Class& dsl) {
    while (true) {
        std::cout << "\n=== QUAN LY LOP ===\n"
                  << "1. Them lop\n"
                  << "2. Xoa lop\n"
                  << "3. Danh sach lop\n"
                  << "0. Quay lai\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) return;

        if (choice == 1) {
            std::string malop, tenlop;
            std::cout << "Nhap MALOP: "; std::cin >> malop;
            std::cout << "Nhap TENLOP: "; std::cin.ignore(); std::getline(std::cin, tenlop);
            Lop* lop = new Lop();
            lop->MALOP = malop;
            lop->TENLOP = tenlop;
            StringNormalizer::normalizeClass(*lop);

            if (dsl.insert(lop))
                std::cout << "Them lop thanh cong.\n";
            else
                std::cout << "Loi: lop da ton tai hoac danh sach day.\n";
        }
        else if (choice == 2) {
            std::string malop;
            std::cout << "Nhap MALOP can xoa: "; std::cin >> malop;
            if (dsl.remove(malop))
                std::cout << "Xoa lop thanh cong.\n";
            else
                std::cout << "Loi: khong tim thay lop.\n";
        }
        else if (choice == 3) {
            PrintClassListReport(dsl.getRoot());
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

static void gvStudentMenu(Class& dsl) {
    while (true) {
        std::cout << "\n=== QUAN LY SINH VIEN ===\n"
                  << "1. Them sinh vien vao lop\n"
                  << "2. Xoa sinh vien\n"
                  << "0. Quay lai\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) return;

        if (choice == 1) {
            std::string malop;
            std::cout << "Nhap MALOP: "; std::cin >> malop;
            Lop* lop = dsl.find(malop);
            if (!lop) {
                std::cout << "Lop khong ton tai.\n";
                continue;
            }
            char again;
            do {
                SinhVien sv;
                std::cout << "  MASV: "; std::cin >> sv.MASV;
                std::cout << "  HO: "; std::cin >> sv.HO;
                std::cout << "  TEN: "; std::cin >> sv.TEN;
                std::cout << "  PHAI (Nam/Nu): "; std::cin >> sv.PHAI;
                std::cout << "  Password: "; std::cin >> sv.passsword;
                if (lop->dssinhvien.insert(sv))
                    std::cout << "  Them SV thanh cong.\n";
                else
                    std::cout << "  Loi: MASV da ton tai.\n";
                std::cout << "  Them nua? (Y/N): "; std::cin >> again;
            } while (again == 'Y' || again == 'y');
        }
        else if (choice == 2) {
            std::string malop, masv;
            std::cout << "Nhap MALOP: "; std::cin >> malop;
            Lop* lop = dsl.find(malop);
            if (!lop) {
                std::cout << "Lop khong ton tai.\n";
                continue;
            }
            std::cout << "Nhap MASV can xoa: "; std::cin >> masv;
            if (lop->dssinhvien.remove(masv))
                std::cout << "Xoa SV thanh cong.\n";
            else
                std::cout << "Loi: khong tim thay SV.\n";
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

static void gvSubjectMenu(Subject& dsmh) {
    while (true) {
        std::cout << "\n=== QUAN LY MON HOC ===\n"
                  << "1. Them mon hoc\n"
                  << "2. Xoa mon hoc\n"
                  << "3. Sua ten mon hoc\n"
                  << "0. Quay lai\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) return;

        if (choice == 1) {
            MonHoc mh;
            std::cout << "Nhap MAMH: "; std::cin >> mh.MAMH;
            std::cin.ignore();
            std::cout << "Nhap TENMH: "; std::getline(std::cin, mh.TENMH);
            if (dsmh.insert(mh))
                std::cout << "Them mon hoc thanh cong.\n";
            else
                std::cout << "Loi: mon hoc da ton tai.\n";
        }
        else if (choice == 2) {
            char mamh[15];
            std::cout << "Nhap MAMH can xoa: "; std::cin >> mamh;
            if (dsmh.remove(mamh))
                std::cout << "Xoa mon hoc thanh cong.\n";
            else
                std::cout << "Loi: khong tim thay mon hoc hoac co cau hoi da dung.\n";
        }
        else if (choice == 3) {
            char mamh[15];
            std::cout << "Nhap MAMH can sua: "; std::cin >> mamh;
            NodeMH* node = dsmh.find(mamh);
            if (!node) {
                std::cout << "Khong tim thay mon hoc.\n";
                continue;
            }
            std::cin.ignore();
            std::cout << "TENMH cu: " << node->data.TENMH << "\n";
            std::cout << "TENMH moi: "; std::getline(std::cin, node->data.TENMH);
            std::cout << "Da cap nhat.\n";
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

static void gvQuestionMenu(Subject& dsmh) {
    while (true) {
        std::cout << "\n=== QUAN LY CAU HOI ===\n"
                  << "1. Them cau hoi\n"
                  << "2. Xoa cau hoi\n"
                  << "0. Quay lai\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) return;

        if (choice == 1) {
            char mamh[15];
            std::cout << "Nhap MAMH: "; std::cin >> mamh;
            NodeMH* node = dsmh.find(mamh);
            if (!node) {
                std::cout << "Khong tim thay mon hoc.\n";
                continue;
            }
            CauHoi ch;
            ch.ID = StorageManager::getInstance().getNextQuestionID();
            std::cin.ignore();
            std::cout << "  NOIDUNG: "; std::getline(std::cin, ch.NOIDUNG);
            std::cout << "  A: "; std::getline(std::cin, ch.A);
            std::cout << "  B: "; std::getline(std::cin, ch.B);
            std::cout << "  C: "; std::getline(std::cin, ch.C);
            std::cout << "  D: "; std::getline(std::cin, ch.D);
            std::cout << "  DAPAN_DUNG (A/B/C/D): "; std::cin >> ch.DAPAN_DUNG;
            if (node->data.dsCauHoi.insert(ch))
                std::cout << "Them cau hoi thanh cong (ID=" << ch.ID << ").\n";
            else
                std::cout << "Loi: them cau hoi that bai.\n";
        }
        else if (choice == 2) {
            char mamh[15];
            std::cout << "Nhap MAMH: "; std::cin >> mamh;
            NodeMH* node = dsmh.find(mamh);
            if (!node) {
                std::cout << "Khong tim thay mon hoc.\n";
                continue;
            }
            int id;
            std::cout << "  Nhap ID cau hoi can xoa: "; std::cin >> id;
            if (node->data.dsCauHoi.removeNode(id))
                std::cout << "Xoa cau hoi thanh cong.\n";
            else
                std::cout << "Loi: khong tim thay cau hoi hoac cau hoi da duoc dung.\n";
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

// -- GV: Xoa diem sinh vien --
static void gvDeleteScoreMenu(Class& dsl) {
    while (true) {
        std::cout << "\n=== XOA DIEM SINH VIEN ===\n"
                  << "1. Xoa diem theo MASV + MAMH\n"
                  << "0. Quay lai\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) return;

        if (choice == 1) {
            std::string masv;
            char mamh[15];
            std::cout << "Nhap MASV: "; std::cin >> masv;
            std::cout << "Nhap MAMH: "; std::cin >> mamh;

            // Find student
            SinhVien* sv = nullptr;
            dsLop* root = dsl.getRoot();
            for (int i = 0; i < root->n && !sv; i++) {
                if (root->dslop[i])
                    sv = root->dslop[i]->dssinhvien.find(masv);
            }

            if (!sv) {
                std::cout << "Khong tim thay SV voi MASV: " << masv << "\n";
                continue;
            }

            // Check if score exists
            dsDiemThi* diem = sv->dsdiemthi.find(mamh);
            if (!diem) {
                std::cout << "SV khong co diem mon: " << mamh << "\n";
                continue;
            }

            // Delete score
            if (sv->dsdiemthi.remove(mamh)) {
                std::cout << "Xoa diem thanh cong.\n";
                std::cout << "SV co the thi lai mon: " << mamh << "\n";
            } else {
                std::cout << "Loi: Khong the xoa diem.\n";
            }
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

static void gvReportMenu(Class& dsl, Subject& dsmh) {
    while (true) {
        std::cout << "\n=== BAO CAO ===\n"
                  << "1. Danh sach lop (si so)\n"
                  << "2. Chi tiet bai thi (Exam Detail)\n"
                  << "3. Bang diem lop (Scoreboard)\n"
                  << "0. Quay lai\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) return;

        if (choice == 1) {
            PrintClassListReport(dsl.getRoot());
        }
        else if (choice == 2) {
            PrintExamDetailReport(dsl, dsmh);
        }
        else if (choice == 3) {
            PrintScoreboardReport(dsl, dsmh);
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

// -- GV main menu --

static void gvMenu(Class& dsl, Subject& dsmh) {
    while (true) {
        std::cout << "\n=============================\n"
                  << "  GIAO VIEN - MENU CHINH\n"
                  << "=============================\n"
                  << "1. Quan ly Lop\n"
                  << "2. Quan ly Sinh vien\n"
                  << "3. Quan ly Mon hoc\n"
                  << "4. Quan ly Cau hoi\n"
                  << "5. Xem bao cao\n"
                  << "6. Xoa diem sinh vien\n"
                  << "0. Thoat (luu va thoat)\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        switch (choice) {
            case 1: gvClassMenu(dsl); break;
            case 2: gvStudentMenu(dsl); break;
            case 3: gvSubjectMenu(dsmh); break;
            case 4: gvQuestionMenu(dsmh); break;
            case 5: gvReportMenu(dsl, dsmh); break;
            case 6: gvDeleteScoreMenu(dsl); break;
            case 0: return;
            default: std::cout << "Lua chon khong hop le.\n"; break;
        }
    }
}

// -- SV main menu --

static void svMenu(Class& dsl, Subject& dsmh, SinhVien* sv) {
    while (true) {
        std::cout << "\n=============================\n"
                  << "  SINH VIEN - MENU CHINH\n"
                  << "  Xin chao: " << sv->HO << " " << sv->TEN
                  << " (" << sv->MASV << ")\n"
                  << "=============================\n"
                  << "1. Xem thong tin ca nhan\n"
                  << "2. Thi trac nghiem\n"
                  << "3. Xem diem\n"
                  << "0. Thoat (luu va thoat)\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) return;

        if (choice == 1) {
            std::cout << "\n--- THONG TIN CA NHAN ---\n"
                      << "MASV: " << sv->MASV << "\n"
                      << "Ho ten: " << sv->HO << " " << sv->TEN << "\n"
                      << "Phai: " << sv->PHAI << "\n";
        }
        else if (choice == 2) {
            ThiTracNghiem(dsmh, sv);
        }
        else if (choice == 3) {
            std::cout << "[Chuc nang dang phat trien] Xem diem chi tiet\n";
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

// -- Main menu entry point --

void ShowMainMenu(Class& dsl, Subject& dsmh) {
    while (true) {
        std::cout << "\n=============================\n"
                  << "  HE THONG THI TRAC NGHIEM\n"
                  << "=============================\n"
                  << "1. Dang nhap Giao vien (GV)\n"
                  << "2. Dang nhap Sinh vien (SV)\n"
                  << "0. Thoat chuong trinh\n"
                  << "Chon: ";
        int choice;
        if (!(std::cin >> choice)) { clearInput(); continue; }

        if (choice == 0) {
            std::cout << "Tam biet!\n";
            return;
        }

        if (choice == 1) {
            std::string user, pass;
            std::cout << "Username: "; std::cin >> user;
            std::cout << "Password: "; std::cin >> pass;
            if (login_teacher(user, pass)) {
                std::cout << "Dang nhap GV thanh cong.\n";
                gvMenu(dsl, dsmh);
                return;
            } else {
                std::cout << "Sai ten dang nhap hoac mat khau.\n";
            }
        }
        else if (choice == 2) {
            std::string user, pass;
            std::cout << "MASV: "; std::cin >> user;
            std::cout << "Password: "; std::cin >> pass;
            if (login_student(*dsl.getRoot(), user, pass)) {
                SinhVien* sv = nullptr;
                dsLop* root = dsl.getRoot();
                for (int i = 0; i < root->n && !sv; i++) {
                    if (root->dslop[i])
                        sv = root->dslop[i]->dssinhvien.find(user);
                }
                if (sv) {
                    std::cout << "Dang nhap SV thanh cong.\n";
                    svMenu(dsl, dsmh, sv);
                    return;
                }
            } else {
                std::cout << "Sai ma dang nhap hoac mat khau.\n";
            }
        }
        else {
            std::cout << "Lua chon khong hop le.\n";
        }
    }
}

#pragma once

#include "CommonTypes.h"

struct DiemThi {
	std::string MAMH;
	float DIEM;
};

struct dsDiemThi {
	dsDiemThi* next;
	DiemThi diemthi;

	dsDiemThi(DiemThi diemthi) {
		next = nullptr;
		this->diemthi = diemthi;
	}
};

struct SinhVien {
	std::string MASV;
	std::string HO;
	std::string TEN;
	std::string PHAI;
	std::string passsword;
	dsDiemThi* dsdiemthi = nullptr;
};

struct dsSinhVien {
	dsSinhVien* next;
	SinhVien sinhvien;

	dsSinhVien(SinhVien sinhvien) {
		next = nullptr;
		this->sinhvien = sinhvien;
	}
};

void InitStudentModule(dsSinhVien* &root) {
    root = nullptr;
};

SinhVien* FindStudent_with_MASV(dsSinhVien* root,const string &MASV);
bool InsertStudent(dsSinhVien* &root, const SinhVien sinhvien);
bool DeleteStudent(dsSinhVien* &root, const std::string& MASV);
bool UpdateStudent(dsSinhVien* root, const std::string& MASV, const SinhVien& newSinhVien);
bool FindStudent(dsSinhVien* root, const std::string& MASV);
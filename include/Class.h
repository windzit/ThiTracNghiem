#pragma once

#include "CommonTypes.h"

struct Lop {
	std::string MALOP;
	std::string TENLOP;
	dsSinhVien* dssinhvien = nullptr;
};

struct dsLop {
	int n;
	Lop* dslop[MAXLOP];

	dsLop() {
		n = 0;
		for (int i = 0; i < MAXLOP; i++) {
			dslop[i] = nullptr;
		}
	}
};

Lop *find_class_with_malop(dsLop *root,const std::string & MALOP);
bool InsertClass(dsLop* &root, Lop* lop);
bool DeleteClass(dsLop* &root, const std::string& MALOP);
bool UpdateClass(dsLop* &root, const std::string& MALOP, const std::string& newTENLOP);
bool FindClass(dsLop* root, const std::string& MALOP, Lop* &lop);
int demSiSo(dsLop* root, const string &MALOP);
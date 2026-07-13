#pragma once

#include "CommonTypes.h"

struct MonHoc {
	char MAMH[15];
	std::string TENMH;
	dsCHT* dsCauHoi = nullptr;
	bool used = false;
};

struct NodeMH {
	MonHoc monhoc;
	NodeMH* left;
	NodeMH* right;

	NodeMH(MonHoc monhoc) {
		left = nullptr;
		right = nullptr;
		this->monhoc = monhoc;
	}
};

void InitSubjectModule(NodeMH* &root){
    root = nullptr;
};
bool InsertSubject(NodeMH* &root, const MonHoc& monhoc);
bool DeleteSubject(NodeMH* &root, const char MAMH[15]);
bool UpdateSubject(NodeMH* &root, const char MAMH[15], const MonHoc& newMonHoc);
bool FindSubject(NodeMH* root, const char MAMH[15], MonHoc& monhoc);


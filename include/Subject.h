#pragma once

#include "CommonTypes.h"

struct MonHoc {
	char MAMH[15];
	std::string TENMH;
	dsCHT* dsCauHoi = nullptr;
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
bool InsertSubject(NodeMH* &root, MonHoc monhoc);
bool DeleteSubject(NodeMH* &root, const std::string& MAMH);
bool UpdateSubject(NodeMH* &root, const std::string& MAMH, const MonHoc& newMonHoc);
bool FindSubject(NodeMH* root, const std::string& MAMH, MonHoc& monhoc);


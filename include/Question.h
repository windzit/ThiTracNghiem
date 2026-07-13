#pragma once

#include "CommonTypes.h"

struct CauHoi {
	int ID;
	std::string NOIDUNG;
	std::string A;
	std::string B;
	std::string C;
	std::string D;
	char DAPAN_DUNG;
	bool used = false; 
};

struct dsCHT {
	dsCHT* next;
	CauHoi cauhoi;

	dsCHT(CauHoi cauhoi) {
		next = nullptr;
		this->cauhoi = cauhoi;
	}
};

void InitQuestionModule(dsCHT* &root) {
    root = nullptr;
};
bool InsertQuestion(dsCHT* &root, CauHoi cauhoi);
bool DeleteQuestion(dsCHT* &root, int ID);
bool UpdateQuestion(dsCHT* &root, int ID, const CauHoi& newCauHoi);
bool FindQuestion(dsCHT* root, int ID, CauHoi& cauhoi);

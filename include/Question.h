#pragma once

#include "CommonTypes.h"

struct CauHoi
{
    int ID = - 1;
    char NOIDUNG[100];
    char A[100];
    char B[100];
    char C[100];
    char D[100];
    char DAPAN_DUNG;   // Chỉ 1 ký tự
    bool used;
};

struct dsCHT {
	dsCHT* next;
	CauHoi cauhoi;

	dsCHT(CauHoi cauhoi) {
		next = nullptr;
		this->cauhoi = cauhoi;
	}
};

inline void InitQuestionModule(dsCHT* &root) {
    root = nullptr;
};
bool InsertQuestion(dsCHT* &root, CauHoi cauhoi);
bool DeleteQuestion(dsCHT* &root, int ID);
bool UpdateQuestion(dsCHT* &root, int ID);
bool FindQuestionByID(dsCHT* root, int ID, CauHoi& cauhoi);

bool FindQuestionByContent(dsCHT* root, const char* noidung, CauHoi& cauhoi);


bool LoadQuestionID();
bool SaveQuestionID();
void InitQuestionID();
int GetNextQuestionID();


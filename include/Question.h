#pragma once
#include "CommonTypes.h"
extern std::string PATH_FOLDER_QUESTIONS;

struct CauHoi {
    int ID;
    std::string NOIDUNG;
    std::string A;
    std::string B;
    std::string C;
    std::string D;
    char DAPAN_DUNG; // 'A' | 'B' | 'C' | 'D'
    bool used = false;
    bool deleted = false;
};

struct dsCHT {
    dsCHT* next;
    CauHoi cauhoi;

    dsCHT(CauHoi cauhoi) {
        next = nullptr;
        this->cauhoi = cauhoi;
    }

};

class Question {
public:
    Question() : root(nullptr) {};

    Question(const Question& other);

    Question& operator=(const Question& other);

    ~Question() {
        clear();
    };

    bool insert(CauHoi& cauhoi, bool autoId = false);
    bool removeNode(int ID);   // Physical removal (hard delete)
    bool setDeleted(int ID);   // Soft delete: set deleted=true
    bool restoreDeleted(int ID); // Restore soft deleted: set deleted=false
    bool update(int ID, const CauHoi& newData);
    dsCHT* find(int ID);
    bool hasUsedQuestions() const;
    int size() const;

    bool save(const char* MAMH);
    bool load(const char* MAMH);
    dsCHT* getRoot() const { return root; }
private:
    dsCHT* root;

    void clear();
};
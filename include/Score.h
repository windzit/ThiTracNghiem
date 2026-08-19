#pragma once
#include "CommonTypes.h"
extern std::string PATH_FOLDER_SCORELIST;

struct DiemThi {
    char MAMH[16] = {0};
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

class Score {
public:
    Score() : root(nullptr) {}
    Score(const Score& other);
    Score& operator=(const Score& other);

    ~Score() {
        clear();
    }

    bool insert(const DiemThi& DIEMTHI);
    bool remove(const char* MAMH);
    dsDiemThi* find(const char* MAMH);
    const dsDiemThi* find(const char* MAMH) const;
    
    bool save(const std::string& MASV);
    bool load(const std::string& MASV);
    int count() const;
    bool empty() const;
    dsDiemThi* getRoot() const { return root; }
private:
    dsDiemThi* root;

    void clear();
};
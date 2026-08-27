#pragma once
#include "CommonTypes.h"

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
    Question() : root(nullptr), tail(nullptr), listSize(0), activeCount(0) {};

    Question(const Question& other);

    Question& operator=(const Question& other);

    ~Question() {
        clear();
    };

    bool insert(const CauHoi& cauhoi);
    bool removeNode(int ID);   // Physical removal (hard delete)
    bool setDeleted(int ID);   // Soft delete: set deleted=true
    bool restoreDeleted(int ID); // Restore soft deleted: set deleted=false
    bool update(int ID, const CauHoi& newData);
    dsCHT* find(int ID);
    bool hasUsedQuestions() const;
    int size() const { return listSize; }
    int activeSize() const { return activeCount; }
    int getActiveCount() const { return activeCount; }
    void swap(Question& other);
    dsCHT* getRoot() const { return root; }
    dsCHT* getTail() const { return tail; }
private:
    dsCHT* root;
    dsCHT* tail;
    int listSize = 0;
    int activeCount = 0;

    void clear();
};

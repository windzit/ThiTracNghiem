#pragma once
#include "./Student.h"
extern std::string PATH_CLASSES_TEXT;

struct Lop {
    std::string MALOP;
    std::string TENLOP;
    Student dssinhvien;
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

class Class {
public:

	Class() : root (new dsLop()) {};
    Class(const Class& other); // class a = class b;
    Class& operator=(const Class& other); // a = b;

    ~Class() {
        clear();
        delete root;
    };

	bool insert(Lop* lop);
	bool remove(const std::string& MALOP);
	bool update(const std::string& MALOP, const std::string& newTENLOP);
	Lop* find(const std::string& MALOP);

    bool save();
    bool load();

    dsLop* getRoot() const { return root; }
private:
	dsLop* root;
    void clear();
};
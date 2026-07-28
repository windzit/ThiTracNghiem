#pragma once
#include <string>
#include "Score.h"
extern std::string PATH_FOLDER_STUDENTS;

struct SinhVien {
	std::string MASV;
	std::string HO;
	std::string TEN;
	std::string PHAI;
	std::string passsword;
	Score dsdiemthi;
};

struct dsSinhVien {
	dsSinhVien* next;
	SinhVien sinhvien;

	dsSinhVien(SinhVien sinhvien) {
		next = nullptr;
		this->sinhvien = sinhvien;
	}
};

class Student {
public:
	Student() : root(nullptr){};
	Student(const Student& other);
	Student& operator=(const Student& other);

	~Student() {
		clear();
	};

	bool insert(const SinhVien& sinhvien);
	bool remove(const std::string& MASV);
	bool update(const std::string& MASV, const SinhVien& newData);
	SinhVien* find(const std::string& MASV);
	int size() const;

	bool save(const std::string& MALOP);
	bool load(const std::string& MALOP);
	dsSinhVien* getRoot() const { return root; }
private:
	dsSinhVien* root;
	void clear();
};
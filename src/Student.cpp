#include "../include/Student.h"
#include "../include/Class.h"
#include <iostream>
using namespace std;

void InitStudentModule() {
    // TODO: Implement Student module.
}

SinhVien* FindStudent_with_MASV(dsSinhVien* root,const string &MASV){
    while(root){
        if(root->sinhvien.MASV == MASV) return &root->sinhvien;
        root = root->next; 
    }
    return nullptr;
}

bool InsertStudent(dsSinhVien* &root, const SinhVien sinhvien){
    dsSinhVien* cur = new dsSinhVien(sinhvien);
    cur->next = root;
    root = cur;
    return true;
}

bool DeleteStudent(dsSinhVien* &root, const std::string& MASV){
    if(root == nullptr) return false;
    if(root->sinhvien.MASV == MASV){
        dsSinhVien* temp = root;
        root = root->next;
        delete temp;
        return true;
    }
    dsSinhVien* cur = root;
    while(cur && cur->next){
        if(cur->next->sinhvien.MASV == MASV){
            dsSinhVien* temp = cur->next;
            cur->next = cur->next->next;
            delete temp;
            return true;
        }
        cur = cur->next;
    }
    return false;
}

bool UpdateStudent(dsSinhVien* root, const std::string& MASV, const SinhVien& newSinhVien){
    if(root == nullptr) return false;
    SinhVien * p = FindStudent_with_MASV(root,MASV);
    if(p == nullptr) return false;
    if(newSinhVien.MASV != "") p->MASV = newSinhVien.MASV;
    if(newSinhVien.HO != "") p->HO = newSinhVien.HO;
    if(newSinhVien.TEN != "") p->TEN = newSinhVien.TEN;
    if(newSinhVien.PHAI != "") p->PHAI = newSinhVien.PHAI;
    if(newSinhVien.passsword != "") p->passsword = newSinhVien.passsword;
    if(newSinhVien.dsdiemthi != nullptr) p->dsdiemthi = newSinhVien.dsdiemthi;
    return true;
}

bool FindStudent(dsSinhVien* root, const std::string& MASV){
    return FindStudent_with_MASV(root,MASV) != nullptr;
}
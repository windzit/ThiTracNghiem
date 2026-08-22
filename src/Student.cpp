#include "../include/Student.h"
#include "../include/Utils.h"
#include "../include/StorageManager.h"

int Student::size() const {
    int count = 0;
    dsSinhVien* cur = root;
    while (cur) {
        count++;
        cur = cur->next;
    }
    return count;
}

Student::Student(const Student& other)
{
    root = nullptr;

    dsSinhVien* p = other.root;
    while (p)
    {
        insert(p->sinhvien);
        p = p->next;
    }
}

Student& Student::operator=(const Student& other)
{
    if (this == &other)
        return *this;

    clear();
    root = nullptr;

    dsSinhVien* p = other.root;
    while (p)
    {
        insert(p->sinhvien);
        p = p->next;
    }

    return *this;
}

void Student::clear() {
    while (root) {
        dsSinhVien* tmp = root;
        root = root->next;
        delete tmp;
    }
}

SinhVien* Student::find(const std::string& MASV) {
    dsSinhVien* temp = root;
    while (temp) {
        if (temp->sinhvien.MASV == MASV)
            return &temp->sinhvien;
        if (temp->sinhvien.MASV > MASV)
            return nullptr;
        temp = temp->next;
    }
    return nullptr;
}

bool Student::insert(const SinhVien& sinhvien) {
    dsSinhVien* cur = new dsSinhVien(sinhvien);

    if (!root) {
        root = cur;
        return true;
    }

    if (root->sinhvien.MASV == sinhvien.MASV) {
        delete cur;
        return false;
    }

    if (root->sinhvien.MASV > cur->sinhvien.MASV) {
        cur->next = root;
        root = cur;
        return true;
    }

    dsSinhVien* temp = root;

    while (temp->next && temp->next->sinhvien.MASV < cur->sinhvien.MASV)
        temp = temp->next;

    if (temp->next && temp->next->sinhvien.MASV == cur->sinhvien.MASV) {
        delete cur;
        return false;
    }

    cur->next = temp->next;
    temp->next = cur;

    return true;
}

bool Student::remove(const std::string& MASV) {
    if (root == nullptr) return false;
    if (root->sinhvien.MASV == MASV) {
        dsSinhVien* temp = root;
        root = root->next;
        delete temp;
        return true;
    }
    dsSinhVien* cur = root;
    while (cur && cur->next) {
        if (cur->next->sinhvien.MASV == MASV) {
            dsSinhVien* temp = cur->next;
            cur->next = cur->next->next;
            delete temp;
            return true;
        }
        else if (MASV < cur->next->sinhvien.MASV) return false;
        cur = cur->next;
    }
    return false;
}

bool Student::update(const std::string& MASV, const SinhVien& newData) {
    SinhVien* sv = find(MASV);
    if (!sv) return false;
    
    // Update editable fields (MASV is immutable)
    sv->HO = newData.HO;
    sv->TEN = newData.TEN;
    sv->PHAI = newData.PHAI;
    sv->passsword = newData.passsword;
    
    return true;
}

bool Student::save(const std::string& MALOP) {
    return true;
}

bool Student::load(const std::string& MALOP) {
    return true;
}
#include <iostream>
#include "../include/Auth.h"
#include "../include/IndexManager.h"
#include "../include/StorageManager.h"

bool login_teacher(std::string &username,std::string &password){
    return (username == "GV" && password == "GV");
}

bool login_student(dsLop &uis, std::string &username, std::string &password) {
    // 1. Fast RAM lookup
    for (int i = 0; i < uis.n; i++) {
        if (uis.dslop[i] == nullptr) continue;
        SinhVien* sv = uis.dslop[i]->dssinhvien.find(username);
        if (sv) {
            return (sv->passsword == password);
        }
    }

    // 2. Disk index fallback
    int64_t offset = -1;
    if (IndexManager::getInstance().getStudentOffset(username, offset)) {
        SinhVien sv;
        std::string malop;
        if (StorageManager::getInstance().readStudentAt(offset, sv, malop)) {
            return (sv.passsword == password);
        }
    }
    return false;
}


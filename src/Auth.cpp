#include <iostream>
#include "../include/Auth.h"
#include "../include/IndexManager.h"
#include "../include/StorageManager.h"

bool login_teacher(std::string &username,std::string &password){
    return (username == "GV" && password == "GV");
}

bool login_student(dsLop &uis, std::string &username,std::string &password){
    int64_t offset = -1;
    if (IndexManager::getInstance().getStudentOffset(username, offset)) {
        SinhVien sv;
        std::string malop;
        if (StorageManager::getInstance().readStudentAt(offset, sv, malop)) {
            if (sv.passsword == password) return true;
        }
    }
    // Fallback: check RAM dsLop for backward compatibility
    for(int i = 0; i < uis.n; i++){
        if(uis.dslop[i] == nullptr) continue;
        SinhVien* sv = uis.dslop[i]->dssinhvien.find(username);
        if(sv && sv->passsword == password) return true;
    }
    return false;
}


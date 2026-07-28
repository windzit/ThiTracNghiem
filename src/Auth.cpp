#include <iostream>
#include "Auth.h"

bool login_teacher(std::string &username,std::string &password){
    return (username == "GV" && password == "GV");
}

bool login_student(dsLop &uis, std::string &username,std::string &password){
    for(int i = 0; i < uis.n; i++){
        if(uis.dslop[i] == nullptr) continue;
        SinhVien* sv = uis.dslop[i]->dssinhvien.find(username);
        if(sv && sv->passsword == password) return true;
    }
    return false;
}

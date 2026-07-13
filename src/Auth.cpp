#include <iostream>
#include "Auth.h"
#include "Init.h"

bool login_teacher(string &username,string &password){
    return (username == "GV" && password == "GV");
}

bool login_student(dsLop &uis, string &username,string &password){
    if(uis.n == 0) return false;
    for(int i = 0 ; i < uis.n ; i++){
        if(uis.dslop[i] == nullptr) continue;
        dsSinhVien * p = uis.dslop[i]->dssinhvien;
        while(p){
            if(p->sinhvien.MASV == username && p->sinhvien.passsword == password) return true;
            p = p->next;
        }
    }
    return false;
}
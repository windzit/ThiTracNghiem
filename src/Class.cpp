#include <iostream>
#include "Init.h"
#include "../include/Class.h"
using namespace std;

bool malop_checker(const string &MALOP){
    return (MALOP.length() > 0 && MALOP.length() <= 15);   
}

bool tenlop_checker(const string &TENLOP){
    return (TENLOP.length() != 0);
}

Lop* find_class_with_malop(dsLop *root,const std::string & MALOP){
    if(root == nullptr || root->n == 0) return nullptr;
    for(int i = 0 ; i < root->n ; i++)
        if(root->dslop[i]->MALOP == MALOP) return root->dslop[i];
    return nullptr;
}

bool FindClass(dsLop* root, const std::string& MALOP){
    if(root == nullptr || root->n == 0) return false;
    return find_class_with_malop(root,MALOP) != nullptr;
}

bool InsertClass(dsLop* root,Lop* lop){
    if(root != nullptr || root->n >= MAXLOP ||
        !malop_checker(lop->MALOP) || !tenlop_checker(lop->TENLOP)
        || FindClass(root,lop->MALOP)
    ) return false;
    root->dslop[root->n++] = lop;
    return true;
}

bool DeleteClass(dsLop* root, const std::string& MALOP){
    if (root == nullptr || root->n == 0) return false;
    for (int i = 0; i < root->n; i++){
        if (root->dslop[i]->MALOP == MALOP){
            delete root->dslop[i];
            for (int j = i; j < root->n - 1; j++)
                root->dslop[j] = root->dslop[j + 1];
            root->dslop[root->n - 1] = nullptr;
            root->n--;
            return true;
        }
    }

    return false;
}
bool UpdateClass(dsLop* root, const std::string& MALOP, const std::string& newTENLOP){
    if(root->n == 0) return false;
    Lop* curlop = find_class_with_malop(root,MALOP);
    if (curlop == nullptr) return false;
    curlop->TENLOP = newTENLOP;
    return true;
}

int demSiSo(dsLop* root, const string &MALOP){
    Lop * curlop = find_class_with_malop(root,MALOP);
    if(curlop == nullptr) return 0;
    dsSinhVien* curstud = curlop->dssinhvien;
    int count = 0;
    while(curstud){
        count++;
        curstud = curstud->next;
    }
    return count;
}
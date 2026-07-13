#include "../include/Subject.h"
#include<iostream>
#include <cstring>
using namespace std;

NodeMH*FindNodeMH(NodeMH* root, const std::string& MAMH) {
    if (root == nullptr) {
        return nullptr;
    }

}

bool InsertSubject(NodeMH* &root, MonHoc monhoc){
    if( root ==nullptr){
        root = new NodeMH(monhoc);
        return true;
    };
    int cmp = strcmp(root->monhoc.MAMH , monhoc.MAMH);
    if ( cmp == 0){
        cout <<"Da ton tai MAMH";
        return false;
    }
    if(cmp>0) return InsertSubject(root->left,monhoc);
    return InsertSubject(root->right,monhoc);
     
    
};

bool DeleteSubject(NodeMH* &root, const std::string& MAMH){};

bool UpdateSubject(NodeMH* &root, const std::string& MAMH, const MonHoc& newMonHoc){};

bool FindSubject(NodeMH* root, const std::string& MAMH, MonHoc& monhoc){};

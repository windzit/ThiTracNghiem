#include "../include/Subject.h"
#include "../include/Question.h"
#include<iostream>
#include <cstring>
using namespace std;

inline  int  CompareMaMH( const char*a , const char*b){
    return strcmp(a,b);
}

NodeMH*FindNodeMH(NodeMH* root, const char MAMH[15]) {
    if (root == nullptr) {
        return nullptr;
    }
    while (root!=nullptr){
        int cmp = CompareMaMH(root->monhoc.MAMH,MAMH);
        if ( cmp == 0)return root;
        if ( cmp  > 0 )root = root ->left;
        else root = root ->right;
    }
    return nullptr;

}



bool InsertSubject(NodeMH* &root, const MonHoc& monhoc){
    if( root ==nullptr){
        root = new NodeMH(monhoc);
        return true;
    };
    int cmp =CompareMaMH(root->monhoc.MAMH , monhoc.MAMH);
    if ( cmp == 0){
        cout <<"Da ton tai MAMH";
        return false;
    }
    if(cmp>0) return InsertSubject(root->left,monhoc);
    return InsertSubject(root->right,monhoc);
     
    
};

// xoa phai thoa : => ton tai mon , cau hoi thi chua dc dung
// xoa mon => giai phong ds cau hoi thi


// xoa dua tren cau hoi
bool CheckUsedQuestion(NodeMH*nodeMH){
    if (nodeMH == nullptr) return false;
    dsCHT*head = nodeMH->monhoc.dsCauHoi;
    if (head == nullptr)return false;
    dsCHT*temp =  head;
    while(temp!=nullptr){
        if (temp->cauhoi.used == true) return true;
        temp = temp->next;
    }
    return false;


}

// xoa  dua tren monhoc( used )
bool CheckUsedSubject(NodeMH*nodeMH){
    if (nodeMH == nullptr) return false;
    if (nodeMH->monhoc.used == true) return true;
    return false;
}

bool DeletedsCH(MonHoc &monhoc){
    dsCHT*head = monhoc.dsCauHoi;
    while (head){
        dsCHT*next = head->next;
        delete head;
        head = next;
    }
    monhoc.dsCauHoi = nullptr;
    return true;
}

NodeMH* findMinNode(NodeMH* node ){
    if (node == nullptr) return nullptr;
    while (node && node->left != nullptr){
        node = node->left;
    }
    return node;
}

void XoaNode(NodeMH*&temp){
    if(temp == nullptr)return;
    temp->left = temp->right = nullptr;
    DeletedsCH(temp->monhoc);
    delete temp;
    temp = nullptr;
    
}

bool DieuKienXoa(NodeMH*&target){
    if (target == nullptr){
        cout <<"Khong ton tai mon hoc ";
        return false;
    }
    if ( CheckUsedQuestion(target) || CheckUsedSubject(target)){
        cout << " Ton tai cau hoi / de thi gan voi Mon Hoc !!! " << endl;
        return false ;
    }
    return true;

}

bool DeleteSubject(NodeMH* &root, const char MAMH[15]){
    if ( root == nullptr) return false;
    // Tim Mon Hoc Can Xoa 
    int cmp = CompareMaMH(root->monhoc.MAMH , MAMH);
    if ( cmp > 0) return DeleteSubject(root->left, MAMH);
    if ( cmp < 0) return DeleteSubject(root->right , MAMH);
    if ( !DieuKienXoa(root)){
        return false;
    }
    // Thuc Hien Xoa
    if ( root->left == nullptr){
        NodeMH*temp = root;
        root = root->right;
        XoaNode(temp);
        return true;
    }
    if ( root->right == nullptr){
        NodeMH*temp = root;
        root = root->left;
        XoaNode(temp);
        return true;
    }

    // transplant 
    else{
        NodeMH*parent = root;
        NodeMH*succ = root->right;
        while (succ->left!=nullptr){
            parent=  succ;
            succ = succ->left;
        }
        if(parent!=root){
            parent->left = succ->right;
            succ->right = root->right;
        }
        succ->left = root->left;
        NodeMH*temp = root;
        root = succ;
        XoaNode(temp);
        return true;
    }

    return false;
};

// Chi cho update TenMH ( ko cho update MaMH) => Thong bao UI
bool UpdateSubject(NodeMH* &root, const char MAMH[15], string newTenMH){
    if (root == nullptr)return false;
    NodeMH*x = FindNodeMH(root , MAMH);
    if( x == nullptr){
        cout <<" Khong ton tai mon Hoc ( UpdateSubject )";
        return false;
    };
    x->monhoc.TENMH = newTenMH;
    return true;
};

bool FindSubject(NodeMH* root, const char MAMH[15], MonHoc& monhoc){
    return FindNodeMH(root,MAMH);
};

#include "../include/Question.h"
#include "../include/Validation.h"
// XU Li ID ( phu thuoc phan file )
static int NextQuestionID = 1;
bool LoadQuestionID();

bool SaveQuestionID();

void InitQuestionID(){
    if(!LoadQuestionID()){
        NextQuestionID = FindMaxQuestionID()+1;
        SaveQuestionID();
    }
};

int GetNextQuestionID(){
    int id = NextQuestionID++;
    SaveQuestionID();
    return id;
};

int FindMaxQuestionID(); // doc file tim ID lon nhat de sinh ID tiep theo
///////////////////////////////////////////////////////////////


void NhapCauHoi(CauHoi &cauhoi,bool update){  // validate tam thoi
    bool usedState = cauhoi.used;
    if ( !update){
        cauhoi.ID = GetNextQuestionID();
    }
    NhapChuoi(cauhoi.NOIDUNG,100);

    NhapChuoi(cauhoi.A,100);
    NhapChuoi(cauhoi.B,100);
    NhapChuoi(cauhoi.C,100);
    NhapChuoi(cauhoi.D,100);

    cauhoi.DAPAN_DUNG = NhapKyTu(); // tam thoi
    cauhoi.used = update ? usedState : false;
}


bool InsertHead(dsCHT* &root, CauHoi cauhoi){
    dsCHT*x = new dsCHT(cauhoi);
    x->next = root;
    root = x;
    return true;
};

bool InsertTail(dsCHT* &root, CauHoi cauhoi){
    dsCHT*x = new dsCHT(cauhoi);
    dsCHT*temp = root;
    while (temp->next!=nullptr){
        temp = temp->next;
    }
    temp->next = x ;
    x->next = nullptr;
    return true;
};

bool DeleteHead(dsCHT* &root, int ID){
    if(root == nullptr) return false;
    dsCHT*temp = root ;
    root = root->next;
    delete temp;
    return true;
};

bool DeleteMid(dsCHT* &temp,dsCHT* &pre){
    pre->next = temp->next;
    delete temp;
    temp = nullptr;
    return true;
};

bool DeleteTail(dsCHT* &temp,dsCHT* &pre){
    pre->next = nullptr;
    delete temp;
    temp = nullptr;
    return true;
};

// FindSubject(NodeMH* root, const char MAMH[15])  = True ( dieu kien )

// Lua mon hoc muon them = > ktra => InsertQuestion(root->monhoc->dsCHT, cauhoi)


// front_end xoa theo kieu tick ô : tick -> trả về ID -> call hàm ở backend để xóa theo ID cap nhat Front_end

// kiem tra mon hoc ton tai khong truoc khi Insert  && ID tu dong tang : 2 TH insert : dau va cuoi

bool InsertQuestion(dsCHT* &root, CauHoi cauhoi){
    if ( root == nullptr){
        return InsertHead(root,cauhoi);
    }
    else return InsertTail(root,cauhoi);

    return false;
};

bool DeleteQuestion(dsCHT* &root, int ID){
    if ( root == nullptr ) return false;
    
    if ( root->cauhoi.ID == ID ){
        if (root->cauhoi.used) {
            return false;
        }
        return DeleteHead(root,ID);
    }
    dsCHT*temp = root;
    dsCHT*pre = nullptr;
    
    while (temp!=nullptr){
        if ( temp->cauhoi.ID == ID){
            break;
        }
        pre = temp;
        temp = temp->next;
    }

    if ( temp == nullptr){ // not found ID 
        // Bao loi KO TON TAI ID
        return false;
    }

    if (temp->cauhoi.used) {
        return false;
    }

    if ( temp->next == nullptr){
        return DeleteTail(temp,pre);
    }
    else return DeleteMid(temp,pre);

    
    return false;
};

bool UpdateQuestion(dsCHT* &root, int ID)
{
    dsCHT* temp = root;

    while (temp != nullptr)
    {
        if (temp->cauhoi.ID == ID)
        {
            CauHoi updated = temp->cauhoi;
            NhapCauHoi(updated, true);   // không đổi ID
            temp->cauhoi = updated;
            return true;
        }

        temp = temp->next;
    }

    // Không tìm thấy
    return false;
}

bool FindQuestionByID(dsCHT* root, int ID, CauHoi& cauhoi)
{
    while (root != nullptr)
    {
        if (root->cauhoi.ID == ID)
        {
            cauhoi = root->cauhoi;
            return true;
        }

        root = root->next;
    }

    return false;
}

#include <cstring>

bool FindQuestionByContent(dsCHT* root, const char* noidung, CauHoi& cauhoi) // neu xai thi noramalize 2 chuoi truoc khi so sanh
{
    while (root != nullptr)
    {
        if (strcmp(root->cauhoi.NOIDUNG, noidung) == 0)
        {
            cauhoi = root->cauhoi;
            return true;
        }

        root = root->next;
    }

    return false;
}
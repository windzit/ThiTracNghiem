#include "../include/Question.h"
#include "../include/Utils.h"
#include "../include/StorageManager.h"

Question::Question(const Question& other)
{
    root = nullptr;
    tail = nullptr;

    dsCHT* p = other.root;

    while (p)
    {
        insert(p->cauhoi);
        p = p->next;
    }
}

Question& Question::operator=(const Question& other)
{
    if (this == &other)
        return *this;

    clear();
    root = nullptr;
    tail = nullptr;

    dsCHT* p = other.root;

    while (p)
    {
        insert(p->cauhoi);
        p = p->next;
    }

    return *this;
}

void Question::clear() {
    while (root)
    {
        dsCHT* tmp = root;
        root = root->next;
        delete tmp;
    }
    tail = nullptr;
}

bool Question::insert(CauHoi& cauhoi, bool autoId){
    if (autoId) {
        cauhoi.ID = generateQuestionID();
    }
    dsCHT* node = new dsCHT(cauhoi);

    // 1. Empty list: node is both root and tail
    if (!root) {
        root = tail = node;
        return true;
    }

    // 2. Fast-path O(1) tail append (happens in 99.99% cases since IDs are auto-incremented)
    if (cauhoi.ID > tail->cauhoi.ID) {
        tail->next = node;
        tail = node;
        return true;
    }

    // 3. Head insert (when ID is smaller than root)
    if (cauhoi.ID < root->cauhoi.ID) {
        node->next = root;
        root = node;
        return true;
    }

    // 4. Middle insert (when ID is between root and tail)
    dsCHT* cur = root;
    while (cur->next && cur->next->cauhoi.ID < cauhoi.ID) {
        cur = cur->next;
    }

    if (cur->cauhoi.ID == cauhoi.ID || (cur->next && cur->next->cauhoi.ID == cauhoi.ID)) {
        delete node;
        return false;
    }

    node->next = cur->next;
    cur->next = node;

    return true;
}

bool Question::removeNode(int ID){
    // Physical removal (hard delete) — no used check, no soft delete
    if (root == nullptr) return false;
    
    if (root->cauhoi.ID == ID){
        dsCHT* temp = root;
        root = root->next;
        if (!root) tail = nullptr;
        delete temp;
        return true;
    }

    dsCHT* temp = root;
    dsCHT* pre = nullptr;
    
    while (temp != nullptr){
        if (temp->cauhoi.ID == ID){
            break;
        }
        pre = temp;
        temp = temp->next;
    }

    if (temp == nullptr) return false;

    pre->next = temp->next;
    if (temp == tail) {
        tail = pre;
    }
    delete temp;
    return true;
}


bool Question::setDeleted(int ID) {
    // Soft delete: set deleted=true, node remains in memory
    dsCHT* node = find(ID);
    if (!node) return false;
    node->cauhoi.deleted = true;
    return true;
}

bool Question::restoreDeleted(int ID) {
    // Restore soft deleted: set deleted=false, node remains in memory
    dsCHT* node = find(ID);
    if (!node) return false;
    node->cauhoi.deleted = false;
    return true;
}

bool Question::hasUsedQuestions() const {
    dsCHT* cur = root;
    while (cur) {
        if (cur->cauhoi.used) return true;
        cur = cur->next;
    }
    return false;
}

bool Question::update(int ID, const CauHoi& newData) {
    dsCHT* node = find(ID);
    if (!node) return false;
    
    // Business rules (used/deleted check) enforced by the service layer (server.cpp)
    // This method only performs the field update
    
    node->cauhoi.NOIDUNG = newData.NOIDUNG;
    node->cauhoi.A = newData.A;
    node->cauhoi.B = newData.B;
    node->cauhoi.C = newData.C;
    node->cauhoi.D = newData.D;
    node->cauhoi.DAPAN_DUNG = newData.DAPAN_DUNG;
    
    return true;
}

dsCHT* Question::find(int ID)
{
    dsCHT* ptr = root;
    while (ptr && ptr->cauhoi.ID < ID)
        ptr = ptr->next;

    if (ptr && ptr->cauhoi.ID == ID)
        return ptr;

    return nullptr;
}

bool Question::save(const char* MAMH) {
    StorageManager::getInstance().markDirty();
    StorageManager::getInstance().incrementOpCount();
    return true;
}

bool Question::load(const char* MAMH) {
    return true;
}

int Question::size() const {
    int count = 0;
    dsCHT* p = root;
    while (p) {
        count++;
        p = p->next;
    }
    return count;
}
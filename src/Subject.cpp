#pragma once
#include "Subject.h"
#include "StorageManager.h"

NodeMH* Subject::clone(NodeMH* node)
{
    if (node == nullptr)
        return nullptr;

    NodeMH* newNode = new NodeMH(node->data);

    newNode->left = clone(node->left);
    newNode->right = clone(node->right);

    return newNode;
}

Subject::Subject(const Subject& other)
{
    root = clone(other.root);
}

Subject& Subject::operator=(const Subject& other)
{
    if (this == &other)
        return *this;

    clear(root);

    root = clone(other.root);

    return *this;
}

void Subject::clear(NodeMH*& node) {
    if (node == nullptr)
        return;

    clear(node->left);
    clear(node->right);

    delete node;
    node = nullptr;
}

bool Subject::_insert(NodeMH*& node, const MonHoc& monhoc) {
    if (node == nullptr) {
        node = new NodeMH(monhoc);
        return true;
    }
    int cmp = strcmp(node->data.MAMH, monhoc.MAMH);
    if (cmp == 0) {
        return false;
    }
    if (cmp > 0) return _insert(node->left, monhoc);
    return _insert(node->right, monhoc);
}

bool Subject::insert(const MonHoc& monhoc) {
    bool result = _insert(root, monhoc);
    if (result) {
        treeSize++;
    }
    return result;
}

// xoa phai thoa : => ton tai mon , cau hoi thi chua dc dung
// xoa mon => giai phong ds cau hoi thi

NodeMH* findMinNode(NodeMH* node) {
    if (node == nullptr) return nullptr;
    while (node && node->left != nullptr) {
        node = node->left;
    }
    return node;
}

bool Subject::_remove(NodeMH*& node, const char MAMH[15]) {
    if (node == nullptr)
        return false;
    // Tim Mon Hoc Can Xoa 
    int cmp = strcmp(node->data.MAMH, MAMH);

    if (cmp > 0)
        return _remove(node->left, MAMH);

    if (cmp < 0)
        return _remove(node->right, MAMH);

    // BR-04: Khong cho xoa neu co cau hoi da duoc dung trong exam
    if (node->data.dsCauHoi.hasUsedQuestions()) {
        return false;
    }
    // Thuc Hien Xoa
    if (node->left == nullptr) {
        NodeMH* temp = node;
        node = node->right;

        delete temp;
        treeSize--;
        return true;
    }

    if (node->right == nullptr) {
        NodeMH* temp = node;
        node = node->left;

        delete temp;
        treeSize--;
        return true;
    }

    // transplant 
    NodeMH* succ = findMinNode(node->right);

    MonHoc tempData = node->data;
    node->data = succ->data;
    succ->data = tempData;

    return _remove(node->right, MAMH);
}

bool Subject::remove(const char MAMH[15]) {
    bool ok = _remove(root, MAMH);
    if (ok) {
        save();
    }
    return ok;
}

bool Subject::update(const char MAMH[15], const std::string& newTENMH) {
    NodeMH* node = find(MAMH);
    if (!node) return false;
    
    // Save old value for rollback
    std::string oldTENMH = node->data.TENMH;
    
    // Update in memory
    node->data.TENMH = newTENMH;
    
    // Persist to storage
    if (!save()) {
        // Rollback on failure
        node->data.TENMH = oldTENMH;
        return false;
    }
    
    return true;
}

NodeMH* Subject::find(const char MAMH[15]) {
    if (root == nullptr) {
        return nullptr;
    }

    NodeMH* node = root;

    while (node != nullptr) {
        int cmp = strcmp(node->data.MAMH, MAMH);
        if (cmp == 0)return node;
        if (cmp > 0)node = node->left;
        else node = node->right;
    }
    return nullptr;
}

bool Subject::save() {
    return StorageManager::getInstance().saveSubjects(*this);
}

bool Subject::load() {
    clear(root);
    return StorageManager::getInstance().loadSubjects(*this);
}



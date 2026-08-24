#include "../include/Question.h"
#include "../include/StorageManager.h"

Question::Question(const Question &other) {
  root = nullptr;
  tail = nullptr;
  listSize = 0;

  dsCHT *p = other.root;

  while (p) {
    insert(p->cauhoi);
    p = p->next;
  }
}

Question &Question::operator=(const Question &other) {
  if (this == &other)
    return *this;

  clear();
  root = nullptr;
  tail = nullptr;
  listSize = 0;

  dsCHT *p = other.root;

  while (p) {
    insert(p->cauhoi);
    p = p->next;
  }

  return *this;
}

void Question::clear() {
  while (root) {
    dsCHT *tmp = root;
    root = root->next;
    delete tmp;
  }
  tail = nullptr;
  listSize = 0;
}

void Question::swap(Question& other) {
  dsCHT *tmpRoot = root;
  dsCHT *tmpTail = tail;
  int tmpSize = listSize;

  root = other.root;
  tail = other.tail;
  listSize = other.listSize;

  other.root = tmpRoot;
  other.tail = tmpTail;
  other.listSize = tmpSize;
}

bool Question::insert(CauHoi &cauhoi, bool autoId) {
    if (autoId) {
        cauhoi.ID = StorageManager::getInstance().getNextQuestionID();
    }
    // 1. Danh sách rỗng: node mới vừa là root vừa là tail
    if (!root) {
        root = tail = new dsCHT(cauhoi);
        listSize++;
        return true;
    }
    // 2. Chèn đuôi O(1) (Do ID tự động tăng toàn cục, ID mới luôn lớn hơn tail->ID)
    if (cauhoi.ID > tail->cauhoi.ID) {
        tail->next = new dsCHT(cauhoi);
        tail = tail->next;
        listSize++;
        return true;
    }
    return false; // Từ chối nếu ID <= tail->cauhoi.ID (trùng lặp hoặc sai thứ tự)
}


bool Question::removeNode(int ID) {
  // Physical removal (hard delete) — no used check, no soft delete
  if (root == nullptr)
    return false;

  if (root->cauhoi.ID == ID) {
    dsCHT *temp = root;
    root = root->next;
    if (!root)
      tail = nullptr;
    delete temp;
    listSize--;
    return true;
  }

  dsCHT *temp = root;
  dsCHT *pre = nullptr;

  while (temp != nullptr) {
    if (temp->cauhoi.ID == ID) {
      break;
    }
    pre = temp;
    temp = temp->next;
  }

  if (temp == nullptr)
    return false;

  pre->next = temp->next;
  if (temp == tail) {
    tail = pre;
  }
  delete temp;
  listSize--;
  return true;
}

bool Question::setDeleted(int ID) {
  // Soft delete: set deleted=true, node remains in memory
  dsCHT *node = find(ID);
  if (!node)
    return false;
  node->cauhoi.deleted = true;
  return true;
}

bool Question::restoreDeleted(int ID) {
  // Restore soft deleted: set deleted=false, node remains in memory
  dsCHT *node = find(ID);
  if (!node)
    return false;
  node->cauhoi.deleted = false;
  return true;
}

bool Question::hasUsedQuestions() const {
  dsCHT *cur = root;
  while (cur) {
    if (cur->cauhoi.used)
      return true;
    cur = cur->next;
  }
  return false;
}

bool Question::update(int ID, const CauHoi &newData) {
  dsCHT *node = find(ID);
  if (!node)
    return false;

  // Business rules (used/deleted check) enforced by the service layer
  // (server.cpp) This method only performs the field update

  node->cauhoi.NOIDUNG = newData.NOIDUNG;
  node->cauhoi.A = newData.A;
  node->cauhoi.B = newData.B;
  node->cauhoi.C = newData.C;
  node->cauhoi.D = newData.D;
  node->cauhoi.DAPAN_DUNG = newData.DAPAN_DUNG;

  return true;
}

dsCHT *Question::find(int ID) {
  dsCHT *ptr = root;
  while (ptr && ptr->cauhoi.ID < ID)
    ptr = ptr->next;

  if (ptr && ptr->cauhoi.ID == ID)
    return ptr;

  return nullptr;
}
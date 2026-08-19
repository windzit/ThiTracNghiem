#include "../include/Question.h"
#include "../include/StorageManager.h"
#include "../include/Utils.h"

Question::Question(const Question &other) {
  root = nullptr;
  tail = nullptr;

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
}

void Question::swap(Question& other) {
  dsCHT *tmpRoot = root;
  dsCHT *tmpTail = tail;
  root = other.root;
  tail = other.tail;
  other.root = tmpRoot;
  other.tail = tmpTail;
}

bool Question::insert(CauHoi &cauhoi, bool autoId) {
  if (autoId) {
    cauhoi.ID = StorageManager::getInstance().getNextQuestionID();
  }
  // 1. Empty list: new node is both root and tail
  if (!root) {
    root = tail = new dsCHT(cauhoi);
    return true;
  }
  // 2. Fast append to tail (ID > tail) - O(1)
  if (cauhoi.ID > tail->cauhoi.ID) {
    tail->next = new dsCHT(cauhoi);
    tail = tail->next;
    return true;
  }
  // 3. Prepend to head (ID < root)
  if (cauhoi.ID < root->cauhoi.ID) {
    dsCHT *newNode = new dsCHT(cauhoi);
    newNode->next = root;
    root = newNode;
    return true;
  }
  // 4. Duplicate check
  if (cauhoi.ID == root->cauhoi.ID || cauhoi.ID == tail->cauhoi.ID) {
    return false;
  }
  // 5. Sorted middle insertion for out-of-order IDs
  dsCHT *cur = root;
  while (cur->next && cur->next->cauhoi.ID < cauhoi.ID) {
    cur = cur->next;
  }
  if (cur->next && cur->next->cauhoi.ID == cauhoi.ID) {
    return false; // Reject duplicate ID
  }
  dsCHT *newNode = new dsCHT(cauhoi);
  newNode->next = cur->next;
  cur->next = newNode;
  return true;
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

// Note: File I/O persistence and loading for questions are fully managed by StorageManager.
// These methods trigger dirty state notifications for batch persistence.
bool Question::save(const char *MAMH) {
  StorageManager::getInstance().markDirty();
  StorageManager::getInstance().incrementOpCount();
  return true;
}

// Note: Loaded directly into RAM data structures via StorageManager::loadQuestions.
bool Question::load(const char *MAMH) { return true; }

int Question::size() const {
  int count = 0;
  dsCHT *p = root;
  while (p) {
    count++;
    p = p->next;
  }
  return count;
}
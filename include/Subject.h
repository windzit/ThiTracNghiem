#pragma once
#include "Question.h"
#include <string>

struct MonHoc {
    char MAMH[16] = {0};
    std::string TENMH;
    bool used = false;
    Question dsCauHoi;
};

struct NodeMH {
    MonHoc data;
    NodeMH* left;
    NodeMH* right;

    NodeMH(const MonHoc& monhoc)
        : data(monhoc), left(nullptr), right(nullptr) {
    }
};

class Subject {
public:
    Subject() : root(nullptr) {};

    Subject(const Subject& other);

    Subject& operator=(const Subject& other);

    ~Subject() {
        clear(root);
    }

    bool insert(const MonHoc& monhoc);
    bool remove(const char MAMH[16]);
    bool update(const char MAMH[16], const std::string& newTENMH);
    NodeMH* find(const char MAMH[16]);

    NodeMH* getRoot() const { return root; }
    int size() const { return treeSize; }
private:
    NodeMH* root;
    int treeSize = 0;
    void clear(NodeMH*& node);
    bool _insert(NodeMH*& node, const MonHoc& monhoc);
    bool _remove(NodeMH*& node, const char MAMH[16]);
    NodeMH* clone(NodeMH* node);
};
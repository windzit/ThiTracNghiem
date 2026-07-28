#include "../include/Class.h"
#include "../include/StorageManager.h"

Class::Class(const Class& other)
{
    root = new dsLop();

    for (int i = 0; i < other.root->n; i++)
    {
        Lop* lop = new Lop(*other.root->dslop[i]);

        root->dslop[root->n++] = lop;
    }
}

Class& Class::operator=(const Class& other)
{
    if (this == &other)
        return *this;

    clear();
    delete root;

    root = new dsLop();

    for (int i = 0; i < other.root->n; i++)
    {
        Lop* lop = new Lop(*other.root->dslop[i]);

        root->dslop[root->n++] = lop;
    }

    return *this;
}

void Class::clear() {
    for (int i = 0; i < root->n; i++) {
        delete root->dslop[i];
    }

    root->n = 0;
}

Lop* Class::find(const std::string& MALOP){
    if (root == nullptr || root->n == 0) return nullptr;
    for (int i = 0; i < root->n; i++)
        if (root->dslop[i]->MALOP == MALOP) return root->dslop[i];
    return nullptr;
}

bool Class::insert(Lop* lop) {
    if(root == nullptr || root->n >= MAXLOP || this->find(lop->MALOP)) 
        return false;
    root->dslop[root->n++] = lop;
    
    // Persist to storage immediately
    if (!save()) {
        // Rollback on failure
        root->n--;
        return false;
    }
    
    return true;
}

bool Class::remove(const std::string& MALOP) {
    if (root == nullptr || root->n == 0) return false;

    for (int i = 0; i < root->n; i++){
        if (root->dslop[i]->MALOP == MALOP){
            // Save the deleted class for potential rollback
            Lop* deletedLop = root->dslop[i];
            
            // Remove from memory first
            for (int j = i; j < root->n - 1; j++)
                root->dslop[j] = root->dslop[j + 1];
            root->dslop[root->n - 1] = nullptr;
            root->n--;
            
            // Persist to storage
            if (!save()) {
                // Rollback on failure - restore the deleted class
                for (int j = root->n; j > i; j--)
                    root->dslop[j] = root->dslop[j - 1];
                root->dslop[i] = deletedLop;
                root->n++;
                return false;
            }
            
            // Save succeeded, now delete the class object
            delete deletedLop;
            return true;
        }
    }

    return false;
}

bool Class::update(const std::string& MALOP, const std::string& newTENLOP) {
    Lop* lop = find(MALOP);
    if (!lop) return false;
    
    // Save old value for rollback
    std::string oldTENLOP = lop->TENLOP;
    
    // Update in memory
    lop->TENLOP = newTENLOP;
    
    // Persist to storage
    if (!save()) {
        // Rollback on failure
        lop->TENLOP = oldTENLOP;
        return false;
    }
    
    return true;
}

bool Class::save() {
    return StorageManager::getInstance().saveClasses(*this);
}

bool Class::load() {
    clear();
    return StorageManager::getInstance().loadClasses(*this);
}
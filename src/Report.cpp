#include <iostream>
#include "../include/Report.h"
#include "Init.h"
#include "../include/Class.h"

using namespace std;

void PrintClassListReport(dsLop* root) {
    if(root == nullptr || root->n == 0) return;
    for(int i = 0 ; i < root->n ; i++){
        if(root->dslop[i] == nullptr) continue;
        cout << "MA LOP: " << root->dslop[i]->MALOP;
        cout << "TEN LOP: " << root->dslop[i]->TENLOP;
        cout << "SI SO: " << demSiSo(root,root->dslop[i]->MALOP) << endl;
    }
}

void PrintExamDetailReport() {
    // TODO: Implement exam detail report.
}

void PrintScoreboardReport() {
    // TODO: Implement score board report.
}

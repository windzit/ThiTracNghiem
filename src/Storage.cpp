#include "../include/Storage.h"
#include "../include/StorageManager.h"
#include "../include/PathResolver.h"
#include "../include/DArray.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

static void countQuestionsRecursive(NodeMH* node, int& count) {
    if (!node) return;
    countQuestionsRecursive(node->left, count);
    count += node->data.dsCauHoi.size();
    countQuestionsRecursive(node->right, count);
}

static void collectSubjectsRecursive(NodeMH* node, DArray<std::string>& codes) {
    if (!node) return;
    collectSubjectsRecursive(node->left, codes);
    codes.push_back(node->data.MAMH);
    collectSubjectsRecursive(node->right, codes);
}

static std::string formatNumber(int number) {
    std::string s = std::to_string(number);
    int n = s.length();
    for (int i = n - 3; i > 0; i -= 3) {
        s.insert(i, ",");
    }
    return s;
}

void PrintStartupReport(Class& dsl, Subject& dsmh, long long totalLoadingTimeMs, long long classLoadMs, long long subjectLoadMs) {
    int classCount = dsl.getRoot() ? dsl.getRoot()->n : 0;
    int studentCount = 0;
    int scoreCount = 0;

    DArray<std::string> classList;
    DArray<std::string> studentIdSet;
    DArray<std::string> duplicateStudentIds;

    if (dsl.getRoot()) {
        for (int i = 0; i < dsl.getRoot()->n; i++) {
            Lop* lop = dsl.getRoot()->dslop[i];
            if (!lop) continue;
            classList.push_back(lop->MALOP);
            studentCount += lop->dssinhvien.size();

            dsSinhVien* cur = lop->dssinhvien.getRoot();
            while (cur) {
                if (studentIdSet.contains(cur->sinhvien.MASV)) {
                    duplicateStudentIds.push_back(cur->sinhvien.MASV);
                } else {
                    studentIdSet.push_back(cur->sinhvien.MASV);
                }
                scoreCount += cur->sinhvien.dsdiemthi.count();
                cur = cur->next;
            }
        }
    }

    int subjectCount = dsmh.size();
    int questionCount = 0;
    countQuestionsRecursive(dsmh.getRoot(), questionCount);

    DArray<std::string> subjectList;
    collectSubjectsRecursive(dsmh.getRoot(), subjectList);

    std::cout << "\n======================================================================\n";
    std::cout << "                  PTIT CBT SERVER STARTUP REPORT                      \n";
    std::cout << "======================================================================\n";
    std::cout << "Loading storage from disk...\n\n";

    std::cout << "  [OK] Classes        : " << formatNumber(classCount) << "\n";
    std::cout << "  [OK] Students       : " << formatNumber(studentCount) << "\n";
    std::cout << "  [OK] Subjects       : " << formatNumber(subjectCount) << "\n";
    std::cout << "  [OK] Questions      : " << formatNumber(questionCount) << "\n";
    std::cout << "  [OK] Scores         : " << formatNumber(scoreCount) << "\n";
    std::cout << "  [OK] Exam Sessions  : " << formatNumber(StorageManager::getInstance().getActiveSessionCount()) << "\n\n";

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "LOADING TIME STATISTICS\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "  [*] Loading Classes & Students : " << classLoadMs << " ms\n";
    std::cout << "  [*] Loading Subjects & Questions: " << subjectLoadMs << " ms\n";
    std::cout << "  -----------------------------------------\n";
    std::cout << "  [*] Total Loading Time          : " << totalLoadingTimeMs << " ms\n\n";

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "STORAGE CONSISTENCY AUDIT\n";
    std::cout << "----------------------------------------------------------------------\n";

    // Audit 1: students.txt consistency (flat file vs RAM)
    {
        std::ifstream file(PathResolver::getFilePath("students.txt"));
        if (!file.is_open()) {
            std::cout << "  [WARNING] Missing flat file: students.txt\n";
        } else {
            std::string line;
            int fileStudentCount = 0;
            bool isHeader = true;
            while (std::getline(file, line)) {
                std::string trimmed = line;
                // trim trailing whitespace/CR
                while (!trimmed.empty() && (trimmed.back() == '\r' || trimmed.back() == ' ' || trimmed.back() == '\t')) {
                    trimmed.pop_back();
                }
                if (trimmed.empty()) continue;
                if (isHeader) { isHeader = false; continue; }
                fileStudentCount++;
            }
            file.close();
            if (fileStudentCount == studentCount) {
                std::cout << "  [OK] students.txt consistent: " << fileStudentCount << " records match RAM.\n";
            } else {
                std::cout << "  [WARNING] students.txt mismatch: file=" << fileStudentCount << " vs RAM=" << studentCount << "\n";
            }
        }
    }

    // Audit 2: questions.txt consistency (flat file vs RAM)
    {
        std::ifstream file(PathResolver::getFilePath("questions.txt"));
        if (!file.is_open()) {
            std::cout << "  [WARNING] Missing flat file: questions.txt\n";
        } else {
            std::string line;
            int fileQuestionCount = 0;
            bool isHeader = true;
            while (std::getline(file, line)) {
                std::string trimmed = line;
                while (!trimmed.empty() && (trimmed.back() == '\r' || trimmed.back() == ' ' || trimmed.back() == '\t')) {
                    trimmed.pop_back();
                }
                if (trimmed.empty()) continue;
                if (isHeader) { isHeader = false; continue; }
                fileQuestionCount++;
            }
            file.close();
            if (fileQuestionCount == questionCount) {
                std::cout << "  [OK] questions.txt consistent: " << fileQuestionCount << " records match RAM.\n";
            } else {
                std::cout << "  [WARNING] questions.txt mismatch: file=" << fileQuestionCount << " vs RAM=" << questionCount << "\n";
            }
        }
    }

    // Audit 3: Duplicate Subject Codes
    DArray<std::string> uniqueSubj;
    for (int i = 0; i < subjectList.size(); i++) {
        if (!uniqueSubj.contains(subjectList[i])) {
            uniqueSubj.push_back(subjectList[i]);
        }
    }
    if (uniqueSubj.size() == subjectList.size()) {
        std::cout << "  [OK] No duplicate subject codes detected.\n";
    } else {
        std::cout << "  [WARNING] Duplicate subject codes detected!\n";
    }

    // Audit 4: Duplicate Student IDs
    if (duplicateStudentIds.empty()) {
        std::cout << "  [OK] No duplicate student IDs detected.\n";
    } else {
        std::cout << "  [WARNING] Duplicate student IDs detected: " << duplicateStudentIds.size() << "\n";
    }

    std::cout << "\n======================================================================\n";
    std::cout << "[DEVELOPMENT MODE NOTICE]\n";
    std::cout << "Manual changes inside storage/ require RESTARTING the server.\n";
    std::cout << "Browser Refresh alone will NOT reload storage files into server RAM.\n";
    std::cout << "======================================================================\n\n";
}

bool LoadAllData(Class& dsl, Subject& dsmh) {
    auto tStart = std::chrono::high_resolution_clock::now();

    auto tClassStart = std::chrono::high_resolution_clock::now();
    StorageManager::getInstance().loadAllData(dsl, dsmh);
    auto tClassEnd = std::chrono::high_resolution_clock::now();
    long long classLoadMs = std::chrono::duration_cast<std::chrono::milliseconds>(tClassEnd - tClassStart).count();
    long long subjectLoadMs = classLoadMs;

    auto tEnd = std::chrono::high_resolution_clock::now();
    long long totalMs = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart).count();

    PrintStartupReport(dsl, dsmh, totalMs, classLoadMs, subjectLoadMs);
    return true;
}

bool SaveAllData(Class& dsl, Subject& dsmh) {
    return StorageManager::getInstance().saveAllData(dsl, dsmh);
}

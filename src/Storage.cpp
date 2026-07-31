#include "../include/Storage.h"
#include "../include/StorageManager.h"
#include "../include/PathResolver.h"
#include "../include/DArray.h"
#include "../include/IndexManager.h"
#include "../include/CommonTypes.h"
#include "../include/Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

static std::string makeProgressBar(int current, int max, int width = 20) {
    if (max <= 0) max = 1;
    double ratio = (double)current / (double)max;
    if (ratio > 1.0) ratio = 1.0;
    if (ratio < 0.0) ratio = 0.0;
    int filled = (int)(ratio * width);
    std::string bar = "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) bar += "=";
        else bar += "-";
    }
    bar += "]";
    return bar;
}

static void printCompactionProgressAudit() {
    int delClass = StorageManager::getInstance().getDeletedCount("class");
    int maxClass = StorageConfig::CLASS_COMPACT_COUNT;
    int pctClass = (int)((double)delClass / maxClass * 100);
    if (pctClass > 100) pctClass = 100;

    int delStudent = StorageManager::getInstance().getDeletedCount("student");
    int maxStudent = StorageConfig::STUDENT_COMPACT_COUNT;
    int pctStudent = (int)((double)delStudent / maxStudent * 100);
    if (pctStudent > 100) pctStudent = 100;

    int delSubject = StorageManager::getInstance().getDeletedCount("subject");
    int maxSubject = StorageConfig::SUBJECT_COMPACT_COUNT;
    int pctSubject = (int)((double)delSubject / maxSubject * 100);
    if (pctSubject > 100) pctSubject = 100;

    int delQuestion = StorageManager::getInstance().getDeletedCount("question");
    int maxQuestion = StorageConfig::QUESTION_COMPACT_COUNT;
    int pctQuestion = (int)((double)delQuestion / maxQuestion * 100);
    if (pctQuestion > 100) pctQuestion = 100;

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "STORAGE COMPACTION THRESHOLD AUDIT (Soft-Deleted vs Threshold)\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "  [*] Classes   : " << makeProgressBar(delClass, maxClass) << " "
              << std::setw(3) << pctClass << "% (" << delClass << " / " << maxClass << " deleted)\n";
    std::cout << "  [*] Students  : " << makeProgressBar(delStudent, maxStudent) << " "
              << std::setw(3) << pctStudent << "% (" << delStudent << " / " << maxStudent << " deleted)\n";
    std::cout << "  [*] Subjects  : " << makeProgressBar(delSubject, maxSubject) << " "
              << std::setw(3) << pctSubject << "% (" << delSubject << " / " << maxSubject << " deleted)\n";
    std::cout << "  [*] Questions : " << makeProgressBar(delQuestion, maxQuestion) << " "
              << std::setw(3) << pctQuestion << "% (" << delQuestion << " / " << maxQuestion << " deleted)\n";
    std::cout << "  --------------------------------------------------------------------\n";

    bool thresholdReached = (delClass >= maxClass || delStudent >= maxStudent ||
                             delSubject >= maxSubject || delQuestion >= maxQuestion);

    if (thresholdReached) {
        std::cout << "  [*] Compaction Status: THRESHOLD REACHED -> Auto-Compaction Executed.\n\n";
    } else {
        std::cout << "  [*] Compaction Status: OK (Storage healthy, no compaction required)\n\n";
    }
}

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

    IndexManager::getInstance().auditAndLoadIndexes();
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
    printCompactionProgressAudit();

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "STORAGE CONSISTENCY AUDIT\n";
    std::cout << "----------------------------------------------------------------------\n";

    // Audit 1: classes.txt consistency (flat file vs RAM)
    {
        std::ifstream file(PathResolver::getFilePath("classes.txt"));
        if (!file.is_open()) {
            std::cout << "  [WARNING] Missing flat file: classes.txt\n";
        } else {
            std::string line;
            int fileClassCount = 0;
            bool isHeader = true;
            while (std::getline(file, line)) {
                std::string trimmed = trim(line);
                if (trimmed.empty()) continue;
                if (isHeader) { isHeader = false; continue; }
                DArray<std::string> tokens = split(trimmed, '|');
                if (tokens.size() >= 3 && trim(tokens[2]) == "1") continue;
                fileClassCount++;
            }
            file.close();
            if (fileClassCount == classCount) {
                std::cout << "  [OK] classes.txt consistent: " << fileClassCount << " active records match RAM.\n";
            } else {
                std::cout << "  [WARNING] classes.txt mismatch: file active=" << fileClassCount << " vs RAM=" << classCount << "\n";
            }
        }
    }

    // Audit 2: students.txt consistency (flat file vs RAM)
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
                if (trimmed.empty()) continue;
                if (isHeader) { isHeader = false; continue; }
                DArray<std::string> tokens = split(trimmed, '|');
                if (tokens.size() >= 7 && trim(tokens[6]) == "1") continue;
                fileStudentCount++;
            }
            file.close();
            if (fileStudentCount == studentCount) {
                std::cout << "  [OK] students.txt consistent: " << fileStudentCount << " active records match RAM.\n";
            } else {
                std::cout << "  [WARNING] students.txt mismatch: file active=" << fileStudentCount << " vs RAM=" << studentCount << "\n";
            }
        }
    }

    // Audit 3: subjects.txt consistency (flat file vs RAM)
    {
        std::ifstream file(PathResolver::getFilePath("subjects.txt"));
        if (!file.is_open()) {
            std::cout << "  [WARNING] Missing flat file: subjects.txt\n";
        } else {
            std::string line;
            int fileSubjectCount = 0;
            bool isHeader = true;
            while (std::getline(file, line)) {
                std::string trimmed = trim(line);
                if (trimmed.empty()) continue;
                if (isHeader) { isHeader = false; continue; }
                DArray<std::string> tokens = split(trimmed, '|');
                if (tokens.size() >= 4 && trim(tokens[3]) == "1") continue;
                fileSubjectCount++;
            }
            file.close();
            if (fileSubjectCount == subjectCount) {
                std::cout << "  [OK] subjects.txt consistent: " << fileSubjectCount << " active records match RAM.\n";
            } else {
                std::cout << "  [WARNING] subjects.txt mismatch: file active=" << fileSubjectCount << " vs RAM=" << subjectCount << "\n";
            }
        }
    }

    // Audit 4: questions.txt consistency (flat file vs RAM)
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
                if (trimmed.empty()) continue;
                if (isHeader) { isHeader = false; continue; }

                DArray<std::string> tokens = split(trimmed, '|');
                if (tokens.size() >= 9 && trim(tokens[8]) == "1") continue;
                std::string mamh = trim(tokens[0]);
                if (!dsmh.find(mamh.c_str())) continue; // Skip questions belonging to deleted subjects
                fileQuestionCount++;
            }
            file.close();
            if (fileQuestionCount == questionCount) {
                std::cout << "  [OK] questions.txt consistent: " << fileQuestionCount << " active records match RAM.\n";
            } else {
                std::cout << "  [WARNING] questions.txt mismatch: file active=" << fileQuestionCount << " vs RAM=" << questionCount << "\n";
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

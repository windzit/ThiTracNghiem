#pragma once
#include <string>
#include <ctime>
#include "DArray.h"

class Subject;
struct SinhVien;

struct ExamSession {
    std::string MASV;
    std::string MAMH;
    DArray<int> questionIds;
    DArray<char> answers;
    std::time_t thoiGianBatDau;
    int tongThoiGianPhut;
    bool in_progress;
    std::time_t lastServerActivityAt;
};

// Exam Session Storage functions
bool saveExamSession(const ExamSession& session);
bool loadExamSession(const std::string& masv, ExamSession& session);
bool removeExamSession(const std::string& masv);
int calculateRemainingSeconds(const ExamSession& session);

// Downtime compensation on server startup
void adjustSessionsForDowntime(std::time_t serverStartupTime);
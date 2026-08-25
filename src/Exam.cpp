#include "../include/Exam.h"
#include "../include/StorageManager.h"
#include "../include/Subject.h"
#include "../include/Student.h"
#include "../include/Utils.h"
#include "../include/Utility/Swap.h"
#include "../include/Utility/Random.h"
#include "../include/DArray.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <limits>

bool saveExamSession(const ExamSession& session) {
    return StorageManager::getInstance().saveExamSession(session);
}

bool loadExamSession(const std::string& MASV, ExamSession& session) {
    if (MASV.empty()) return false;
    const DArray<ExamSession>& cached = StorageManager::getInstance().getCachedExamSessions();
    for (int i = 0; i < cached.size(); i++) {
        if (cached[i].MASV == MASV && cached[i].in_progress) {
            session = cached[i];
            return true;
        }
    }
    return false;
}

bool removeExamSession(const std::string& MASV) {
    return StorageManager::getInstance().removeExamSession(MASV);
}

int calculateRemainingSeconds(const ExamSession& session) {
    if (!session.in_progress) return 0;
    int64_t now = std::time(nullptr);
    int totalSeconds = session.tongThoiGianPhut * 60;
    int elapsed = (int)(now - session.thoiGianBatDau);
    int remaining = totalSeconds - elapsed;
    return remaining > 0 ? remaining : 0;
}

void adjustSessionsForDowntime(std::time_t serverStartupTime) {
    // Load all active exam sessions from flat file via StorageManager
    DArray<ExamSession> sessions;
    if (!StorageManager::getInstance().loadExamSessions(sessions)) return;

    for (const auto& s : sessions) {
        if (s.in_progress && s.lastServerActivityAt > 0 && serverStartupTime > s.lastServerActivityAt) {
            std::time_t downtimeGap = serverStartupTime - s.lastServerActivityAt;
            ExamSession updated = s;
            updated.thoiGianBatDau += downtimeGap;
            updated.lastServerActivityAt = serverStartupTime;
            saveExamSession(updated);
            std::cout << "[ExamDowntime] Compensated " << downtimeGap << "s downtime for MASV: " << updated.MASV << "\n";
        }
    }
}
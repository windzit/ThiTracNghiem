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
    DArray<ExamSession> sessions;
    if (!StorageManager::getInstance().loadExamSessions(sessions)) return false;
    for (const auto& s : sessions) {
        if (s.MASV == MASV && s.in_progress) {
            session = s;
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

// Helper: clear input buffer
static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Helper: random select questions from a subject
static DArray<dsCHT*> randomSelectQuestions(Question& dsCauHoi, int soCau) {
    DArray<dsCHT*> allQuestions;
    for (int id = 0; id < 10000; id++) {
        dsCHT* q = dsCauHoi.find(id);
        if (q) {
            allQuestions.push_back(q);
        }
    }
    
    DArray<dsCHT*> selected;
    if (allQuestions.size() <= soCau) {
        selected = allQuestions;
    } else {
        Utility::Random::Seed();
        DArray<int> indices;
        for (int i = 0; i < allQuestions.size(); i++) {
            indices.push_back(i);
        }
        for (int i = indices.size() - 1; i > 0; i--) {
            int j = Utility::Random::RandomInt(0, i);
            Utility::Swap(indices[i], indices[j]);
        }
        for (int i = 0; i < soCau; i++) {
            selected.push_back(allQuestions[indices[i]]);
        }
    }
    return selected;
}

void ThiTracNghiem(Subject& dsmh, SinhVien* sv) {
    if (!sv) {
        std::cout << "Loi: Sinh vien khong hop le.\n";
        return;
    }
    
    char mamh[15];
    std::cout << "\n=== THI TRAC NGHIEM ===\n";
    std::cout << "Nhap MAMH: ";
    std::cin >> mamh;
    
    NodeMH* monhoc = dsmh.find(mamh);
    if (!monhoc) {
        std::cout << "Mon hoc khong ton tai.\n";
        return;
    }
    
    if (sv->dsdiemthi.find(mamh) != nullptr) {
        std::cout << "Ban da thi mon nay roi.\n";
        return;
    }
    
    int totalQuestions = 0;
    for (int id = 0; id < 10000; id++) {
        if (monhoc->data.dsCauHoi.find(id)) {
            totalQuestions++;
        }
    }
    
    if (totalQuestions == 0) {
        std::cout << "Mon hoc nay chua co cau hoi nao.\n";
        return;
    }
    
    int soCau;
    std::cout << "Co " << totalQuestions << " cau hoi co san.\n";
    std::cout << "Nhap so cau hoi se thi: ";
    std::cin >> soCau;
    
    if (soCau <= 0 || soCau > totalQuestions) {
        std::cout << "So cau hoi khong hop le (phai tu 1 den " << totalQuestions << ").\n";
        return;
    }
    
    int soPhut;
    std::cout << "Nhap so phut thi: ";
    std::cin >> soPhut;
    
    if (soPhut <= 0) {
        std::cout << "So phut khong hop le.\n";
        return;
    }
    
    DArray<dsCHT*> selectedQuestions = randomSelectQuestions(monhoc->data.dsCauHoi, soCau);
    for (dsCHT* q : selectedQuestions) {
        q->cauhoi.used = true;
    }
    
    std::cout << "\n=== BAT DAU THI ===\n";
    std::cout << "Thoi gian: " << soPhut << " phut\n";
    std::cout << "So cau hoi: " << selectedQuestions.size() << "\n\n";
    
    auto startTime = std::chrono::steady_clock::now();
    int soDung = 0;
    DArray<char> answers;
    
    for (size_t i = 0; i < selectedQuestions.size(); i++) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(currentTime - startTime);
        
        if (elapsed.count() >= soPhut) {
            std::cout << "\n=== HET GIO ===\n";
            std::cout << "Tu dong nop bai.\n";
            break;
        }
        
        dsCHT* q = selectedQuestions[i];
        std::cout << "Cau " << (i + 1) << " (ID=" << q->cauhoi.ID << "): " << q->cauhoi.NOIDUNG << "\n";
        std::cout << "  A. " << q->cauhoi.A << "\n";
        std::cout << "  B. " << q->cauhoi.B << "\n";
        std::cout << "  C. " << q->cauhoi.C << "\n";
        std::cout << "  D. " << q->cauhoi.D << "\n";
        
        char answer;
        std::cout << "  Chon (A/B/C/D): ";
        std::cin >> answer;
        
        answer = toupper(answer);
        answers.push_back(answer);
        
        if (answer == q->cauhoi.DAPAN_DUNG) {
            soDung++;
            std::cout << "  [DUNG]\n\n";
        } else {
            std::cout << "  [SAI] (Dap an dung: " << q->cauhoi.DAPAN_DUNG << ")\n\n";
        }
    }
    
    int totalAnswered = answers.size();
    float diem = 0.0f;
    
    if (totalAnswered > 0) {
        diem = ((float)soDung / totalAnswered) * 10.0f;
        diem = roundScore(diem, 2);
    }
    
    std::cout << "\n=== KET QUA THI ===\n";
    std::cout << "Mon hoc: " << monhoc->data.TENMH << "\n";
    std::cout << "So cau dung: " << soDung << "/" << totalAnswered << "\n";
    std::cout << "Diem: " << diem << "\n";
    
    DiemThi dt;
    std::strcpy(dt.MAMH, mamh);
    dt.DIEM = diem;
    
    if (sv->dsdiemthi.insert(dt)) {
        std::cout << "Da luu diem.\n";
    } else {
        std::cout << "Loi: Khong the luu diem (co the da ton tai).\n";
    }
    
    std::cout << "\n=== HOAN THANH ===\n";
}
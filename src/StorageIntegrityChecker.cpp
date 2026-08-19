#include "../include/StorageIntegrityChecker.h"
#include "../include/StorageValidator.h"
#include "../include/IndexManager.h"
#include "../include/Utils.h"
#include <iostream>

static void logIntegrityWarning(const std::string& relation, const std::string& key, const std::string& missingTarget) {
    std::cerr << "\n[INTEGRITY WARNING]\n"
              << "Relation:       " << relation << "\n"
              << "Source Key:     " << key << "\n"
              << "Missing Target: " << missingTarget << "\n"
              << "Reason:         Orphan reference detected during startup integrity check.\n"
              << std::endl;
}

static bool isStudentInRAM(dsLop* rootLop, const std::string& masv) {
    int64_t dummyOffset = -1;
    if (IndexManager::getInstance().getStudentOffset(masv, dummyOffset)) return true;
    if (!rootLop) return false;
    for (int k = 0; k < rootLop->n; k++) {
        if (rootLop->dslop[k] && rootLop->dslop[k]->dssinhvien.find(masv)) {
            return true;
        }
    }
    return false;
}

bool StorageIntegrityChecker::auditStorageIntegrity(Class& dsl, Subject& dsmh, const DArray<ExamSession>& sessions) {
    int warningsCount = 0;
    dsLop* rootLop = dsl.getRoot();

    // 1. Audit Scores -> Subject
    if (rootLop) {
        for (int i = 0; i < rootLop->n; i++) {
            Lop* lop = rootLop->dslop[i];
            if (!lop) continue;
            dsSinhVien* curSV = lop->dssinhvien.getRoot();
            while (curSV) {
                const SinhVien& sv = curSV->sinhvien;
                dsDiemThi* curScore = sv.dsdiemthi.getRoot();
                while (curScore) {
                    const DiemThi& dt = curScore->diemthi;
                    std::string mamh = trim(dt.MAMH);
                    if (!dsmh.find(mamh.c_str())) {
                        warningsCount++;
                        logIntegrityWarning("Score -> Subject", "MASV: " + sv.MASV + ", MAMH: " + mamh, "Subject: " + mamh);
                    }
                    curScore = curScore->next;
                }
                curSV = curSV->next;
            }
        }
    }

    // 2. Audit Active Exam Sessions -> Student & Subject
    for (int i = 0; i < sessions.size(); i++) {
        const auto& sess = sessions[i];
        if (!sess.in_progress) continue;

        std::string masv = trim(sess.MASV);
        std::string mamh = trim(sess.MAMH);

        if (!isStudentInRAM(rootLop, masv)) {
            warningsCount++;
            logIntegrityWarning("ExamSession -> Student", "Active MASV: " + masv, "Student: " + masv);
        }

        if (!dsmh.find(mamh.c_str())) {
            warningsCount++;
            logIntegrityWarning("ExamSession -> Subject", "Active MAMH: " + mamh, "Subject: " + mamh);
        }
    }

    if (warningsCount > 0) {
        std::cerr << "[StorageIntegrityChecker] Completed audit with " << warningsCount << " integrity warnings.\n";
    } else {
        std::cout << "[StorageIntegrityChecker] Storage integrity audit PASSED (0 orphan references).\n";
    }

    return true; // Non-blocking audit: returns true so server startup proceeds safely
}

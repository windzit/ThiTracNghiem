#include "../include/StorageIntegrityChecker.h"
#include "../include/StorageValidator.h"
#include <iostream>

static void logIntegrityWarning(const std::string& relation, const std::string& key, const std::string& missingTarget) {
    std::cerr << "\n[INTEGRITY WARNING]\n"
              << "Relation:       " << relation << "\n"
              << "Source Key:     " << key << "\n"
              << "Missing Target: " << missingTarget << "\n"
              << "Reason:         Orphan reference detected during startup integrity check.\n"
              << std::endl;
}

bool StorageIntegrityChecker::auditStorageIntegrity(Class& dsl, Subject& dsmh, const DArray<ExamSession>& sessions) {
    int warningsCount = 0;
    dsLop* rootLop = dsl.getRoot();

    // 1. Audit Students -> Class
    if (rootLop) {
        for (int i = 0; i < rootLop->n; i++) {
            Lop* lop = rootLop->dslop[i];
            if (!lop) continue;
            std::string classCode = StorageValidator::trim(lop->MALOP);
            if (!dsl.find(classCode)) {
                warningsCount++;
                logIntegrityWarning("Student -> Class", "MALOP: " + classCode, "Class: " + classCode);
            }
        }
    }

    // 2. Audit Scores -> Student & Subject
    if (rootLop) {
        for (int i = 0; i < rootLop->n; i++) {
            Lop* lop = rootLop->dslop[i];
            if (!lop) continue;
            dsSinhVien* curSV = lop->dssinhvien.getRoot();
            while (curSV) {
                const SinhVien& sv = curSV->sinhvien;
                dsDiemThi* curScore = const_cast<SinhVien&>(sv).dsdiemthi.getRoot();
                while (curScore) {
                    const DiemThi& dt = curScore->diemthi;
                    std::string mamh = StorageValidator::trim(dt.MAMH);
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

    // 3. Audit Active Exam Sessions -> Student & Subject
    for (int i = 0; i < sessions.size(); i++) {
        const auto& sess = sessions[i];
        if (!sess.in_progress) continue;

        std::string masv = StorageValidator::trim(sess.MASV);
        std::string mamh = StorageValidator::trim(sess.MAMH);

        bool studentFound = false;
        if (rootLop) {
            for (int k = 0; k < rootLop->n && !studentFound; k++) {
                if (rootLop->dslop[k] && rootLop->dslop[k]->dssinhvien.find(masv)) {
                    studentFound = true;
                }
            }
        }
        if (!studentFound) {
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

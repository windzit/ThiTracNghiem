#include "../include/Migrator.h"
#include "../include/PathResolver.h"
#include "../include/StorageManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

static std::string backupDirName;

bool Migrator::backupStorage() {
    try {
        std::string storageDir = PathResolver::getStorageDir();
        fs::path srcPath(storageDir);
        if (!fs::exists(srcPath)) return false;

        fs::path backupPath = srcPath.parent_path() / "storage_backup";
        backupDirName = backupPath.string();

        if (fs::exists(backupPath)) {
            fs::remove_all(backupPath);
        }
        fs::copy(srcPath, backupPath, fs::copy_options::recursive);
        std::cout << "[Migrator] Backup created successfully at: " << backupDirName << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Migrator] Backup failed: " << e.what() << "\n";
        return false;
    }
}

bool Migrator::rollbackMigration() {
    try {
        if (backupDirName.empty()) return false;
        fs::path backupPath(backupDirName);
        if (!fs::exists(backupPath)) return false;

        std::string storageDir = PathResolver::getStorageDir();
        fs::path srcPath(storageDir);

        if (fs::exists(srcPath)) {
            fs::remove_all(srcPath);
        }
        fs::copy(backupPath, srcPath, fs::copy_options::recursive);
        std::cout << "[Migrator] Rollback executed successfully from backup.\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Migrator] Rollback failed: " << e.what() << "\n";
        return false;
    }
}

bool Migrator::runMigration(Class& dsl, Subject& dsmh) {
    std::cout << "[Migrator] Starting Data Migration Phase 2...\n";

    // 1. Create Backup
    if (!backupStorage()) {
        std::cerr << "[Migrator] Error: Migration aborted due to backup failure.\n";
        return false;
    }

    // 2. Load legacy data into RAM
    dsl.load();
    dsmh.load();

    // Load Last Question ID from ID_Question.txt
    int lastQuestionId = 0;
    std::string idFilePath = PathResolver::getFilePath("ID_Question.txt");
    std::ifstream idFile(idFilePath);
    if (idFile.is_open()) {
        idFile >> lastQuestionId;
        idFile.close();
    }

    // 3. Prepare new flat files content
    std::string storageDir = PathResolver::getStorageDir();

    // 3.1 metadata.txt
    std::ostringstream metaSS;
    metaSS << "SCHEMA_VERSION=2.0\n";
    metaSS << "LAST_QUESTION_ID=" << lastQuestionId << "\n";
    StorageManager::atomicWriteFile(PathResolver::getFilePath("metadata.txt"), metaSS.str());

    // 3.2 classes.txt & students.txt & scores.txt
    std::ostringstream classesSS;
    std::ostringstream studentsSS;
    std::ostringstream scoresSS;

    classesSS << "MALOP|TENLOP\n";
    studentsSS << "MALOP|MASV|HO|TEN|PHAI|PASSWORD\n";
    scoresSS << "MASV|MAMH|DIEM\n";

    dsLop* rootLop = dsl.getRoot();
    if (rootLop) {
        for (int i = 0; i < rootLop->n; i++) {
            Lop* lop = rootLop->dslop[i];
            if (!lop) continue;

            classesSS << lop->MALOP << '|' << lop->TENLOP << '\n';

            dsSinhVien* curSV = lop->dssinhvien.getRoot();
            while (curSV) {
                const SinhVien& sv = curSV->sinhvien;
                studentsSS << lop->MALOP << '|'
                           << sv.MASV << '|'
                           << sv.HO << '|'
                           << sv.TEN << '|'
                           << sv.PHAI << '|'
                           << sv.passsword << '\n';

                dsDiemThi* curScore = sv.dsdiemthi.getRoot();
                while (curScore) {
                    const DiemThi& dt = curScore->diemthi;
                    scoresSS << sv.MASV << '|'
                             << dt.MAMH << '|'
                             << dt.DIEM << '\n';
                    curScore = curScore->next;
                }
                curSV = curSV->next;
            }
        }
    }

    StorageManager::atomicWriteFile(PathResolver::getFilePath("classes.txt"), classesSS.str());
    StorageManager::atomicWriteFile(PathResolver::getFilePath("students.txt"), studentsSS.str());
    StorageManager::atomicWriteFile(PathResolver::getFilePath("scores.txt"), scoresSS.str());

    // 3.3 subjects.txt & questions.txt
    std::ostringstream subjectsSS;
    std::ostringstream questionsSS;

    subjectsSS << "MAMH|TENMH|USED\n";
    questionsSS << "MAMH|ID|NOIDUNG|A|B|C|D|DAPAN_DUNG|USED\n";

    std::vector<NodeMH*> subjectNodes;
    DArray<NodeMH*> nodesDArray;
    // Helper lambda to collect subjects
    auto collectSubjects = [&](auto& self, NodeMH* node) -> void {
        if (!node) return;
        self(self, node->left);
        subjectNodes.push_back(node);
        self(self, node->right);
    };
    collectSubjects(collectSubjects, dsmh.getRoot());

    for (NodeMH* node : subjectNodes) {
        const MonHoc& mh = node->data;
        subjectsSS << mh.MAMH << '|' << mh.TENMH << '|' << (mh.used ? 1 : 0) << '\n';

        for (int id = 0; id <= 100000; id++) {
            dsCHT* qNode = const_cast<MonHoc&>(mh).dsCauHoi.find(id);
            if (qNode) {
                const CauHoi& q = qNode->cauhoi;
                questionsSS << mh.MAMH << '|'
                            << q.ID << '|'
                            << q.NOIDUNG << '|'
                            << q.A << '|'
                            << q.B << '|'
                            << q.C << '|'
                            << q.D << '|'
                            << q.DAPAN_DUNG << '|'
                            << (q.used ? 1 : 0) << '\n';
            }
        }
    }

    StorageManager::atomicWriteFile(PathResolver::getFilePath("subjects.txt"), subjectsSS.str());
    StorageManager::atomicWriteFile(PathResolver::getFilePath("questions.txt"), questionsSS.str());

    // 3.4 exam_sessions.txt
    std::ostringstream examSessionsSS;
    examSessionsSS << "MASV|MAMH|THOIGIAN_BATDAU|TONGPHUT|IN_PROGRESS|LAST_ACT|QUESTION_IDS|ANSWERS\n";

    fs::path examSessFolder = fs::path(storageDir) / "ExamSessions";
    if (fs::exists(examSessFolder) && fs::is_directory(examSessFolder)) {
        for (const auto& entry : fs::directory_iterator(examSessFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string masv = entry.path().stem().string();
                ExamSession session;
                if (loadExamSession(masv, session) && session.in_progress) {
                    examSessionsSS << session.MASV << '|'
                                   << session.MAMH << '|'
                                   << session.thoiGianBatDau << '|'
                                   << session.tongThoiGianPhut << '|'
                                   << (session.in_progress ? 1 : 0) << '|'
                                   << session.lastServerActivityAt << '|';

                    for (size_t i = 0; i < session.questionIds.size(); i++) {
                        examSessionsSS << session.questionIds[i];
                        if (i + 1 < session.questionIds.size()) examSessionsSS << ',';
                    }
                    examSessionsSS << '|';

                    for (size_t i = 0; i < session.answers.size(); i++) {
                        char ans = session.answers[i];
                        if (ans == '\0' || ans == ' ') ans = '-';
                        examSessionsSS << ans;
                        if (i + 1 < session.answers.size()) examSessionsSS << ',';
                    }
                    examSessionsSS << '\n';
                }
            }
        }
    }

    StorageManager::atomicWriteFile(PathResolver::getFilePath("exam_sessions.txt"), examSessionsSS.str());

    // 3.5 exam_history.txt & transaction.log
    std::ostringstream examHistorySS;
    examHistorySS << "MASV|MAMH|THOIGIAN_BATDAU|DIEM|QUESTION_IDS|ANSWERS\n";
    StorageManager::atomicWriteFile(PathResolver::getFilePath("exam_history.txt"), examHistorySS.str());

    StorageManager::atomicWriteFile(PathResolver::getFilePath("transaction.log"), "");

    // 4. Verify Migration
    if (!verifyMigration(dsl, dsmh)) {
        std::cerr << "[Migrator] Error: Verification failed! Rolling back migration.\n";
        rollbackMigration();
        return false;
    }

    std::cout << "[Migrator] Data Migration Phase 2 completed successfully and verified!\n";
    return true;
}

bool Migrator::verifyMigration(Class& dsl, Subject& dsmh) {
    std::cout << "[Migrator] Verifying migrated data integrity...\n";

    // 1. Calculate expected counts from RAM
    int expectedStudents = 0;
    int expectedScores = 0;
    dsLop* rootLop = dsl.getRoot();
    if (rootLop) {
        for (int i = 0; i < rootLop->n; i++) {
            Lop* lop = rootLop->dslop[i];
            if (!lop) continue;
            dsSinhVien* curSV = lop->dssinhvien.getRoot();
            while (curSV) {
                expectedStudents++;
                curSV->sinhvien.dsdiemthi.load(curSV->sinhvien.MASV);
                expectedScores += curSV->sinhvien.dsdiemthi.count();
                curSV = curSV->next;
            }
        }
    }

    int expectedQuestions = 0;
    auto countQuestions = [&](auto& self, NodeMH* node) -> void {
        if (!node) return;
        self(self, node->left);
        expectedQuestions += node->data.dsCauHoi.size();
        self(self, node->right);
    };
    countQuestions(countQuestions, dsmh.getRoot());

    // 2. Count actual lines in generated flat files
    auto countLinesInFile = [](const std::string& filename) -> int {
        std::ifstream file(PathResolver::getFilePath(filename));
        if (!file.is_open()) return -1;
        std::string line;
        int lines = 0;
        bool isHeader = true;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            if (isHeader) { isHeader = false; continue; } // Skip header line
            lines++;
        }
        return lines;
    };

    int actualStudents = countLinesInFile("students.txt");
    int actualQuestions = countLinesInFile("questions.txt");
    int actualScores = countLinesInFile("scores.txt");

    std::cout << "  [*] Students  : Expected = " << expectedStudents << ", Actual = " << actualStudents << "\n";
    std::cout << "  [*] Questions : Expected = " << expectedQuestions << ", Actual = " << actualQuestions << "\n";
    std::cout << "  [*] Scores    : Expected = " << expectedScores << ", Actual = " << actualScores << "\n";

    if (actualStudents != expectedStudents || actualQuestions != expectedQuestions || actualScores != expectedScores) {
        std::cerr << "[Migrator] Mismatch detected during verification!\n";
        return false;
    }

    std::cout << "  [OK] All record counts match 100%!\n";
    return true;
}

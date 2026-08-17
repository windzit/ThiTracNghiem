#include "../include/StorageManager.h"
#include "../include/IndexManager.h"
#include "../include/CommonTypes.h"
#include "../include/StorageValidator.h"
#include "../include/StorageVerifier.h"

#include "../include/StorageIntegrityChecker.h"
#include "../include/StringNormalizer.h"
#include "../include/Utils.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <functional>



namespace fs = std::filesystem;

static int cachedLastQuestionId = 0;
static DArray<ExamSession> cachedExamSessions;

static int s_deletedStudentCount  = 0;
static int s_deletedQuestionCount = 0;
static int s_deletedSubjectCount  = 0;
static int s_deletedClassCount    = 0;

StorageManager& StorageManager::getInstance() {
    static StorageManager instance;
    return instance;
}

bool StorageManager::initializeStorage() {
    std::string dir = PathResolver::getStorageDir();
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }
    PathResolver::getDataDir();
    PathResolver::getIndexDir();
    PathResolver::getBackupDir();

    // 1. Startup Compaction Check (runs compaction if threshold met BEFORE loading runtime indexes)
    checkAndExecuteStartupCompaction();

    // 2. Audit and load runtime indexes with startup terminal report
    IndexManager::getInstance().auditAndLoadIndexes();
    return true;
}


bool StorageManager::atomicWriteFile(const std::string& targetPath, const std::string& content) {
    std::string tempPath = targetPath + ".tmp";
    std::ofstream out(tempPath, std::ios::out | std::ios::binary);
    if (!out.is_open()) return false;
    
    out << content;
    out.flush();
    out.close();

    std::error_code ec;
    fs::rename(tempPath, targetPath, ec);
    if (ec) {
        std::error_code ec2;
        fs::copy_file(tempPath, targetPath, fs::copy_options::overwrite_existing, ec2);
        fs::remove(tempPath, ec2);
        if (ec2) return false;
    }
    return true;
}


void StorageManager::markDirty() {
    dirty = true;
}

bool StorageManager::isDirty() const {
    return dirty;
}

void StorageManager::incrementOpCount() {
    opCount++;
}

bool StorageManager::checkAndSaveAuto(Class& dsl, Subject& dsmh) {
    if (opCount >= SAVE_THRESHOLD) {
        return saveAllData(dsl, dsmh);
    }
    return true;
}

int StorageManager::getNextQuestionID() {
    cachedLastQuestionId++;
    markDirty();
    saveMetadata();
    return cachedLastQuestionId;
}

bool StorageManager::saveMetadata() {
    std::ostringstream ss;
    ss << "SCHEMA_VERSION=2.0\n"
       << "LAST_QUESTION_ID=" << cachedLastQuestionId << "\n"
       << "DELETED_STUDENT_COUNT=" << s_deletedStudentCount << "\n"
       << "DELETED_QUESTION_COUNT=" << s_deletedQuestionCount << "\n"
       << "DELETED_SUBJECT_COUNT=" << s_deletedSubjectCount << "\n"
       << "DELETED_CLASS_COUNT=" << s_deletedClassCount << "\n";

    return atomicWriteFile(PathResolver::getFilePath("metadata.txt"), ss.str());
}

// ============================================================
// LOAD IMPLEMENTATIONS
// ============================================================

bool StorageManager::loadClasses(Class& dsl) {
    std::string filePath = PathResolver::getFilePath("classes.txt");
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string malop = trim(tokens[0]);
            std::string tenlop = trim(tokens[1]);
            std::string status = tokens.size() >= 3 ? trim(tokens[2]) : "0";
            if (status == "1") {
                s_deletedClassCount++;
                continue; // Skip deleted class record
            }

            if (!malop.empty() && !dsl.find(malop)) {
                Lop* lop = new Lop();
                lop->MALOP = malop;
                lop->TENLOP = tenlop;
                StringNormalizer::normalizeClass(*lop);

                std::string errReason;
                if (!StorageValidator::validateClass(*lop, errReason)) {
                    std::cerr << "[StorageManager] Warning: Skipped invalid class on load: " << errReason << std::endl;
                    delete lop;
                    continue;
                }

                dsLop* root = dsl.getRoot();
                if (root && root->n < MAXLOP) {
                    root->dslop[root->n++] = lop;
                } else {
                    delete lop;
                }
            }
        }
    }
    file.close();
    return true;
}

bool StorageManager::loadStudents(Class& dsl) {
    std::string filePath = PathResolver::getFilePath("students.txt");
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 5) {
            std::string malop = trim(tokens[0]);
            std::string masv = trim(tokens[1]);
            std::string ho = trim(tokens[2]);
            std::string ten = trim(tokens[3]);
            std::string phai = trim(tokens[4]);
            std::string pass = tokens.size() >= 6 ? trim(tokens[5]) : "";
            std::string status = tokens.size() >= 7 ? trim(tokens[6]) : "0";
            if (status == "1") {
                s_deletedStudentCount++;
                continue; // Skip deleted student record
            }

            if (!malop.empty() && !masv.empty()) {
                Lop* lop = dsl.find(malop);
                if (lop) {
                    SinhVien sv;
                    sv.MASV = masv;
                    sv.HO = ho;
                    sv.TEN = ten;
                    sv.PHAI = phai;
                    sv.passsword = pass;
                    StringNormalizer::normalizeStudent(sv);

                    std::string errReason;
                    if (!StorageValidator::validateStudent(sv, malop, errReason)) {
                        std::cerr << "[StorageManager] Warning: Skipped invalid student on load: " << errReason << std::endl;
                        continue;
                    }

                    lop->dssinhvien.insert(sv);
                }
            }
        }
    }
    file.close();
    return true;
}

bool StorageManager::loadSubjects(Subject& dsmh) {
    std::string filePath = PathResolver::getFilePath("subjects.txt");
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string mamh = trim(tokens[0]);
            std::string tenmh = trim(tokens[1]);
            bool used = tokens.size() >= 3 ? (trim(tokens[2]) == "1" || trim(tokens[2]) == "true") : false;
            std::string status = tokens.size() >= 4 ? trim(tokens[3]) : "0";
            if (status == "1") {
                s_deletedSubjectCount++;
                continue; // Skip deleted subject record
            }

            if (!mamh.empty() && !dsmh.find(mamh.c_str())) {
                MonHoc mh;
                std::strcpy(mh.MAMH, mamh.c_str());
                mh.TENMH = tenmh;
                mh.used = used;
                StringNormalizer::normalizeSubject(mh);

                std::string errReason;
                if (!StorageValidator::validateSubject(mh, errReason)) {
                    std::cerr << "[StorageManager] Warning: Skipped invalid subject on load: " << errReason << std::endl;
                    continue;
                }

                dsmh.insert(mh);
            }
        }
    }
    file.close();
    return true;
}

bool StorageManager::loadQuestions(Subject& dsmh) {
    std::string filePath = PathResolver::getFilePath("questions.txt");
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 8) {
            std::string mamh = trim(tokens[0]);
            int id = std::stoi(trim(tokens[1]));
            if (id > cachedLastQuestionId) {
                cachedLastQuestionId = id;
            }
            std::string noidung = tokens[2];
            std::string a = tokens[3];
            std::string b = tokens[4];
            std::string c = tokens[5];
            std::string d = tokens[6];
            std::string dapan = trim(tokens[7]);
            bool used = false;
            std::string status = tokens.size() >= 9 ? trim(tokens[8]) : "0";
            if (status == "1") {
                s_deletedQuestionCount++;
                continue; // Skip hard deleted question record
            }
            if (status == "2") {
                s_deletedQuestionCount++;
            }
            bool deleted = (status == "2");

            NodeMH* node = dsmh.find(mamh.c_str());
            if (node) {
                CauHoi q;
                q.ID = id;
                q.NOIDUNG = noidung;
                q.A = a; q.B = b; q.C = c; q.D = d;
                q.DAPAN_DUNG = dapan.empty() ? '\0' : dapan[0];
                q.used = used;
                q.deleted = deleted;
                StringNormalizer::normalizeQuestion(q);

                std::string errReason;
                if (!StorageValidator::validateQuestion(q, mamh, errReason)) {
                    std::cerr << "[StorageManager] Warning: Skipped invalid question on load: " << errReason << std::endl;
                    continue;
                }

                node->data.dsCauHoi.insert(q, false);
            }
        }
    }
    file.close();
    return true;
}

bool StorageManager::loadScores(Class& dsl) {
    std::string filePath = PathResolver::getFilePath("scores.txt");
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 3) {
            std::string masv = trim(tokens[0]);
            std::string mamh = trim(tokens[1]);
            float diem = std::stof(trim(tokens[2]));

            dsLop* rootLop = dsl.getRoot();
            if (rootLop) {
                for (int i = 0; i < rootLop->n; i++) {
                    Lop* lop = rootLop->dslop[i];
                    if (!lop) continue;
                    SinhVien* sv = lop->dssinhvien.find(masv);
                    if (sv) {
                        DiemThi dt;
                        std::strcpy(dt.MAMH, mamh.c_str());
                        dt.DIEM = diem;
                        sv->dsdiemthi.insert(dt);
                        break;
                    }
                }
            }
        }
    }
    file.close();
    return true;
}

bool StorageManager::loadExamSessions(DArray<ExamSession>& sessions) {
    sessions.clear();
    std::string filePath = PathResolver::getFilePath("exam_sessions.txt");
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) { isHeader = false; continue; }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 8) {
            ExamSession s;
            s.MASV = trim(tokens[0]);
            s.MAMH = trim(tokens[1]);
            s.thoiGianBatDau = std::stoll(trim(tokens[2]));
            s.tongThoiGianPhut = std::stoi(trim(tokens[3]));
            s.in_progress = (trim(tokens[4]) == "1" || trim(tokens[4]) == "true");
            s.lastServerActivityAt = std::stoll(trim(tokens[5]));

            DArray<std::string> qIdStrs = split(tokens[6], ',');
            for (int qIdx = 0; qIdx < qIdStrs.size(); qIdx++) {
                std::string qid = qIdStrs[qIdx];
                if (!trim(qid).empty()) s.questionIds.push_back(std::stoi(trim(qid)));
            }

            DArray<std::string> ansStrs = split(tokens[7], ',');
            for (int aIdx = 0; aIdx < ansStrs.size(); aIdx++) {
                std::string ans = ansStrs[aIdx];
                if (!trim(ans).empty()) {
                    char ch = trim(ans)[0];
                    if (ch == '-') ch = '\0';
                    s.answers.push_back(ch);
                }
            }

            if (s.in_progress) {
                sessions.push_back(s);
            }
        }
    }
    file.close();
    cachedExamSessions = sessions;
    return true;
}

int StorageManager::getActiveSessionCount() const {
    return (int)cachedExamSessions.size();
}

bool StorageManager::loadSystemSettings(bool& outFullscreenRequired) {
    outFullscreenRequired = false;
    std::string path = PathResolver::getFilePath("SystemSettings.txt");
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.find("fullscreen=") == 0) {
            std::string val = trimmed.substr(11);
            outFullscreenRequired = (val == "true" || val == "1");
        }
    }
    file.close();
    return true;
}

bool StorageManager::saveSystemSettings(bool fullscreenRequired) {
    std::string path = PathResolver::getFilePath("SystemSettings.txt");
    std::string content = "fullscreen=" + std::string(fullscreenRequired ? "true" : "false") + "\n";

    // 1. Atomic Write (DO NOT update RAM yet)
    if (!atomicWriteFile(path, content)) return false;

    // 2. Read back from disk
    bool diskValue = false;
    if (!loadSystemSettings(diskValue)) return false;

    // 3. Compare — if mismatch, keep RAM unchanged, return false
    if (diskValue != fullscreenRequired) {
        std::cerr << "[StorageManager] SystemSettings verification FAILED: expected="
                  << fullscreenRequired << " disk=" << diskValue << std::endl;
        return false;
    }

    // 4. Success — caller updates RAM
    return true;
}

bool StorageManager::loadAllData(Class& dsl, Subject& dsmh) {
    std::cout << "[STARTUP LOG] [BEGIN] loadAllData\n";

    std::string metaPath = PathResolver::getFilePath("metadata.txt");
    std::cout << "[STARTUP LOG] [BEGIN] loadMetadata from: " << metaPath << "\n";
    std::ifstream metaFile(metaPath);
    if (metaFile.is_open()) {
        std::string line;
        while (std::getline(metaFile, line)) {
            if (line.find("LAST_QUESTION_ID=") == 0) {
                try { cachedLastQuestionId = std::stoi(line.substr(17)); } catch (...) {}
            }
        }
        metaFile.close();
    }
    std::cout << "[STARTUP LOG] [END] loadMetadata (LAST_QUESTION_ID: " << cachedLastQuestionId << ")\n";

    std::cout << "[STARTUP LOG] [BEGIN] loadClasses\n";
    loadClasses(dsl);
    std::cout << "[STARTUP LOG] [END] loadClasses\n";

    std::cout << "[STARTUP LOG] [BEGIN] loadStudents\n";
    loadStudents(dsl);
    std::cout << "[STARTUP LOG] [END] loadStudents\n";

    std::cout << "[STARTUP LOG] [BEGIN] loadSubjects\n";
    loadSubjects(dsmh);
    std::cout << "[STARTUP LOG] [END] loadSubjects\n";

    std::cout << "[STARTUP LOG] [BEGIN] loadQuestions\n";
    loadQuestions(dsmh);
    std::cout << "[STARTUP LOG] [END] loadQuestions\n";

    std::cout << "[STARTUP LOG] [BEGIN] loadScores\n";
    loadScores(dsl);
    std::cout << "[STARTUP LOG] [END] loadScores\n";
    
    std::cout << "[STARTUP LOG] [BEGIN] loadExamSessions\n";
    loadExamSessions(cachedExamSessions);
    std::cout << "[STARTUP LOG] [END] loadExamSessions\n";

    std::cout << "[STARTUP LOG] [BEGIN] Rebuild derived 'used' flags\n";
    rebuildUsedFlags(dsmh);
    std::cout << "[STARTUP LOG] [END] Rebuild derived 'used' flags\n";

    std::cout << "[STARTUP LOG] [BEGIN] StorageIntegrityChecker::auditStorageIntegrity\n";
    StorageIntegrityChecker::auditStorageIntegrity(dsl, dsmh, cachedExamSessions);
    std::cout << "[STARTUP LOG] [END] StorageIntegrityChecker::auditStorageIntegrity\n";

    std::cout << "[STARTUP LOG] [BEGIN] rebuildIndexes\n";
    rebuildIndexes();
    std::cout << "[STARTUP LOG] [END] rebuildIndexes\n";

    saveMetadata();
    std::cout << "[STARTUP LOG] [END] loadAllData\n";
    return true;
}

// ============================================================
// SAVE IMPLEMENTATIONS (VALIDATED & VERIFIED ATOMIC WRITE)
// ============================================================

bool StorageManager::saveClasses(Class& dsl) {
    std::ostringstream ss;
    ss << "MALOP          |TENLOP                                            |S\n";
    int validCount = 0;
    dsLop* root = dsl.getRoot();
    if (root) {
        for (int i = 0; i < root->n; i++) {
            if (root->dslop[i]) {
                std::string errReason;
                if (!StorageValidator::validateClass(*root->dslop[i], errReason)) {
                    std::cerr << "[StorageValidation] Class validation failed: " << errReason << std::endl;
                    return false;
                }
                ss << std::left << std::setw(15) << root->dslop[i]->MALOP << '|'
                   << std::setw(50) << root->dslop[i]->TENLOP << '|'
                   << STATUS_ACTIVE << '\n';
                validCount++;
            }
        }
    }

    std::string targetPath = PathResolver::getFilePath("classes.txt");
    if (!atomicWriteFile(targetPath, ss.str())) return false;

    std::string verifyErr;
    if (!StorageVerifier::verifyClasses(dsl, targetPath, verifyErr)) {
        std::cerr << "[StorageVerification] Deep verification failed: " << verifyErr << std::endl;
        return false;
    }
    IndexManager::getInstance().rebuildClassIndex();
    IndexManager::getInstance().saveClassIndex();
    return true;
}

bool StorageManager::saveStudents(Class& dsl) {
    std::ostringstream ss;
    ss << "MALOP          |MASV      |HO                                                |TEN            |PHAI|PASSWORD                        |S\n";
    int validCount = 0;
    dsLop* root = dsl.getRoot();
    if (root) {
        for (int i = 0; i < root->n; i++) {
            Lop* lop = root->dslop[i];
            if (!lop) continue;
            dsSinhVien* cur = lop->dssinhvien.getRoot();
            while (cur) {
                const SinhVien& sv = cur->sinhvien;
                std::string errReason;
                if (!StorageValidator::validateStudent(sv, lop->MALOP, errReason)) {
                    std::cerr << "[StorageValidation] Student validation failed: " << errReason << std::endl;
                    return false;
                }
                ss << std::left << std::setw(15) << lop->MALOP << '|'
                   << std::setw(10) << sv.MASV << '|'
                   << std::setw(50) << sv.HO << '|'
                   << std::setw(15) << sv.TEN << '|'
                   << std::setw(4)  << sv.PHAI << '|'
                   << std::setw(32) << sv.passsword << '|'
                   << STATUS_ACTIVE << '\n';
                validCount++;
                cur = cur->next;
            }
        }
    }
    std::string targetPath = PathResolver::getFilePath("students.txt");
    if (!atomicWriteFile(targetPath, ss.str())) return false;

    std::string verifyErr;
    if (!StorageVerifier::verifyStudents(dsl, targetPath, verifyErr)) {
        std::cerr << "[StorageVerification] Deep verification failed: " << verifyErr << std::endl;
        return false;
    }
    IndexManager::getInstance().rebuildStudentIndex();
    IndexManager::getInstance().saveStudentIndex();
    return true;
}

static bool validateAndCollectSubjects(NodeMH* node, std::ostringstream& subjSS, int& subjectCount, std::string& errReason) {
    if (!node) return true;
    if (!validateAndCollectSubjects(node->left, subjSS, subjectCount, errReason)) return false;

    const MonHoc& mh = node->data;
    if (!StorageValidator::validateSubject(mh, errReason)) return false;
    subjSS << std::left << std::setw(15) << mh.MAMH << '|'
           << std::setw(50) << mh.TENMH << '|'
           << (mh.used ? '1' : '0') << '|'
           << STATUS_ACTIVE << '\n';
    subjectCount++;

    return validateAndCollectSubjects(node->right, subjSS, subjectCount, errReason);
}

static bool validateAndCollectQuestions(NodeMH* node, std::ostringstream& questSS, int& questionCount, std::string& errReason) {
    if (!node) return true;
    if (!validateAndCollectQuestions(node->left, questSS, questionCount, errReason)) return false;

    const MonHoc& mh = node->data;
    dsCHT* qNode = const_cast<MonHoc&>(mh).dsCauHoi.getRoot();
    while (qNode) {
        const CauHoi& q = qNode->cauhoi;
        if (!StorageValidator::validateQuestion(q, mh.MAMH, errReason)) return false;
        char st = q.deleted ? STATUS_SOFT_DELETED : STATUS_ACTIVE;
        questSS << std::left  << std::setfill(' ') << std::setw(15) << mh.MAMH << '|'
                << std::right << std::setfill('0') << std::setw(10) << q.ID << '|'
                << std::left  << std::setfill(' ') << std::setw(300) << q.NOIDUNG << '|'
                << std::setw(100) << q.A << '|'
                << std::setw(100) << q.B << '|'
                << std::setw(100) << q.C << '|'
                << std::setw(100) << q.D << '|'
                << q.DAPAN_DUNG << '|'
                << st << '\n';
        questionCount++;
        qNode = qNode->next;
    }

    return validateAndCollectQuestions(node->right, questSS, questionCount, errReason);
}

bool StorageManager::saveSubjects(Subject& dsmh) {
    std::ostringstream subjSS;
    subjSS << "MAMH           |TENMH                                             |U|S\n";
    int subjectCount = 0;
    std::string errReason;

    if (!validateAndCollectSubjects(dsmh.getRoot(), subjSS, subjectCount, errReason)) {
        std::cerr << "[StorageValidation] Subject validation failed: " << errReason << std::endl;
        return false;
    }

    std::string targetPath = PathResolver::getFilePath("subjects.txt");
    if (!atomicWriteFile(targetPath, subjSS.str())) return false;

    std::string verifyErr;
    if (!StorageVerifier::verifySubjects(dsmh, targetPath, verifyErr)) {
        std::cerr << "[StorageVerification] Deep verification failed: " << verifyErr << std::endl;
        return false;
    }
    IndexManager::getInstance().rebuildSubjectIndex();
    IndexManager::getInstance().saveSubjectIndex();
    return true;
}

bool StorageManager::saveQuestions(Subject& dsmh) {
    std::ostringstream questSS;
    questSS << std::left << std::setfill(' ') << std::setw(15) << "MAMH" << '|' << std::right << std::setfill(' ') << std::setw(10) << "ID" << '|' << std::left << std::setfill(' ') << std::setw(300) << "NOIDUNG" << '|' << std::setw(100) << "A" << '|' << std::setw(100) << "B" << '|' << std::setw(100) << "C" << '|' << std::setw(100) << "D" << '|' << "A" << '|' << "S" << '\n';
    int questionCount = 0;
    std::string errReason;

    if (!validateAndCollectQuestions(dsmh.getRoot(), questSS, questionCount, errReason)) {
        std::cerr << "[StorageValidation] Question validation failed: " << errReason << std::endl;
        return false;
    }

    std::string targetPath = PathResolver::getFilePath("questions.txt");
    if (!atomicWriteFile(targetPath, questSS.str())) return false;

    std::string verifyErr;
    if (!StorageVerifier::verifyQuestions(dsmh, targetPath, verifyErr)) {
        std::cerr << "[StorageVerification] Deep verification failed: " << verifyErr << std::endl;
        return false;
    }
    IndexManager::getInstance().rebuildQuestionIndex();
    IndexManager::getInstance().saveQuestionIndex();
    return true;
}

bool StorageManager::saveScores(Class& dsl) {
    std::ostringstream ss;
    ss << "MASV|MAMH|DIEM\n";
    int scoreCount = 0;
    dsLop* root = dsl.getRoot();
    if (root) {
        for (int i = 0; i < root->n; i++) {
            Lop* lop = root->dslop[i];
            if (!lop) continue;
            dsSinhVien* curSV = lop->dssinhvien.getRoot();
            while (curSV) {
                const SinhVien& sv = curSV->sinhvien;
                dsDiemThi* curScore = const_cast<SinhVien&>(sv).dsdiemthi.getRoot();
                while (curScore) {
                    const DiemThi& dt = curScore->diemthi;
                    std::string errReason;
                    if (!StorageValidator::validateScore(sv.MASV, dt, errReason)) {
                        std::cerr << "[StorageValidation] Score validation failed: " << errReason << std::endl;
                        return false;
                    }
                    ss << sv.MASV << '|'
                       << dt.MAMH << '|'
                       << dt.DIEM << '\n';
                    scoreCount++;
                    curScore = curScore->next;
                }
                curSV = curSV->next;
            }
        }
    }
    std::string targetPath = PathResolver::getFilePath("scores.txt");
    if (!atomicWriteFile(targetPath, ss.str())) return false;

    std::string verifyErr;
    if (!StorageVerifier::verifyScores(dsl, targetPath, verifyErr)) {
        std::cerr << "[StorageVerification] Deep verification failed: " << verifyErr << std::endl;
        return false;
    }
    return true;
}

static bool flushExamSessionsFile() {
    std::ostringstream ss;
    ss << "MASV|MAMH|THOIGIAN_BATDAU|TONGPHUT|IN_PROGRESS|LAST_ACT|QUESTION_IDS|ANSWERS\n";
    int activeCount = 0;
    for (const auto& s : cachedExamSessions) {
        if (!s.in_progress) continue;
        ss << s.MASV << '|'
           << s.MAMH << '|'
           << s.thoiGianBatDau << '|'
           << s.tongThoiGianPhut << '|'
           << (s.in_progress ? 1 : 0) << '|'
           << s.lastServerActivityAt << '|';

        for (size_t i = 0; i < s.questionIds.size(); i++) {
            ss << s.questionIds[i];
            if (i + 1 < s.questionIds.size()) ss << ',';
        }
        ss << '|';

        for (size_t i = 0; i < s.answers.size(); i++) {
            char ans = s.answers[i];
            if (ans == '\0' || ans == ' ') ans = '-';
            ss << ans;
            if (i + 1 < s.answers.size()) ss << ',';
        }
        ss << '\n';
        activeCount++;
    }
    std::string targetPath = PathResolver::getFilePath("exam_sessions.txt");
    if (!StorageManager::atomicWriteFile(targetPath, ss.str())) return false;

    std::string verifyErr;
    if (!StorageVerifier::verifyExamSessions(cachedExamSessions, targetPath, verifyErr)) {
        std::cerr << "[StorageVerification] Deep verification failed: " << verifyErr << std::endl;
        return false;
    }
    return true;
}

bool StorageManager::saveExamSession(const ExamSession& session) {
    std::string errReason;
    if (!StorageValidator::validateExamSession(session, errReason)) {
        std::cerr << "[StorageValidation] ExamSession validation failed: " << errReason << std::endl;
        return false;
    }
    bool found = false;
    for (auto& s : cachedExamSessions) {
        if (s.MASV == session.MASV) {
            s = session;
            found = true;
            break;
        }
    }
    if (!found) {
        cachedExamSessions.push_back(session);
    }
    return flushExamSessionsFile();
}

bool StorageManager::removeExamSession(const std::string& masv) {
    for (int i = cachedExamSessions.size() - 1; i >= 0; i--) {
        if (cachedExamSessions[i].MASV == masv) {
            cachedExamSessions.remove(i);
        }
    }
    return flushExamSessionsFile();
}

bool StorageManager::appendExamHistory(const ExamSession& session, float diem) {
    std::string errReason;
    if (!StorageValidator::validateExamSession(session, errReason)) {
        std::cerr << "[StorageValidation] appendExamHistory validation failed (session): " << errReason << std::endl;
        return false;
    }
    if (diem < 0.0f || diem > 10.0f) {
        std::cerr << "[StorageValidation] appendExamHistory validation failed (score range): " << diem << std::endl;
        return false;
    }

    std::string filePath = PathResolver::getFilePath("exam_history.txt");
    std::ifstream in(filePath);
    std::ostringstream ss;
    bool hasHeader = false;
    int existingLines = 0;
    if (in.is_open()) {
        std::string line;
        if (std::getline(in, line)) {
            ss << line << "\n";
            hasHeader = true;
            existingLines++;
        }
        while (std::getline(in, line)) {
            if (!StorageValidator::isEmptyOrWhitespace(line)) {
                ss << line << "\n";
                existingLines++;
            }
        }
        in.close();
    }

    if (!hasHeader) {
        ss << "MASV|MAMH|THOIGIAN_BATDAU|DIEM|QUESTION_IDS|ANSWERS\n";
        existingLines = 1;
    }

    ss << session.MASV << '|'
       << session.MAMH << '|'
       << session.thoiGianBatDau << '|'
       << diem << '|';

    for (size_t i = 0; i < session.questionIds.size(); i++) {
        ss << session.questionIds[i];
        if (i + 1 < session.questionIds.size()) ss << ',';
    }
    ss << '|';

    for (size_t i = 0; i < session.answers.size(); i++) {
        char ans = session.answers[i];
        if (ans == '\0' || ans == ' ') ans = '-';
        ss << ans;
        if (i + 1 < session.answers.size()) ss << ',';
    }
    ss << '\n';

    if (!atomicWriteFile(filePath, ss.str())) return false;

    std::string verifyErr;
    if (!StorageVerifier::verifyFileRowCount(filePath, 1, existingLines, verifyErr)) {
        std::cerr << "[StorageVerification] " << verifyErr << std::endl;
        return false;
    }
    IndexManager::getInstance().rebuildHistoryIndex();
    IndexManager::getInstance().saveHistoryIndex();
    return true;
}

bool StorageManager::resetToDefault() {
    namespace fs = std::filesystem;

    // 1. Auto-backup: storage/ → storage_backup_reset_<timestamp>/
    std::string storageDir = PathResolver::getStorageDir();
    if (!fs::exists(storageDir)) {
        std::cerr << "[ResetStorage] Error: storage directory does not exist.\n";
        return false;
    }

    // Generate timestamp YYYYMMDD_HHMMSS
    std::time_t now = std::time(nullptr);
    std::tm* tmNow = std::localtime(&now);
    char tsBuf[32];
    std::strftime(tsBuf, sizeof(tsBuf), "%Y%m%d_%H%M%S", tmNow);
    std::string timestamp(tsBuf);

    fs::path backupPath = fs::path(storageDir).parent_path() / ("storage_backup_reset_" + timestamp);
    try {
        fs::copy(storageDir, backupPath, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        std::cout << "[ResetStorage] Backup created at: " << backupPath.string() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[ResetStorage] Backup failed: " << e.what() << "\n";
        return false;
    }

    // 2. Write 9 default header-only files via atomicWriteFile
    std::ostringstream cHdr, stHdr, subHdr, qHdr;
    cHdr << std::left << std::setw(15) << "MALOP" << '|' << std::setw(50) << "TENLOP" << '|' << "S" << '\n';
    stHdr << std::left << std::setw(15) << "MALOP" << '|' << std::setw(10) << "MASV" << '|' << std::setw(50) << "HO" << '|' << std::setw(15) << "TEN" << '|' << std::setw(4) << "PHAI" << '|' << std::setw(32) << "PASSWORD" << '|' << "S" << '\n';
    subHdr << std::left << std::setw(15) << "MAMH" << '|' << std::setw(50) << "TENMH" << '|' << "U" << '|' << "S" << '\n';
    qHdr << std::left << std::setfill(' ') << std::setw(15) << "MAMH" << '|' << std::right << std::setfill(' ') << std::setw(10) << "ID" << '|' << std::left << std::setfill(' ') << std::setw(300) << "NOIDUNG" << '|' << std::setw(100) << "A" << '|' << std::setw(100) << "B" << '|' << std::setw(100) << "C" << '|' << std::setw(100) << "D" << '|' << "A" << '|' << "S" << '\n';

    atomicWriteFile(PathResolver::getFilePath("metadata.txt"), "SCHEMA_VERSION=2.0\nLAST_QUESTION_ID=0\n");
    atomicWriteFile(PathResolver::getFilePath("classes.txt"), cHdr.str());
    atomicWriteFile(PathResolver::getFilePath("students.txt"), stHdr.str());
    atomicWriteFile(PathResolver::getFilePath("subjects.txt"), subHdr.str());
    atomicWriteFile(PathResolver::getFilePath("questions.txt"), qHdr.str());
    atomicWriteFile(PathResolver::getFilePath("scores.txt"), "MASV|MAMH|DIEM\n");
    atomicWriteFile(PathResolver::getFilePath("exam_history.txt"), "MASV|MAMH|THOIGIAN_BATDAU|DIEM|QUESTION_IDS|ANSWERS\n");
    atomicWriteFile(PathResolver::getFilePath("exam_sessions.txt"), "MASV|MAMH|THOIGIAN_BATDAU|TONGPHUT|IN_PROGRESS|LAST_ACT|QUESTION_IDS|ANSWERS\n");
    atomicWriteFile(PathResolver::getFilePath("transaction.log"), "");
    atomicWriteFile(PathResolver::getFilePath("SystemSettings.txt"), "fullscreen=false\n");

    // 3. Reset internal state
    cachedLastQuestionId = 0;
    cachedExamSessions.clear();
    dirty = false;
    opCount = 0;

    IndexManager::getInstance().clear();
    IndexManager::getInstance().rebuildAllIndexes();

    std::cout << "[ResetStorage] 10 files reset to default empty state.\n";
    return true;
}

bool StorageManager::rebuildIndexes() {
    return IndexManager::getInstance().rebuildAllIndexes();
}

bool StorageManager::syncIndexes() {
    return IndexManager::getInstance().saveQuestionIndex() &&
           IndexManager::getInstance().saveStudentIndex() &&
           IndexManager::getInstance().saveHistoryIndex();
}

bool StorageManager::saveAllData(Class& dsl, Subject& dsmh) {
    // 1. Write metadata.txt
    std::ostringstream metaSS;
    metaSS << "SCHEMA_VERSION=2.0\n";
    metaSS << "LAST_QUESTION_ID=" << cachedLastQuestionId << "\n";
    atomicWriteFile(PathResolver::getFilePath("metadata.txt"), metaSS.str());

    // 2. Save classes & students
    saveClasses(dsl);
    saveStudents(dsl);

    // 3. Save subjects & questions
    saveSubjects(dsmh);
    saveQuestions(dsmh);

    // 4. Save scores
    saveScores(dsl);

    // Reset dirty state
    dirty = false;
    opCount = 0;
    return true;
}

void StorageManager::rebuildUsedFlags(Subject& dsmh) {
    // 1. Reset all question and subject used flags to false
    std::function<void(NodeMH*)> clearNode = [&](NodeMH* node) {
        if (!node) return;
        clearNode(node->left);
        node->data.used = false;
        dsCHT* qNode = node->data.dsCauHoi.getRoot();
        while (qNode) {
            qNode->cauhoi.used = false;
            qNode = qNode->next;
        }
        clearNode(node->right);
    };
    clearNode(dsmh.getRoot());

    // 2. Mark used questions from active exam sessions
    for (const auto& s : cachedExamSessions) {
        if (!s.in_progress) continue;
        NodeMH* node = dsmh.find(s.MAMH.c_str());
        if (node) {
            node->data.used = true;
            for (size_t i = 0; i < s.questionIds.size(); i++) {
                dsCHT* qNode = node->data.dsCauHoi.find(s.questionIds[i]);
                if (qNode) {
                    qNode->cauhoi.used = true;
                }
            }
        }
    }

    // 3. Mark used questions from exam history
    std::string historyPath = PathResolver::getFilePath("exam_history.txt");
    std::ifstream hFile(historyPath);
    if (hFile.is_open()) {
        std::string line;
        bool isHeader = true;
        while (std::getline(hFile, line)) {
            std::string trimmed = trim(line);
            if (trimmed.empty()) continue;
            if (isHeader) { isHeader = false; continue; }
            DArray<std::string> tokens = split(trimmed, '|');
            if (tokens.size() >= 5) {
                std::string mamh = trim(tokens[1]);
                std::string qIdsStr = trim(tokens[4]);
                NodeMH* node = dsmh.find(mamh.c_str());
                if (node) {
                    node->data.used = true;
                    DArray<std::string> qTokens = split(qIdsStr, ',');
                    for (size_t i = 0; i < qTokens.size(); i++) {
                        std::string qStr = trim(qTokens[i]);
                        if (!qStr.empty()) {
                            try {
                                int qId = std::stoi(qStr);
                                dsCHT* qNode = node->data.dsCauHoi.find(qId);
                                if (qNode) {
                                    qNode->cauhoi.used = true;
                                }
                            } catch (...) {}
                        }
                    }
                }
            }
        }
        hFile.close();
    }

    // 4. Mark subject used if any student has scores for that subject in scores.txt
    std::string scoresPath = PathResolver::getFilePath("scores.txt");
    std::ifstream scFile(scoresPath);
    if (scFile.is_open()) {
        std::string line;
        bool isHeader = true;
        while (std::getline(scFile, line)) {
            std::string trimmed = trim(line);
            if (trimmed.empty()) continue;
            if (isHeader) { isHeader = false; continue; }
            DArray<std::string> tokens = split(trimmed, '|');
            if (tokens.size() >= 2) {
                std::string mamh = trim(tokens[1]);
                NodeMH* node = dsmh.find(mamh.c_str());
                if (node) {
                    node->data.used = true;
                }
            }
        }
        scFile.close();
    }
}

// ============================================================
// FIXED-LENGTH RECORD DIRECT seekg() / seekp() OPERATIONS
// ============================================================

bool StorageManager::readStudentAt(int64_t offset, SinhVien& outSv, std::string& outMalop) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("students.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekg(offset);
    std::string line;
    if (!std::getline(file, line)) {
        file.close();
        return false;
    }
    file.close();

    DArray<std::string> tokens = split(line, '|');
    if (tokens.size() >= 7) {
        std::string status = trim(tokens[6]);
        if (status == "1") return false; // Deleted

        outMalop = trim(tokens[0]);
        outSv.MASV = trim(tokens[1]);
        outSv.HO = trim(tokens[2]);
        outSv.TEN = trim(tokens[3]);
        outSv.PHAI = trim(tokens[4]);
        outSv.passsword = trim(tokens[5]);
        return true;
    }
    return false;
}

bool StorageManager::writeStudentAt(int64_t offset, const SinhVien& sv, const std::string& malop, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("students.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    std::ostringstream ss;
    ss << std::left << std::setw(15) << malop << '|'
       << std::setw(10) << sv.MASV << '|'
       << std::setw(50) << sv.HO << '|'
       << std::setw(15) << sv.TEN << '|'
       << std::setw(4)  << sv.PHAI << '|'
       << std::setw(32) << sv.passsword << '|'
       << status << '\n';

    std::string recordStr = ss.str();
    file.seekp(offset);
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();
    return true;
}

bool StorageManager::appendStudent(const SinhVien& sv, const std::string& malop, int64_t& outOffset) {
    std::string filePath = PathResolver::getFilePath("students.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::ofstream newFile(filePath, std::ios::out | std::ios::binary);
        newFile << "MALOP          |MASV      |HO                                                |TEN            |PHAI|PASSWORD                        |S\n";
        newFile.close();
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    }
    if (!file.is_open()) return false;

    file.seekp(0, std::ios::end);
    outOffset = file.tellp();

    std::ostringstream ss;
    ss << std::left << std::setw(15) << malop << '|'
       << std::setw(10) << sv.MASV << '|'
       << std::setw(50) << sv.HO << '|'
       << std::setw(15) << sv.TEN << '|'
       << std::setw(4)  << sv.PHAI << '|'
       << std::setw(32) << sv.passsword << '|'
       << STATUS_ACTIVE << '\n';

    std::string recordStr = ss.str();
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();

    IndexManager::getInstance().updateStudentOffset(sv.MASV, outOffset);
    return true;
}

bool StorageManager::markStudentStatusAt(int64_t offset, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("students.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekp(offset + StorageConfig::STATUS_OFFSET_STUDENT);
    file.write(&status, 1);
    file.flush();
    file.close();

    if (status == STATUS_DELETED || status == STATUS_SOFT_DELETED) {
        incrementDeletedCount("student");
    }
    return true;
}

bool StorageManager::readQuestionAt(int64_t offset, CauHoi& outQ, std::string& outMamh) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("questions.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekg(offset);
    std::string line;
    if (!std::getline(file, line)) {
        file.close();
        return false;
    }
    file.close();

    DArray<std::string> tokens = split(line, '|');
    if (tokens.size() >= 9) {
        std::string status = trim(tokens[8]);
        if (status == "1") return false; // Deleted unused

        outMamh = trim(tokens[0]);
        outQ.ID = std::stoi(trim(tokens[1]));
        outQ.NOIDUNG = trim(tokens[2]);
        outQ.A = trim(tokens[3]);
        outQ.B = trim(tokens[4]);
        outQ.C = trim(tokens[5]);
        outQ.D = trim(tokens[6]);
        outQ.DAPAN_DUNG = trim(tokens[7]).empty() ? 'A' : trim(tokens[7])[0];
        outQ.deleted = (status == "2"); // Soft deleted
        return true;
    }
    return false;
}

bool StorageManager::writeQuestionAt(int64_t offset, const CauHoi& q, const std::string& mamh, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("questions.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    std::ostringstream ss;
    ss << std::left  << std::setfill(' ') << std::setw(15) << mamh << '|'
       << std::right << std::setfill('0') << std::setw(10) << q.ID << '|'
       << std::left  << std::setfill(' ') << std::setw(300) << q.NOIDUNG << '|'
       << std::setw(100) << q.A << '|'
       << std::setw(100) << q.B << '|'
       << std::setw(100) << q.C << '|'
       << std::setw(100) << q.D << '|'
       << q.DAPAN_DUNG << '|'
       << status << '\n';

    std::string recordStr = ss.str();
    file.seekp(offset);
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();
    return true;
}

bool StorageManager::appendQuestion(const CauHoi& q, const std::string& mamh, int64_t& outOffset) {
    std::string filePath = PathResolver::getFilePath("questions.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::ofstream newFile(filePath, std::ios::out | std::ios::binary);
        newFile << "MAMH           |ID        |NOIDUNG                                                                                                                                                                                                                                                                                                                        |A                                                                                                   |B                                                                                                   |C                                                                                                   |D                                                                                                   |A|S\n";
        newFile.close();
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    }
    if (!file.is_open()) return false;

    file.seekp(0, std::ios::end);
    outOffset = file.tellp();

    std::ostringstream ss;
    ss << std::left  << std::setfill(' ') << std::setw(15) << mamh << '|'
       << std::right << std::setfill('0') << std::setw(10) << q.ID << '|'
       << std::left  << std::setfill(' ') << std::setw(300) << q.NOIDUNG << '|'
       << std::setw(100) << q.A << '|'
       << std::setw(100) << q.B << '|'
       << std::setw(100) << q.C << '|'
       << std::setw(100) << q.D << '|'
       << q.DAPAN_DUNG << '|'
       << STATUS_ACTIVE << '\n';

    std::string recordStr = ss.str();
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();

    IndexManager::getInstance().updateQuestionOffset(q.ID, outOffset);
    return true;
}

bool StorageManager::markQuestionStatusAt(int64_t offset, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("questions.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekp(offset + StorageConfig::STATUS_OFFSET_QUESTION);
    file.write(&status, 1);
    file.flush();
    file.close();

    if (status == STATUS_DELETED || status == STATUS_SOFT_DELETED) {
        incrementDeletedCount("question");
    }
    return true;
}

bool StorageManager::readSubjectAt(int64_t offset, MonHoc& outMh) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("subjects.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekg(offset);
    std::string line;
    if (!std::getline(file, line)) {
        file.close();
        return false;
    }
    file.close();

    DArray<std::string> tokens = split(line, '|');
    if (tokens.size() >= 4) {
        std::string status = trim(tokens[3]);
        if (status == "1") return false;

        std::strcpy(outMh.MAMH, trim(tokens[0]).c_str());
        outMh.TENMH = trim(tokens[1]);
        outMh.used = (trim(tokens[2]) == "1");
        return true;
    }
    return false;
}

bool StorageManager::writeSubjectAt(int64_t offset, const MonHoc& mh, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("subjects.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    std::ostringstream ss;
    ss << std::left << std::setw(15) << mh.MAMH << '|'
       << std::setw(50) << mh.TENMH << '|'
       << (mh.used ? '1' : '0') << '|'
       << status << '\n';

    std::string recordStr = ss.str();
    file.seekp(offset);
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();
    return true;
}

bool StorageManager::appendSubject(const MonHoc& mh, int64_t& outOffset) {
    std::string filePath = PathResolver::getFilePath("subjects.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::ofstream newFile(filePath, std::ios::out | std::ios::binary);
        newFile << "MAMH           |TENMH                                             |U|S\n";
        newFile.close();
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    }
    if (!file.is_open()) return false;

    file.seekp(0, std::ios::end);
    outOffset = file.tellp();

    std::ostringstream ss;
    ss << std::left << std::setw(15) << mh.MAMH << '|'
       << std::setw(50) << mh.TENMH << '|'
       << (mh.used ? '1' : '0') << '|'
       << STATUS_ACTIVE << '\n';

    std::string recordStr = ss.str();
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();

    IndexManager::getInstance().updateSubjectOffset(mh.MAMH, outOffset);
    return true;
}

bool StorageManager::markSubjectStatusAt(int64_t offset, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("subjects.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekp(offset + StorageConfig::STATUS_OFFSET_SUBJECT);
    file.write(&status, 1);
    file.flush();
    file.close();

    if (status == STATUS_DELETED || status == STATUS_SOFT_DELETED) {
        incrementDeletedCount("subject");
    }
    return true;
}

bool StorageManager::readClassAt(int64_t offset, Lop& outLop) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("classes.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekg(offset);
    std::string line;
    if (!std::getline(file, line)) {
        file.close();
        return false;
    }
    file.close();

    DArray<std::string> tokens = split(line, '|');
    if (tokens.size() >= 3) {
        std::string status = trim(tokens[2]);
        if (status == "1") return false;

        outLop.MALOP = trim(tokens[0]);
        outLop.TENLOP = trim(tokens[1]);
        return true;
    }
    return false;
}

bool StorageManager::writeClassAt(int64_t offset, const Lop& lop, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("classes.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    std::ostringstream ss;
    ss << std::left << std::setw(15) << lop.MALOP << '|'
       << std::setw(50) << lop.TENLOP << '|'
       << status << '\n';

    std::string recordStr = ss.str();
    file.seekp(offset);
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();
    return true;
}

bool StorageManager::appendClass(const Lop& lop, int64_t& outOffset) {
    std::string filePath = PathResolver::getFilePath("classes.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::ofstream newFile(filePath, std::ios::out | std::ios::binary);
        newFile << "MALOP          |TENLOP                                            |S\n";
        newFile.close();
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
    }
    if (!file.is_open()) return false;

    file.seekp(0, std::ios::end);
    outOffset = file.tellp();

    std::ostringstream ss;
    ss << std::left << std::setw(15) << lop.MALOP << '|'
       << std::setw(50) << lop.TENLOP << '|'
       << STATUS_ACTIVE << '\n';

    std::string recordStr = ss.str();
    file.write(recordStr.c_str(), recordStr.length());
    file.flush();
    file.close();

    IndexManager::getInstance().updateClassOffset(lop.MALOP, outOffset);
    return true;
}


bool StorageManager::markClassStatusAt(int64_t offset, char status) {
    if (offset < 0) return false;
    std::string filePath = PathResolver::getFilePath("classes.txt");
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    file.seekp(offset + StorageConfig::STATUS_OFFSET_CLASS);
    file.write(&status, 1);
    file.flush();
    file.close();

    if (status == STATUS_DELETED || status == STATUS_SOFT_DELETED) {
        incrementDeletedCount("class");
    }
    return true;
}

// ============================================================
// METADATA DELETED COUNTER TRACKING & STARTUP COMPACTION CHECK
// ============================================================

void StorageManager::incrementDeletedCount(const std::string& entityType) {
    if (entityType == "student") {
        s_deletedStudentCount++;
        if (s_deletedStudentCount >= StorageConfig::STUDENT_COMPACT_COUNT) {
            compactStudents();
        }
    }
    else if (entityType == "question") {
        s_deletedQuestionCount++;
        if (s_deletedQuestionCount >= StorageConfig::QUESTION_COMPACT_COUNT) {
            compactQuestions();
        }
    }
    else if (entityType == "subject") {
        s_deletedSubjectCount++;
        if (s_deletedSubjectCount >= StorageConfig::SUBJECT_COMPACT_COUNT) {
            compactSubjects();
        }
    }
    else if (entityType == "class") {
        s_deletedClassCount++;
        if (s_deletedClassCount >= StorageConfig::CLASS_COMPACT_COUNT) {
            compactClasses();
        }
    }

    // Update metadata.txt
    saveMetadata();
}

int StorageManager::getDeletedCount(const std::string& entityType) const {
    if (entityType == "student") return s_deletedStudentCount;
    if (entityType == "question") return s_deletedQuestionCount;
    if (entityType == "subject") return s_deletedSubjectCount;
    if (entityType == "class") return s_deletedClassCount;
    return 0;
}

void StorageManager::resetDeletedCount(const std::string& entityType) {
    if (entityType == "student") s_deletedStudentCount = 0;
    else if (entityType == "question") s_deletedQuestionCount = 0;
    else if (entityType == "subject") s_deletedSubjectCount = 0;
    else if (entityType == "class") s_deletedClassCount = 0;
    else {
        s_deletedStudentCount = 0;
        s_deletedQuestionCount = 0;
        s_deletedSubjectCount = 0;
        s_deletedClassCount = 0;
    }

    saveMetadata();
}

static int countDeletedInFile(const std::string& filename, int statusTokenIdx) {
    std::string filePath = PathResolver::getFilePath(filename);
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return 0;
    std::string line;
    bool isHeader = true;
    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (isHeader) { isHeader = false; continue; }
        DArray<std::string> tokens = split(line, '|');
        if (tokens.size() > (size_t)statusTokenIdx) {
            std::string st = trim(tokens[statusTokenIdx]);
            if (st == "1" || st == "2") {
                count++;
            }
        }
    }
    file.close();
    return count;
}

bool StorageManager::checkAndExecuteStartupCompaction() {
    s_deletedClassCount = countDeletedInFile("classes.txt", 2);
    s_deletedStudentCount = countDeletedInFile("students.txt", 6);
    s_deletedSubjectCount = countDeletedInFile("subjects.txt", 3);
    s_deletedQuestionCount = countDeletedInFile("questions.txt", 8);

    std::string metaPath = PathResolver::getFilePath("metadata.txt");
    std::ifstream metaFile(metaPath);
    if (metaFile.is_open()) {
        std::string line;
        while (std::getline(metaFile, line)) {
            if (line.find("DELETED_STUDENT_COUNT=") == 0) {
                try { int val = std::stoi(line.substr(22)); if (val > s_deletedStudentCount) s_deletedStudentCount = val; } catch (...) {}
            } else if (line.find("DELETED_QUESTION_COUNT=") == 0) {
                try { int val = std::stoi(line.substr(23)); if (val > s_deletedQuestionCount) s_deletedQuestionCount = val; } catch (...) {}
            } else if (line.find("DELETED_SUBJECT_COUNT=") == 0) {
                try { int val = std::stoi(line.substr(22)); if (val > s_deletedSubjectCount) s_deletedSubjectCount = val; } catch (...) {}
            } else if (line.find("DELETED_CLASS_COUNT=") == 0) {
                try { int val = std::stoi(line.substr(20)); if (val > s_deletedClassCount) s_deletedClassCount = val; } catch (...) {}
            }
        }
        metaFile.close();
    }

    if (s_deletedClassCount >= StorageConfig::CLASS_COMPACT_COUNT) {
        compactClasses();
    }
    if (s_deletedStudentCount >= StorageConfig::STUDENT_COMPACT_COUNT) {
        compactStudents();
    }
    if (s_deletedSubjectCount >= StorageConfig::SUBJECT_COMPACT_COUNT) {
        compactSubjects();
    }
    if (s_deletedQuestionCount >= StorageConfig::QUESTION_COMPACT_COUNT) {
        compactQuestions();
    }

    return true;
}


// ============================================================
// STORAGE COMPACTION ENGINE
// ============================================================

bool StorageManager::compactStudents() {
    std::string filePath = PathResolver::getFilePath("students.txt");
    std::ifstream in(filePath, std::ios::in | std::ios::binary);
    if (!in.is_open()) return false;

    std::ostringstream ss;
    std::string line;
    bool isHeader = true;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            ss << line << "\n";
            isHeader = false;
            continue;
        }
        DArray<std::string> tokens = split(line, '|');
        if (tokens.size() >= 7) {
            std::string status = trim(tokens[6]);
            if (status != "1") { // Preserve Active ('0') and Soft Deleted ('2')
                ss << line << "\n";
            }
        }
    }
    in.close();

    if (!atomicWriteFile(filePath, ss.str())) return false;
    IndexManager::getInstance().rebuildStudentIndex();
    IndexManager::getInstance().saveStudentIndex();
    resetDeletedCount("student");
    return true;
}

bool StorageManager::compactQuestions() {
    std::string filePath = PathResolver::getFilePath("questions.txt");
    std::ifstream in(filePath, std::ios::in | std::ios::binary);
    if (!in.is_open()) return false;

    std::ostringstream ss;
    std::string line;
    bool isHeader = true;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            ss << line << "\n";
            isHeader = false;
            continue;
        }
        DArray<std::string> tokens = split(line, '|');
        if (tokens.size() >= 9) {
            std::string status = trim(tokens[8]);
            if (status != "1") { // Drop Deleted Unused ('1'), Preserve Active ('0') and Soft Deleted Used ('2')
                ss << line << "\n";
            }
        }
    }
    in.close();

    if (!atomicWriteFile(filePath, ss.str())) return false;
    IndexManager::getInstance().rebuildQuestionIndex();
    IndexManager::getInstance().saveQuestionIndex();
    resetDeletedCount("question");
    return true;
}

bool StorageManager::compactSubjects() {
    std::string filePath = PathResolver::getFilePath("subjects.txt");
    std::ifstream in(filePath, std::ios::in | std::ios::binary);
    if (!in.is_open()) return false;

    std::ostringstream ss;
    std::string line;
    bool isHeader = true;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            ss << line << "\n";
            isHeader = false;
            continue;
        }
        DArray<std::string> tokens = split(line, '|');
        if (tokens.size() >= 4) {
            std::string status = trim(tokens[3]);
            if (status != "1") {
                ss << line << "\n";
            }
        }
    }
    in.close();

    if (!atomicWriteFile(filePath, ss.str())) return false;
    IndexManager::getInstance().rebuildSubjectIndex();
    IndexManager::getInstance().saveSubjectIndex();
    resetDeletedCount("subject");
    return true;
}

bool StorageManager::compactClasses() {
    std::string filePath = PathResolver::getFilePath("classes.txt");
    std::ifstream in(filePath, std::ios::in | std::ios::binary);
    if (!in.is_open()) return false;

    std::ostringstream ss;
    std::string line;
    bool isHeader = true;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (isHeader) {
            ss << line << "\n";
            isHeader = false;
            continue;
        }
        DArray<std::string> tokens = split(line, '|');
        if (tokens.size() >= 3) {
            std::string status = trim(tokens[2]);
            if (status != "1") {
                ss << line << "\n";
            }
        }
    }
    in.close();

    if (!atomicWriteFile(filePath, ss.str())) return false;
    IndexManager::getInstance().rebuildClassIndex();
    IndexManager::getInstance().saveClassIndex();
    resetDeletedCount("class");
    return true;
}

bool StorageManager::compactAll() {
    bool sOk = compactStudents();
    bool qOk = compactQuestions();
    bool subOk = compactSubjects();
    bool cOk = compactClasses();
    resetDeletedCount("all");
    return sOk && qOk && subOk && cOk;
}

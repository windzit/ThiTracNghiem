#include "../include/IndexManager.h"
#include "../include/PathResolver.h"
#include "../include/Utils.h"
#include "../include/StorageManager.h"
#include "../include/CommonTypes.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>

namespace fs = std::filesystem;

IndexManager& IndexManager::getInstance() {
    static IndexManager instance;
    return instance;
}

void IndexManager::clear() {
    m_questionIndex.clear();
    m_questionSubjectIndex.clear();
    m_studentIndex.clear();
    m_subjectIndex.clear();
    m_classIndex.clear();
    m_historyIndex.clear();
}

bool IndexManager::auditAndLoadIndexes() {
    std::cout << "\n======================================================================\n";
    std::cout << "                  PTIT CBT SERVER STARTUP REPORT                      \n";
    std::cout << "======================================================================\n";
    std::cout << "[INDEX AUDIT] Checking runtime indexes (.idx)...\n";

    bool anyRebuilt = false;

    // 1. Class Index
    std::string classIdx = PathResolver::getIndexPath("class.idx");
    if (fs::exists(classIdx)) {
        loadClassIndex();
        std::cout << "  [*] Class Index     : OK (loaded from class.idx)\n";
    } else {
        std::cout << "  [*] Class Index     : MISSING -> Rebuilding class.idx... ";
        rebuildClassIndex();
        saveClassIndex();
        std::cout << "DONE\n";
        anyRebuilt = true;
    }

    // 2. Student Index
    std::string studentIdx = PathResolver::getIndexPath("student.idx");
    if (fs::exists(studentIdx)) {
        loadStudentIndex();
        std::cout << "  [*] Student Index   : OK (loaded from student.idx)\n";
    } else {
        std::cout << "  [*] Student Index   : MISSING -> Rebuilding student.idx... ";
        rebuildStudentIndex();
        saveStudentIndex();
        std::cout << "DONE\n";
        anyRebuilt = true;
    }

    // 3. Subject Index
    std::string subjectIdx = PathResolver::getIndexPath("subject.idx");
    if (fs::exists(subjectIdx)) {
        loadSubjectIndex();
        std::cout << "  [*] Subject Index   : OK (loaded from subject.idx)\n";
    } else {
        std::cout << "  [*] Subject Index   : MISSING -> Rebuilding subject.idx... ";
        rebuildSubjectIndex();
        saveSubjectIndex();
        std::cout << "DONE\n";
        anyRebuilt = true;
    }

    // 4. Question Index
    std::string questionIdx = PathResolver::getIndexPath("question.idx");
    if (fs::exists(questionIdx)) {
        loadQuestionIndex();
        std::cout << "  [*] Question Index  : OK (loaded from question.idx)\n";
    } else {
        std::cout << "  [*] Question Index  : MISSING -> Rebuilding question.idx... ";
        rebuildQuestionIndex();
        saveQuestionIndex();
        std::cout << "DONE\n";
        anyRebuilt = true;
    }

    // 5. History Index
    std::string historyIdx = PathResolver::getIndexPath("history.idx");
    if (fs::exists(historyIdx)) {
        loadHistoryIndex();
        std::cout << "  [*] History Index   : OK (loaded from history.idx)\n";
    } else {
        std::cout << "  [*] History Index   : MISSING -> Rebuilding history.idx... ";
        rebuildHistoryIndex();
        saveHistoryIndex();
        std::cout << "DONE\n";
        anyRebuilt = true;
    }

    std::cout << "  --------------------------------------------------------------------\n";
    if (anyRebuilt) {
        std::cout << "  [*] Index Status    : Missing indexes rebuilt successfully.\n\n";
    } else {
        std::cout << "  [*] Index Status    : All indexes up to date (No rebuild needed).\n\n";
    }

    return true;
}

bool IndexManager::rebuildAllIndexes() {
    bool qOk = rebuildQuestionIndex();
    bool sOk = rebuildStudentIndex();
    bool subOk = rebuildSubjectIndex();
    bool cOk = rebuildClassIndex();
    bool hOk = rebuildHistoryIndex();
    saveQuestionIndex();
    saveStudentIndex();
    saveSubjectIndex();
    saveClassIndex();
    saveHistoryIndex();
    return qOk && sOk && subOk && cOk && hOk;
}

bool IndexManager::rebuildQuestionIndex() {
    m_questionIndex.clear();
    m_questionSubjectIndex.clear();
    std::string filePath = PathResolver::getFilePath("questions.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (file) {
        int64_t offset = file.tellg();
        if (offset < 0) break;
        if (!std::getline(file, line)) break;

        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) {
            isHeader = false;
            continue;
        }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 3) {
            // Status is last column (column 8)
            std::string status = trim(tokens[tokens.size() - 1]);
            if (status == "1") continue; // Skip STATUS_DELETED ('1')

            std::string mamh = trim(tokens[0]);
            std::string idStr = trim(tokens[1]);
            try {
                int id = std::stoi(idStr);
                m_questionIndex.insert(id, offset);
                if (!mamh.empty()) {
                    m_questionSubjectIndex.insert(id, mamh);
                }
            } catch (const std::exception& e) {
                std::cerr << "[IndexManager] Warning: Failed to parse rebuildQuestionIndex line: '" << trimmed << "' (" << e.what() << ")" << std::endl;
            } catch (...) {
                std::cerr << "[IndexManager] Warning: Failed to parse rebuildQuestionIndex line: '" << trimmed << "'" << std::endl;
            }
        }
    }
    file.close();
    return true;
}

bool IndexManager::rebuildStudentIndex() {
    m_studentIndex.clear();
    std::string filePath = PathResolver::getFilePath("students.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (file) {
        int64_t offset = file.tellg();
        if (offset < 0) break;
        if (!std::getline(file, line)) break;

        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) {
            isHeader = false;
            continue;
        }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 3) {
            std::string status = trim(tokens[tokens.size() - 1]);
            if (status == "1") continue; // Skip STATUS_DELETED ('1')

            std::string masv = trim(tokens[1]); // MALOP|MASV|HO|TEN|PHAI|PASSWORD|STATUS
            if (!masv.empty()) {
                m_studentIndex.insert(masv, offset);
            }
        }
    }
    file.close();
    return true;
}

bool IndexManager::rebuildSubjectIndex() {
    m_subjectIndex.clear();
    std::string filePath = PathResolver::getFilePath("subjects.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (file) {
        int64_t offset = file.tellg();
        if (offset < 0) break;
        if (!std::getline(file, line)) break;

        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) {
            isHeader = false;
            continue;
        }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string status = trim(tokens[tokens.size() - 1]);
            if (status == "1") continue;

            std::string mamh = trim(tokens[0]);
            if (!mamh.empty()) {
                m_subjectIndex.insert(mamh, offset);
            }
        }
    }
    file.close();
    return true;
}

bool IndexManager::rebuildClassIndex() {
    m_classIndex.clear();
    std::string filePath = PathResolver::getFilePath("classes.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (file) {
        int64_t offset = file.tellg();
        if (offset < 0) break;
        if (!std::getline(file, line)) break;

        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) {
            isHeader = false;
            continue;
        }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string status = trim(tokens[tokens.size() - 1]);
            if (status == "1") continue;

            std::string malop = trim(tokens[0]);
            if (!malop.empty()) {
                m_classIndex.insert(malop, offset);
            }
        }
    }
    file.close();
    return true;
}

bool IndexManager::rebuildHistoryIndex() {
    m_historyIndex.clear();
    std::string filePath = PathResolver::getFilePath("exam_history.txt");
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

    std::string line;
    bool isHeader = true;
    while (file) {
        int64_t offset = file.tellg();
        if (offset < 0) break;
        if (!std::getline(file, line)) break;

        std::string trimmed = trim(line);
        if (trimmed.empty()) continue;
        if (isHeader) {
            isHeader = false;
            continue;
        }

        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 1) {
            std::string masv = trim(tokens[0]);
            if (!masv.empty()) {
                DArray<int64_t>* offsetsPtr = m_historyIndex.find(masv);
                if (offsetsPtr) {
                    offsetsPtr->push_back(offset);
                } else {
                    DArray<int64_t> newArr;
                    newArr.push_back(offset);
                    m_historyIndex.insert(masv, newArr);
                }
            }
        }
    }
    file.close();
    return true;
}

bool IndexManager::saveQuestionIndex() {
    std::ostringstream ss;
    ss << "# QUESTION_ID|OFFSET|MAMH\n";
    m_questionIndex.forEach([this, &ss](int id, int64_t offset) {
        std::string mamh;
        std::string* mamhPtr = m_questionSubjectIndex.find(id);
        if (mamhPtr) mamh = *mamhPtr;
        ss << id << '|' << offset << '|' << mamh << "\n";
    });
    return StorageManager::atomicWriteFile(PathResolver::getIndexPath("question.idx"), ss.str());
}

bool IndexManager::loadQuestionIndex() {
    m_questionIndex.clear();
    m_questionSubjectIndex.clear();
    std::string indexPath = PathResolver::getIndexPath("question.idx");
    std::ifstream file(indexPath);
    if (!file.is_open()) {
        return rebuildQuestionIndex() && saveQuestionIndex();
    }

    std::string line;
    bool validIndex = true;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            try {
                int id = std::stoi(trim(tokens[0]));
                int64_t offset = std::stoll(trim(tokens[1]));
                m_questionIndex.insert(id, offset);
                if (tokens.size() >= 3) {
                    std::string mamh = trim(tokens[2]);
                    if (!mamh.empty()) {
                        m_questionSubjectIndex.insert(id, mamh);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "[IndexManager] Warning: Failed to parse question.idx line: '" << trimmed << "' (" << e.what() << ")" << std::endl;
                validIndex = false;
            } catch (...) {
                std::cerr << "[IndexManager] Warning: Failed to parse question.idx line: '" << trimmed << "'" << std::endl;
                validIndex = false;
            }
        }
    }
    file.close();

    // If question.idx is missing 3rd column MAMH mapping (legacy 2-column index) or had parse errors, trigger rebuild
    if (m_questionSubjectIndex.empty() && !m_questionIndex.empty()) {
        std::cout << "  [*] Question Index  : Upgrade 2-column -> 3-column index (SCHEMA 2.0)...\n";
        return rebuildQuestionIndex() && saveQuestionIndex();
    }

    if (!validIndex) {
        std::cout << "  [*] Question Index  : Parse warning -> Rebuilding question.idx...\n";
        return rebuildQuestionIndex() && saveQuestionIndex();
    }

    return true;
}

bool IndexManager::saveStudentIndex() {
    std::ostringstream ss;
    ss << "# MASV|OFFSET\n";
    m_studentIndex.forEach([&ss](const std::string& masv, int64_t offset) {
        ss << masv << '|' << offset << "\n";
    });
    return StorageManager::atomicWriteFile(PathResolver::getIndexPath("student.idx"), ss.str());
}

bool IndexManager::loadStudentIndex() {
    m_studentIndex.clear();
    std::string indexPath = PathResolver::getIndexPath("student.idx");
    std::ifstream file(indexPath);
    if (!file.is_open()) {
        return rebuildStudentIndex() && saveStudentIndex();
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string masv = trim(tokens[0]);
            try {
                int64_t offset = std::stoll(trim(tokens[1]));
                m_studentIndex.insert(masv, offset);
            } catch (...) {}
        }
    }
    file.close();
    return true;
}

bool IndexManager::saveSubjectIndex() {
    std::ostringstream ss;
    ss << "# MAMH|OFFSET\n";
    m_subjectIndex.forEach([&ss](const std::string& mamh, int64_t offset) {
        ss << mamh << '|' << offset << "\n";
    });
    return StorageManager::atomicWriteFile(PathResolver::getIndexPath("subject.idx"), ss.str());
}

bool IndexManager::loadSubjectIndex() {
    m_subjectIndex.clear();
    std::string indexPath = PathResolver::getIndexPath("subject.idx");
    std::ifstream file(indexPath);
    if (!file.is_open()) {
        return rebuildSubjectIndex() && saveSubjectIndex();
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string mamh = trim(tokens[0]);
            try {
                int64_t offset = std::stoll(trim(tokens[1]));
                m_subjectIndex.insert(mamh, offset);
            } catch (...) {}
        }
    }
    file.close();
    return true;
}

bool IndexManager::saveClassIndex() {
    std::ostringstream ss;
    ss << "# MALOP|OFFSET\n";
    m_classIndex.forEach([&ss](const std::string& malop, int64_t offset) {
        ss << malop << '|' << offset << "\n";
    });
    return StorageManager::atomicWriteFile(PathResolver::getIndexPath("class.idx"), ss.str());
}

bool IndexManager::loadClassIndex() {
    m_classIndex.clear();
    std::string indexPath = PathResolver::getIndexPath("class.idx");
    std::ifstream file(indexPath);
    if (!file.is_open()) {
        return rebuildClassIndex() && saveClassIndex();
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string malop = trim(tokens[0]);
            try {
                int64_t offset = std::stoll(trim(tokens[1]));
                m_classIndex.insert(malop, offset);
            } catch (...) {}
        }
    }
    file.close();
    return true;
}

bool IndexManager::saveHistoryIndex() {
    std::ostringstream ss;
    ss << "# MASV|OFFSETS\n";
    m_historyIndex.forEach([&ss](const std::string& masv, const DArray<int64_t>& offsets) {
        ss << masv << '|';
        for (int i = 0; i < offsets.size(); i++) {
            ss << offsets[i];
            if (i + 1 < offsets.size()) ss << ',';
        }
        ss << "\n";
    });
    return StorageManager::atomicWriteFile(PathResolver::getIndexPath("history.idx"), ss.str());
}

bool IndexManager::loadHistoryIndex() {
    m_historyIndex.clear();
    std::string indexPath = PathResolver::getIndexPath("history.idx");
    std::ifstream file(indexPath);
    if (!file.is_open()) {
        return rebuildHistoryIndex() && saveHistoryIndex();
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            std::string masv = trim(tokens[0]);
            DArray<std::string> offsetTokens = split(trim(tokens[1]), ',');
            DArray<int64_t> offsets;
            for (int i = 0; i < offsetTokens.size(); i++) {
                try {
                    offsets.push_back(std::stoll(trim(offsetTokens[i])));
                } catch (...) {}
            }
            m_historyIndex.insert(masv, offsets);
        }
    }
    file.close();
    return true;
}

bool IndexManager::getQuestionOffset(int id, int64_t& outOffset) const {
    const int64_t* ptr = m_questionIndex.find(id);
    if (ptr) {
        outOffset = *ptr;
        return true;
    }
    return false;
}

bool IndexManager::getQuestionSubject(int id, std::string& outMamh) const {
    const std::string* ptr = m_questionSubjectIndex.find(id);
    if (ptr) {
        outMamh = *ptr;
        return true;
    }
    return false;
}

bool IndexManager::getStudentOffset(const std::string& masv, int64_t& outOffset) const {
    const int64_t* ptr = m_studentIndex.find(masv);
    if (ptr) {
        outOffset = *ptr;
        return true;
    }
    return false;
}

bool IndexManager::getSubjectOffset(const std::string& mamh, int64_t& outOffset) const {
    const int64_t* ptr = m_subjectIndex.find(mamh);
    if (ptr) {
        outOffset = *ptr;
        return true;
    }
    return false;
}

bool IndexManager::getClassOffset(const std::string& malop, int64_t& outOffset) const {
    const int64_t* ptr = m_classIndex.find(malop);
    if (ptr) {
        outOffset = *ptr;
        return true;
    }
    return false;
}

bool IndexManager::getHistoryOffsets(const std::string& masv, DArray<int64_t>& outOffsets) const {
    const DArray<int64_t>* ptr = m_historyIndex.find(masv);
    if (ptr) {
        outOffsets = *ptr;
        return true;
    }
    return false;
}

void IndexManager::updateQuestionOffset(int id, int64_t offset) {
    m_questionIndex.insert(id, offset);
    saveQuestionIndex();
}

void IndexManager::removeQuestionOffset(int id) {
    m_questionIndex.remove(id);
    saveQuestionIndex();
}

void IndexManager::updateQuestionSubject(int id, const std::string& mamh) {
    m_questionSubjectIndex.insert(id, mamh);
}

void IndexManager::removeQuestionSubject(int id) {
    m_questionSubjectIndex.remove(id);
}

void IndexManager::updateStudentOffset(const std::string& masv, int64_t offset) {
    m_studentIndex.insert(masv, offset);
    saveStudentIndex();
}

void IndexManager::removeStudentOffset(const std::string& masv) {
    m_studentIndex.remove(masv);
    saveStudentIndex();
}

void IndexManager::updateSubjectOffset(const std::string& mamh, int64_t offset) {
    m_subjectIndex.insert(mamh, offset);
    saveSubjectIndex();
}

void IndexManager::removeSubjectOffset(const std::string& mamh) {
    m_subjectIndex.remove(mamh);
    saveSubjectIndex();
}

void IndexManager::updateClassOffset(const std::string& malop, int64_t offset) {
    m_classIndex.insert(malop, offset);
    saveClassIndex();
}

void IndexManager::removeClassOffset(const std::string& malop) {
    m_classIndex.remove(malop);
    saveClassIndex();
}

void IndexManager::appendHistoryOffset(const std::string& masv, int64_t offset) {
    DArray<int64_t>* offsetsPtr = m_historyIndex.find(masv);
    if (offsetsPtr) {
        offsetsPtr->push_back(offset);
    } else {
        DArray<int64_t> newArr;
        newArr.push_back(offset);
        m_historyIndex.insert(masv, newArr);
    }
    saveHistoryIndex();
}

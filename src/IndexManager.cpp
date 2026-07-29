#include "../include/IndexManager.h"
#include "../include/PathResolver.h"
#include "../include/Utils.h"
#include "../include/StorageManager.h"
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
    m_studentIndex.clear();
    m_historyIndex.clear();
}

bool IndexManager::rebuildAllIndexes() {
    bool qOk = rebuildQuestionIndex();
    bool sOk = rebuildStudentIndex();
    bool hOk = rebuildHistoryIndex();
    saveQuestionIndex();
    saveStudentIndex();
    saveHistoryIndex();
    return qOk && sOk && hOk;
}

bool IndexManager::rebuildQuestionIndex() {
    m_questionIndex.clear();
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
        if (tokens.size() >= 2) {
            std::string idStr = trim(tokens[1]);
            if (idStr.empty() || !std::isdigit(static_cast<unsigned char>(idStr[0]))) {
                idStr = trim(tokens[0]); // fallback if ID is first column
            }
            try {
                int id = std::stoi(idStr);
                m_questionIndex.insert(id, offset);
            } catch (...) {
                // Ignore non-numeric ID parse failures
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
        if (tokens.size() >= 2) {
            std::string masv = trim(tokens[1]); // MALOP|MASV|HO|TEN|PHAI|PASSWORD
            if (masv.empty()) {
                masv = trim(tokens[0]); // Fallback if MASV is first column
            }
            if (!masv.empty()) {
                m_studentIndex.insert(masv, offset);
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
            std::string masv = trim(tokens[0]); // MASV|MAMH|THOIGIAN_BATDAU|DIEM|...
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
    ss << "# QUESTION_ID|OFFSET\n";
    m_questionIndex.forEach([&ss](int id, int64_t offset) {
        ss << id << '|' << offset << "\n";
    });
    return StorageManager::atomicWriteFile(PathResolver::getIndexPath("question.idx"), ss.str());
}

bool IndexManager::loadQuestionIndex() {
    m_questionIndex.clear();
    std::string indexPath = PathResolver::getIndexPath("question.idx");
    std::ifstream file(indexPath);
    if (!file.is_open()) {
        return rebuildQuestionIndex() && saveQuestionIndex();
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;
        DArray<std::string> tokens = split(trimmed, '|');
        if (tokens.size() >= 2) {
            try {
                int id = std::stoi(trim(tokens[0]));
                int64_t offset = std::stoll(trim(tokens[1]));
                m_questionIndex.insert(id, offset);
            } catch (...) {}
        }
    }
    file.close();
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

bool IndexManager::getStudentOffset(const std::string& masv, int64_t& outOffset) const {
    const int64_t* ptr = m_studentIndex.find(masv);
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

void IndexManager::updateStudentOffset(const std::string& masv, int64_t offset) {
    m_studentIndex.insert(masv, offset);
    saveStudentIndex();
}

void IndexManager::removeStudentOffset(const std::string& masv) {
    m_studentIndex.remove(masv);
    saveStudentIndex();
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

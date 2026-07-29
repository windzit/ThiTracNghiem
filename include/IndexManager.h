#pragma once
#include "HashTable.h"
#include "DArray.h"
#include <string>
#include <cstdint>

class IndexManager {
public:
    static IndexManager& getInstance();

    // Rebuild all indexes from source .txt data files
    bool rebuildAllIndexes();

    // Rebuild specific entity indexes
    bool rebuildQuestionIndex();
    bool rebuildStudentIndex();
    bool rebuildHistoryIndex();

    // Save / Load index accelerator files (.idx)
    bool saveQuestionIndex();
    bool loadQuestionIndex();

    bool saveStudentIndex();
    bool loadStudentIndex();

    bool saveHistoryIndex();
    bool loadHistoryIndex();

    // Lookup functions
    bool getQuestionOffset(int id, int64_t& outOffset) const;
    bool getStudentOffset(const std::string& masv, int64_t& outOffset) const;
    bool getHistoryOffsets(const std::string& masv, DArray<int64_t>& outOffsets) const;

    // Mutation update functions
    void updateQuestionOffset(int id, int64_t offset);
    void removeQuestionOffset(int id);

    void updateStudentOffset(const std::string& masv, int64_t offset);
    void removeStudentOffset(const std::string& masv);

    void appendHistoryOffset(const std::string& masv, int64_t offset);

    // Index metrics
    size_t getQuestionIndexCount() const { return m_questionIndex.size(); }
    size_t getStudentIndexCount() const { return m_studentIndex.size(); }
    size_t getHistoryIndexCount() const { return m_historyIndex.size(); }

    void clear();

private:
    IndexManager() = default;
    IndexManager(const IndexManager&) = delete;
    IndexManager& operator=(const IndexManager&) = delete;

    HashTable<int, int64_t> m_questionIndex;                      // Question ID -> Byte Offset
    HashTable<std::string, int64_t> m_studentIndex;               // Student MASV -> Byte Offset
    HashTable<std::string, DArray<int64_t>> m_historyIndex;        // Student MASV -> DArray of Offsets
};

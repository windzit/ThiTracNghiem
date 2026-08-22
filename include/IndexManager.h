#pragma once
#include "HashTable.h"
#include "DArray.h"
#include <string>
#include <cstdint>

class IndexManager {
public:
    static IndexManager& getInstance();

    // Audit & Load indexes at startup with terminal report
    bool auditAndLoadIndexes();

    // Rebuild all indexes from source .txt data files
    bool rebuildAllIndexes();

    // Rebuild specific entity indexes
    bool rebuildQuestionIndex();
    bool rebuildStudentIndex();
    bool rebuildSubjectIndex();
    bool rebuildClassIndex();
    bool rebuildHistoryIndex();

    // Save / Load index accelerator files (.idx)
    bool saveQuestionIndex();
    bool loadQuestionIndex();

    bool saveStudentIndex();
    bool loadStudentIndex();

    bool saveSubjectIndex();
    bool loadSubjectIndex();

    bool saveClassIndex();
    bool loadClassIndex();

    bool saveHistoryIndex();
    bool loadHistoryIndex();

    // Lookup functions
    bool getQuestionOffset(int id, int64_t& outOffset) const;
    bool getQuestionSubject(int id, std::string& outMamh) const;
    bool getStudentOffset(const std::string& masv, int64_t& outOffset) const;
    bool getSubjectOffset(const std::string& mamh, int64_t& outOffset) const;
    bool getClassOffset(const std::string& malop, int64_t& outOffset) const;
    bool getHistoryOffsets(const std::string& masv, DArray<int64_t>& outOffsets) const;

    // Mutation update functions
    void updateQuestionOffset(int id, int64_t offset);
    void removeQuestionOffset(int id);
    void updateQuestionSubject(int id, const std::string& mamh);
    void removeQuestionSubject(int id);

    void updateStudentOffset(const std::string& masv, int64_t offset);
    void removeStudentOffset(const std::string& masv);

    void updateSubjectOffset(const std::string& mamh, int64_t offset);
    void removeSubjectOffset(const std::string& mamh);

    void updateClassOffset(const std::string& malop, int64_t offset);
    void removeClassOffset(const std::string& malop);

    void appendHistoryOffset(const std::string& masv, int64_t offset);

    void clear();

private:
    IndexManager() = default;
    IndexManager(const IndexManager&) = delete;
    IndexManager& operator=(const IndexManager&) = delete;

    HashTable<int, int64_t> m_questionIndex;                      // Question ID -> Byte Offset
    HashTable<int, std::string> m_questionSubjectIndex;           // Question ID -> MAMH
    HashTable<std::string, int64_t> m_studentIndex;               // Student MASV -> Byte Offset
    HashTable<std::string, int64_t> m_subjectIndex;               // Subject MAMH -> Byte Offset
    HashTable<std::string, int64_t> m_classIndex;                 // Class MALOP -> Byte Offset
    HashTable<std::string, DArray<int64_t>> m_historyIndex;        // Student MASV -> DArray of Offsets
};


